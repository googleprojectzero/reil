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

#ifndef REIL_ANALYSIS_SOLVER_H_

#include <map>
#include <memory>
#include <set>

#include "absl/types/span.h"
#include "analysis/instruction_graph.h"
#include "control_flow_graph/control_flow_graph.h"
#include "glog/logging.h"
#include "memory_image/memory_image.h"
#include "reil/reil.h"

namespace reil {
namespace analysis {
template <class T>
bool Solve(
    std::shared_ptr<InstructionGraph> instruction_graph,
    std::map<Edge, std::shared_ptr<T>>& edge_states,
    std::function<std::shared_ptr<T>(absl::Span<const std::shared_ptr<T>>)>
        merge,
    std::function<std::shared_ptr<T>(const Edge&, const Instruction&,
                                     std::shared_ptr<T>)>
        transform,
    std::function<bool(const T&, const T&)> compare,
    std::function<std::string(const T&)> print, size_t step_limit = 0x1000) {
  std::set<Node> node_queue;
  for (auto& edge_state : edge_states) {
    node_queue.insert(edge_state.first.target);
  }

  size_t step_count = 0;
  while (step_count++ < step_limit && !node_queue.empty()) {
    Node node = *node_queue.begin();
    node_queue.erase(node_queue.begin());

    if (node == 0) continue;

    std::shared_ptr<T> in_state;
    std::vector<std::shared_ptr<T>> in_states;
    for (auto& in_edge : instruction_graph->incoming_edges(node)) {
      auto in_edge_state = edge_states.find(in_edge);
      if (in_edge_state == edge_states.end()) continue;
      in_states.push_back(in_edge_state->second);
    }
    in_state = merge(in_states);

    Instruction ri = instruction_graph->Instruction(node);
    for (auto& out_edge : instruction_graph->outgoing_edges(node)) {
      std::shared_ptr<T> out_state = transform(out_edge, ri, in_state);

      auto out_edge_state = edge_states.find(out_edge);
      if (out_edge_state == edge_states.end()) {
        VLOG(1) << "new_edge_state: " << out_edge << " " << print(*out_state);
        edge_states[out_edge] = out_state;
        node_queue.insert(out_edge.target);
      } else if (out_edge_state->second == out_state) {
        VLOG(2) << "no_change";
      } else if (compare(*out_edge_state->second, *out_state)) {
        if (compare(*out_state, *out_edge_state->second)) {
          VLOG(2) << "no_change";
          continue;
        }
        VLOG(1) << "updated_edge_state: " << out_edge << " "
                << print(*out_state);
        edge_states[out_edge] = out_state;
        node_queue.insert(out_edge.target);
      } else {
        CHECK(!compare(*out_state, *out_edge_state->second))
            << "non monotonic state transition! " << print(*out_state) << " -> "
            << print(*out_edge_state->second);
      }
    }
  }

  return step_count < step_limit;
}
}  // namespace analysis
}  // namespace reil

#define REIL_ANALYSIS_SOLVER_H_
#endif  // REIL_ANALYSIS_SOLVER_H_