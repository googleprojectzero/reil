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

#include "disassembler/first_pass_disassembler.h"

#include "glog/logging.h"

namespace reil {
namespace disassembler {

FirstPassDisassembler::FirstPassDisassembler(
    std::shared_ptr<MemoryImage> memory_image)
    : memory_image_(memory_image) {}

FirstPassDisassembler::~FirstPassDisassembler() {}

void FirstPassDisassembler::DecodeInstruction(uint64_t address) {
  const uint8_t* bytes = nullptr;
  uint64_t size = 0;
  std::tie(bytes, size) = memory_image_->Read(address);
  if (size >= sizeof(uint32_t)) {
    uint32_t opcode;
    memcpy(&opcode, bytes, sizeof(opcode));
    current_insn_ = aarch64::decoder::DecodeInstruction(address, opcode);
  } else {
    current_insn_.address = address;
    current_insn_.opcode = aarch64::decoder::kUnallocated;
  }
}

bool FirstPassDisassembler::QueueFunction(uint64_t address) {
  if (!functions_.count(address)) {
    VLOG(1) << "Queueing Function: " << std::hex << address;
    function_queue_.insert(address);
    return true;
  }
  return false;
}

bool FirstPassDisassembler::QueueBasicBlock(uint64_t address, bool force) {
  if (force || (!current_fn_->incoming_edges().count(Node(address)) &&
                !functions_.count(address))) {
    VLOG(1) << "Queueing BasicBlock: " << std::hex << address;
    basic_block_queue_.insert(address);
    return true;
  }
  return false;
}

uint64_t FirstPassDisassembler::PcRelativeOffset(
    const aarch64::decoder::Operand& dst) {
  auto offset = absl::get<aarch64::decoder::Immediate>(dst);
  if (offset.value & (1ull << (offset.size - 1))) {
    return current_insn_.address - ((~offset.value) + 1ull);
  } else {
    return current_insn_.address + offset.value;
  }
}

bool FirstPassDisassembler::AnalyseBranch(
    const aarch64::decoder::Operand& dst) {
  uint64_t address;
  if (dst.index() == aarch64::decoder::kRegister) {
    current_fn_->AddEdge(current_insn_.address, 0, kNativeJump);
  } else if (dst.index() == aarch64::decoder::kImmediate) {
    address = PcRelativeOffset(dst);
    QueueBasicBlock(address);
    current_fn_->AddEdge(current_insn_.address, address, kNativeJump);
  }

  VLOG(2) << "----[" << std::hex << address << "]";
  return false;
}

bool FirstPassDisassembler::AnalyseMaybeBranch(
    const aarch64::decoder::Operand& dst) {
  QueueBasicBlock(current_insn_.address + 4);
  current_fn_->AddEdge(current_insn_.address, current_insn_.address + 4,
                       kNativeFlow);

  uint64_t address;
  if (dst.index() == aarch64::decoder::kRegister) {
    current_fn_->AddEdge(current_insn_.address, 0, kNativeJump);
  } else if (dst.index() == aarch64::decoder::kImmediate) {
    address = PcRelativeOffset(dst);
    QueueBasicBlock(address);
    current_fn_->AddEdge(current_insn_.address, address, kNativeJump);
  }

  VLOG(2) << "----[" << std::hex << address << ", " << current_insn_.address + 4
          << "]";
  return false;
}

bool FirstPassDisassembler::AnalyseCall(const aarch64::decoder::Operand& dst) {
  // sort of a philosophical question about whether a call constitutes the end
  // of a basic block...
  QueueBasicBlock(current_insn_.address + 4);
  current_fn_->AddEdge(current_insn_.address, current_insn_.address + 4,
                       kNativeFlow);

  uint64_t address;
  if (dst.index() == aarch64::decoder::kRegister) {
    // TODO:
  } else if (dst.index() == aarch64::decoder::kImmediate) {
    address = PcRelativeOffset(dst);
    QueueFunction(address);
    current_fn_->AddEdge(current_insn_.address, address, kNativeCall);
  }

  VLOG(2) << "----[" << std::hex << address << ", " << current_insn_.address + 4
          << "]";
  return false;
}

bool FirstPassDisassembler::AnalyseOneInstruction(bool basic_block_start) {
  // if this address is the start of an existing basic block then we end the
  // current basic block with a native flow edge into the next one.
  if (!basic_block_start &&
      current_fn_->incoming_edges().count(current_insn_.address)) {
    current_fn_->AddEdge(current_insn_.address - 4, current_insn_.address,
                         kNativeFlow);
    VLOG(2) << "----[" << std::hex << current_insn_.address << "]";
    return false;
  }

  DecodeInstruction(current_insn_.address);
  VLOG(3) << std::hex << current_insn_.address << " " << current_insn_;
  switch (current_insn_.opcode) {
    case aarch64::decoder::kBCond:
      return AnalyseMaybeBranch(current_insn_.operands[0]);

    case aarch64::decoder::kB:
    case aarch64::decoder::kBr:
    case aarch64::decoder::kBraa:
    case aarch64::decoder::kBraaz:
    case aarch64::decoder::kBrab:
    case aarch64::decoder::kBrabz:
      return AnalyseBranch(current_insn_.operands[0]);

    case aarch64::decoder::kBl:
    case aarch64::decoder::kBlr:
    case aarch64::decoder::kBlraa:
    case aarch64::decoder::kBlraaz:
    case aarch64::decoder::kBlrab:
    case aarch64::decoder::kBlrabz:
      return AnalyseCall(current_insn_.operands[0]);

    case aarch64::decoder::kCbnz:
    case aarch64::decoder::kCbz:
      return AnalyseMaybeBranch(current_insn_.operands[1]);

    case aarch64::decoder::kTbnz:
    case aarch64::decoder::kTbz:
      return AnalyseMaybeBranch(current_insn_.operands[2]);

    case aarch64::decoder::kEret:
    case aarch64::decoder::kEretaa:
    case aarch64::decoder::kEretaaz:
    case aarch64::decoder::kEretab:
    case aarch64::decoder::kEretabz:
    case aarch64::decoder::kRet:
    case aarch64::decoder::kRetaa:
    case aarch64::decoder::kRetaaz:
    case aarch64::decoder::kRetab:
    case aarch64::decoder::kRetabz: {
      current_fn_->AddEdge(current_insn_.address, 0, kNativeReturn);
      VLOG(2) << "----[]";
      return false;
    }

    case aarch64::decoder::kBrk: {
      current_fn_->AddEdge(current_insn_.address, 0, kNativeBreak);
      VLOG(2) << "----[X]";
      return false;
    }

    case aarch64::decoder::kUnallocated: {
      current_fn_->AddEdge(current_insn_.address, 0, kInvalid);
      VLOG(2) << "----[X]";
      return false;
    }

    default: {
      current_insn_.address += 4;
      return true;
    }
  }
}

bool FirstPassDisassembler::AnalyseOneBasicBlock() {
  if (!basic_block_queue_.empty()) {
    uint64_t address = *basic_block_queue_.begin();
    basic_block_queue_.erase(basic_block_queue_.begin());

    VLOG(2) << "--" << std::hex << address;

    // check if there's an existing basic block overlapping with the start of
    // this one, and add a new flow edge to split that basic block if necessary
    const auto& in_iter = current_fn_->incoming_edges().lower_bound(address);
    if (in_iter != current_fn_->incoming_edges().end()) {
      const auto& out_iter =
          current_fn_->outgoing_edges().lower_bound(in_iter->first);
      if (out_iter->first >= address) {
        current_fn_->AddEdge(address - 4, address, kNativeFlow);
      }
    }

    current_insn_.address = address;
    if (AnalyseOneInstruction(true)) {
      while (AnalyseOneInstruction(false)) {
      }
    }
  }

  return !basic_block_queue_.empty();
}

bool FirstPassDisassembler::AnalyseOneFunction() {
  if (!function_queue_.empty()) {
    uint64_t address = *function_queue_.begin();
    function_queue_.erase(function_queue_.begin());

    current_fn_ = absl::make_unique<ControlFlowGraph>();
    current_fn_->AddEdge(0, Node(address), kNativeCall);

    QueueBasicBlock(address, true);
    while (AnalyseOneBasicBlock()) {
    }

    functions_[address] = std::move(current_fn_);
  }

  LOG_EVERY_N(INFO, 1000) << "Function queue length: " << std::dec
                          << function_queue_.size();

  return !function_queue_.empty();
}

std::map<uint64_t, std::unique_ptr<ControlFlowGraph>>
FirstPassDisassembler::AnalyseAllFunctions() {
  while (AnalyseOneFunction()) {
  }

  return std::move(functions_);
}
}  // namespace disassembler
}  // namespace reil
