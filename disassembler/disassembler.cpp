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

#include "disassembler/disassembler.h"

#include "glog/logging.h"

#include "disassembler/aarch64/disassembler.h"

namespace reil {
namespace disassembler {
std::set<uint64_t> FindFunctions(const MemoryImage& memory_image) {
  if (memory_image.architecture_name() == "aarch64") {
    return aarch64::FindFunctions(memory_image);
  } else {
    LOG(FATAL) << "Unsupported memory image architecture: "
               << memory_image.architecture_name();
  }
}

void Disassemble(const MemoryImage& memory_image, NativeFlowGraph& nfg,
                 uint64_t address) {
  if (memory_image.architecture_name() == "aarch64") {
    aarch64::Disassemble(memory_image, nfg, address);
  } else {
    LOG(FATAL) << "Unsupported memory image architecture: "
               << memory_image.architecture_name();
  }
}

std::unique_ptr<NativeFlowGraph> DisassembleFunction(
    const MemoryImage& memory_image, uint64_t address) {
  std::unique_ptr<NativeFlowGraph> nfg = absl::make_unique<NativeFlowGraph>();
  nfg->AddEdge(0, address, NativeEdgeKind::kCall);

  if (memory_image.architecture_name() == "aarch64") {
    Disassemble(memory_image, *nfg, address);
  } else {
    LOG(FATAL) << "Unsupported memory image architecture: "
               << memory_image.architecture_name();
  }

  return nfg;
}

std::map<uint64_t, std::unique_ptr<NativeFlowGraph>> DisassembleFunctions(
    const MemoryImage& memory_image, std::set<uint64_t> address) {
  /*  if (memory_image.architecture_name() == "aarch64") {
      return DisassembleFunction(memory_image, address);
    } else {
      LOG(FATAL) << "Unsupported memory image architecture: "
                 << memory_image.architecture_name();
    }
  */
}
}  // namespace disassembler
}  // namespace reil
