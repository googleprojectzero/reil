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

#ifndef REIL_DISASSEMBLER_AARCH64_DISASSEMBLER_H_

#include <cstdint>
#include <memory>

#include "flow_graph/native_flow_graph.h"
#include "memory_image/memory_image.h"

namespace reil {
namespace disassembler {
namespace aarch64 {
// Find all the likely function entry-points.
std::set<uint64_t> FindFunctions(const MemoryImage& memory_image);

// Basic disassembly in a single pass - adds all flow from address to the
// provided function.
void Disassemble(const MemoryImage& memory_image, NativeFlowGraph& nfg,
                 uint64_t address);
}  // namespace aarch64
}  // namespace disassembler
}  // namespace reil

#define REIL_DISASSEMBLER_AARCH64_DISASSEMBLER_H_
#endif  // REIL_DISASSEMBLER_AARCH64_DISASSEMBLER_H_