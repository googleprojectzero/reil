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

#ifndef REIL_ANALYSIS_SESSION_H_

#include <mutex>
#include <thread>

#include "flow_graph/flow_graph.h"
#include "memory_image/memory_image.h"

namespace reil {
namespace analysis {
class Session {
  const MemoryImage& memory_image_;
  std::map<uint64_t, std::unique_ptr<FlowGraph>> flow_graphs_;

  std::mutex instruction_providers_lock_;
  std::map<std::thread::id, std::unique_ptr<InstructionProvider>>
      instruction_providers_;

 public:
  Session(const MemoryImage& memory_image)
      : memory_image_(memory_image), flow_graphs_() {}

  inline const MemoryImage& memory_image() const { return memory_image_; }

  inline const FlowGraph* flow_graph(uint64_t address) const {
    return flow_graphs_.at(address).get();
  }

  inline InstructionProvider* instruction_provider() {
    auto iter = instruction_providers_.find(std::this_thread::get_id());
    if (iter == instruction_providers_.end()) {
      std::lock_guard<std::mutex> guard_ip_lock(instruction_providers_lock_);
      std::tie(iter, std::ignore) = instruction_providers_.emplace(
          std::this_thread::get_id(),
          InstructionProvider::Create(memory_image_));
    }
    return iter->second.get();
  }

  inline void AddFlowGraph(std::unique_ptr<FlowGraph> flow_graph) {
    flow_graphs_[flow_graph->Entry().address] = std::move(flow_graph);
  }
};
}  // namespace analysis
}  // namespace reil

#define REIL_ANALYSIS_SESSION_H_
#endif  // REIL_ANALYSIS_SESSION_H_
