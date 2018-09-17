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

#ifndef REIL_CONTROL_FLOW_GRAPH_INSTRUCTION_GRAPH_H_

#include <map>
#include <memory>
#include <vector>

#include "reil/reil.h"

#include "control_flow_graph/control_flow_graph.h"
#include "memory_image/memory_image.h"

namespace reil {
class InstructionGraph : public ControlFlowGraph {
 protected:
  std::shared_ptr<MemoryImage> memory_image_;

  virtual reil::NativeInstruction NativeInstructionImpl(uint64_t address,
                                                        const uint8_t* bytes,
                                                        size_t bytes_len) = 0;

  uint64_t NextNativeInstruction(uint64_t address);
  std::shared_ptr<reil::NativeInstruction> NativeInstruction(uint64_t address);

  InstructionGraph(std::shared_ptr<MemoryImage> memory_image,
                   size_t cache_size);

 private:
  std::map<uint64_t, std::weak_ptr<reil::NativeInstruction>> cache_;
  std::vector<std::shared_ptr<reil::NativeInstruction>> ring_cache_;
  size_t cache_index_ = 0;

 public:
  virtual ~InstructionGraph();

  void Add(const ControlFlowGraph& other) override;

  Node NextInstruction(Node node);
  reil::Instruction Instruction(Node node);

  static std::unique_ptr<InstructionGraph> Create(
      std::shared_ptr<MemoryImage> memory_image, size_t cache_size = 0x100);

  static std::unique_ptr<InstructionGraph> Load(
      std::shared_ptr<MemoryImage> memory_image, std::string path, size_t cache_size = 0x100);
};
}  // namespace reil

#define REIL_CONTROL_FLOW_GRAPH_INSTRUCTION_GRAPH_H_
#endif  // REIL_CONTROL_FLOW_GRAPH_INSTRUCTION_GRAPH_H_
