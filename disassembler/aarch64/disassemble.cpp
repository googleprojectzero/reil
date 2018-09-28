// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "disassembler/aarch64/disassembler.h"

#include <cstring>

#include "absl/memory/memory.h"
#include "memory_image/memory_image.h"
#include "reil/aarch64/decoder.h"

#include "glog/logging.h"

namespace reil {
namespace disassembler {
namespace aarch64 {

namespace decoder = ::reil::aarch64::decoder;

static decoder::Instruction DecodeInstruction(const MemoryImage& memory_image,
                                              uint64_t address) {
  decoder::Instruction insn;
  absl::Span<const uint8_t> bytes = memory_image.Read(address);
  if (bytes.size() >= sizeof(uint32_t)) {
    uint32_t opcode;
    memcpy(&opcode, bytes.data(), sizeof(opcode));
    insn = decoder::DecodeInstruction(address, opcode);
  } else {
    insn.address = address;
    insn.opcode = aarch64::decoder::kUnallocated;
  }
  return insn;
}

static uint64_t PcRelativeAddress(uint64_t address,
                                  const decoder::Operand& dst) {
  auto offset = absl::get<decoder::Immediate>(dst);
  if (offset.value & (1ull << (offset.size - 1))) {
    return address - ((~offset.value) + 1ull);
  } else {
    return address + offset.value;
  }
}

static void AddBranchEdge(NativeFlowGraph& nfg, uint64_t source,
                          uint64_t target, NativeEdgeKind kind) {
  // a branch edge can split an existing basic block, so we need to be careful
  // and check if we need to add an extra edge.

  uint64_t bb_start = nfg.BasicBlockStart(target);
  if (bb_start != 0 && bb_start < target) {
    nfg.AddEdge(target - 4, target, NativeEdgeKind::kFlow);
  }

  nfg.AddEdge(source, target, kind);
}

static std::set<uint64_t> Branch(NativeFlowGraph& nfg, uint64_t address,
                                 const decoder::Operand& dst) {
  std::set<uint64_t> next_instructions;

  uint64_t target = 0;
  if (dst.index() == decoder::kRegister) {
    // leave a 0 target for the branch resolver to worry about
  } else if (dst.index() == decoder::kImmediate) {
    target = PcRelativeAddress(address, dst);
    next_instructions.insert(target);
  }

  AddBranchEdge(nfg, address, target, NativeEdgeKind::kJump);
  VLOG(2) << "branch-------[" << std::hex << target << "]";

  return next_instructions;
}

static std::set<uint64_t> MaybeBranch(NativeFlowGraph& nfg, uint64_t address,
                                      const decoder::Operand& dst) {
  std::set<uint64_t> next_instructions;

  next_instructions.insert(address + 4);
  nfg.AddEdge(address, address + 4, NativeEdgeKind::kFlow);

  uint64_t target = 0;
  if (dst.index() == decoder::kRegister) {
    // leave a 0 target for the branch resolver to worry about
  } else if (dst.index() == decoder::kImmediate) {
    target = PcRelativeAddress(address, dst);
    next_instructions.insert(target);
  }

  AddBranchEdge(nfg, address, target, NativeEdgeKind::kJump);
  VLOG(2) << "maybe-branch-[" << std::hex << target << ", " << address + 4
          << "]";

  return next_instructions;
}

static std::set<uint64_t> Call(NativeFlowGraph& nfg, uint64_t address,
                               const aarch64::decoder::Operand& dst) {
  std::set<uint64_t> next_instructions;

  next_instructions.insert(address + 4);
  nfg.AddEdge(address, address + 4, NativeEdgeKind::kFlow);

  uint64_t target = 0;
  if (dst.index() == aarch64::decoder::kRegister) {
    // can't do anything more here, leave a 0 target for the branch resolver to
    // figure out later
  } else if (dst.index() == aarch64::decoder::kImmediate) {
    target = PcRelativeAddress(address, dst);
  }

  AddBranchEdge(nfg, address, target, NativeEdgeKind::kCall);
  VLOG(2) << "call---------[" << std::hex << target << ", " << address + 4
          << "]";

  return next_instructions;
}

static std::set<uint64_t> DisassembleInstruction(
    const MemoryImage& memory_image, NativeFlowGraph& nfg, uint64_t address) {
  std::set<uint64_t> next_instructions;

  auto insn = DecodeInstruction(memory_image, address);
  VLOG(3) << std::hex << address << " " << insn;
  switch (insn.opcode) {
    case decoder::kBCond: {
      next_instructions = MaybeBranch(nfg, address, insn.operands[0]);
    } break;

    case decoder::kB:
    case decoder::kBr:
    case decoder::kBraa:
    case decoder::kBraaz:
    case decoder::kBrab:
    case decoder::kBrabz: {
      next_instructions = Branch(nfg, address, insn.operands[0]);
    } break;

    case decoder::kBl:
    case decoder::kBlr:
    case decoder::kBlraa:
    case decoder::kBlraaz:
    case decoder::kBlrab:
    case decoder::kBlrabz: {
      next_instructions = Call(nfg, address, insn.operands[0]);
    } break;

    case decoder::kCbnz:
    case decoder::kCbz: {
      next_instructions = MaybeBranch(nfg, address, insn.operands[1]);
    } break;

    case decoder::kTbnz:
    case decoder::kTbz: {
      next_instructions = MaybeBranch(nfg, address, insn.operands[2]);
    } break;

    case decoder::kEret:
    case decoder::kEretaa:
    case decoder::kEretaaz:
    case decoder::kEretab:
    case decoder::kEretabz:
    case decoder::kRet:
    case decoder::kRetaa:
    case decoder::kRetaaz:
    case decoder::kRetab:
    case decoder::kRetabz: {
      nfg.AddEdge(address, 0, NativeEdgeKind::kReturn);
      VLOG(2) << "return--------[]";
    } break;

    case decoder::kBrk: {
      nfg.AddEdge(address, 0, NativeEdgeKind::kBreak);
      VLOG(2) << "break---------[X]";
    } break;

    case decoder::kUnallocated: {
      nfg.AddEdge(address, 0, NativeEdgeKind::kInvalid);
      VLOG(2) << "invalid-------[X]";
    } break;

    default: {
      // if the next instruction is the start of an existing basic block then we
      // end the current basic block with a native flow edge into it.
      if (nfg.incoming_edges().count(address + 4)) {
        nfg.AddEdge(address, address + 4, NativeEdgeKind::kFlow);
        VLOG(2) << "block-flow---[" << std::hex << address << "]";
      }
      next_instructions.insert(address + 4);
    }
  }

  return next_instructions;
}

void Disassemble(const MemoryImage& memory_image, NativeFlowGraph& nfg,
                 uint64_t address) {
  std::set<uint64_t> queue = {address};
  std::set<uint64_t> visited;

  while (!queue.empty()) {
    uint64_t address = *queue.begin();
    queue.erase(queue.begin());

    if (visited.count(address) == 0) {
      std::set<uint64_t> next_instructions =
          DisassembleInstruction(memory_image, nfg, address);
      queue.insert(next_instructions.begin(), next_instructions.end());
      visited.insert(address);
    }
  }
}
}  // namespace aarch64
}  // namespace disassembler
}  // namespace reil