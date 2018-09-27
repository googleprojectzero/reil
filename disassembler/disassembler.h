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

#ifndef REIL_DISASSEMBLER_DISASSEMBLER_H_

#include <cstdint>
#include <memory>

#include "flow_graph/instruction_provider.h"
#include "flow_graph/native_flow_graph.h"
#include "memory_image/memory_image.h"

namespace reil {
namespace disassembler {
std::set<uint64_t> FindFunctions(const MemoryImage& memory_image);

void Disassemble(const MemoryImage& memory_image, NativeFlowGraph& nfg,
                 uint64_t address);

std::unique_ptr<NativeFlowGraph> DisassembleFunction(
    const MemoryImage& memory_image, uint64_t address);

std::map<uint64_t, std::unique_ptr<NativeFlowGraph>> DisassembleFunctions(
    const MemoryImage& memory_image, std::set<uint64_t> addresses);

bool ResolveBranches(const MemoryImage& memory_image,
                     InstructionProvider& instruction_provider,
                     NativeFlowGraph& native_flow_graph);

bool ResolveBranches(const MemoryImage& memory_image,
                     NativeFlowGraph& native_flow_graph);
}  // namespace disassembler
}  // namespace reil

#define REIL_DISASSEMBLER_DISASSEMBLER_H_
#endif  // REIL_DISASSEMBLER_DISASSEMBLER_H_