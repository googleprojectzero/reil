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
#include "glog/logging.h"

#include "flow_graph/flow_graph.h"
#include "flow_graph/instruction_provider.h"
#include "memory_image/memory_image.h"
#include "reil/reil.h"

namespace reil {
namespace analysis {
template <class T>
bool SolveFunction(FlowGraph& rfg, InstructionProvider& ip,
                   std::map<Edge, T>& edge_states,
                   std::function<T(absl::Span<const T*>)> MergeStates,
                   size_t step_limit = 0x1000) {
  std::set<Node> queue;
  for (auto& in_edge_iter : rfg.incoming_edges()) {
    for (auto& in_edge : in_edge_iter.second) {
      auto edge_state_iter = edge_states.find(in_edge);
      if (edge_state_iter != edge_states.end()) {
        queue.insert(in_edge_iter.first);
      }
    }
  }

  size_t step_count = 0;
  std::shared_ptr<NativeInstruction> ni;
  while (step_count++ < step_limit && !queue.empty()) {
    Node node = *queue.begin();
    queue.erase(queue.begin());

    if (node == 0) continue;

    std::vector<const T*> in_states;
    for (auto& in_edge : rfg.incoming_edges(node)) {
      auto in_edge_state = edge_states.find(in_edge);
      if (in_edge_state == edge_states.end()) {
        continue;
      }
      in_states.push_back(&in_edge_state->second);
    }
    auto in_state = MergeStates(absl::Span<const T*>(in_states));

    auto out_edge_iter = rfg.outgoing_edges().lower_bound(node);
    if (out_edge_iter == rfg.outgoing_edges().end() ||
        out_edge_iter->first.address != node.address) {
      // flowgraph consists of reil basic blocks, so if we haven't found a
      // return edge within this native instruction, we have a broken flowgraph.
      LOG(WARNING) << "invalid_basic_block: " << node << " "
                   << out_edge_iter->first;
      continue;
    }

    if (!ni || ni->address != node.address) {
      ni = ip.NativeInstruction(node.address);
    }

    DCHECK(node.offset < ni->reil.size());
    Instruction ri = ni->reil[node.offset];
    VLOG(2) << in_state;
    VLOG(3) << node << " " << ri;
    while (node != out_edge_iter->first) {
      Node next_node(node.address, node.offset + 1);
      // NB: we don't run across native instruction boundaries here, so we don't
      // need to check for that or request a new native instruction.

      in_state.Transform(Edge(node, next_node, EdgeKind::kFlow), ri);
      if (!in_state.Valid()) {
        break;
      }

      node = next_node;
      DCHECK(node.offset < ni->reil.size());
      ri = ni->reil[node.offset];
      VLOG(4) << in_state;
      VLOG(3) << node << " " << ri;
    }

    if (!in_state.Valid()) {
      VLOG(1) << "invalid_in_state";
      continue;
    }

    VLOG(1) << *ni;
    for (auto& out_edge : out_edge_iter->second) {
      auto out_state = in_state;
      out_state.Transform(out_edge, ri);
      if (!out_state.Valid()) {
        VLOG(1) << "invalid_out_state";
        continue;
      }

      auto out_edge_state = edge_states.find(out_edge);
      if (out_edge_state == edge_states.end()) {
        VLOG(1) << "new_edge_state: " << out_edge << " " << out_state;
        edge_states.emplace(std::make_pair(out_edge, std::move(out_state)));
        if (out_edge.kind < EdgeKind::kNativeCall) {
          queue.insert(out_edge.target);
        }
      } else if (out_edge_state->second != out_state) {
        VLOG(1) << "updated_edge_state: " << out_edge << " " << out_state;
        out_edge_state->second = std::move(out_state);
        if (out_edge.kind < EdgeKind::kNativeCall) {
          queue.insert(out_edge.target);
        }
      } else {
        VLOG(1) << "equal_edge_state";
      }
    }
  }

  VLOG(1) << "solver finished after " << step_count << " / " << step_limit;

  return step_count < step_limit;
}

template <class T>
bool SolveFunction(FlowGraph& rfg, InstructionProvider& ip,
                   std::map<Edge, T>& edge_states, size_t step_limit = 0x1000) {
  return SolveFunction<T>(rfg, ip, edge_states, T::Merge, step_limit,
                          basic_block_limit);
}

template <class T>
bool SolveFunction(FlowGraph& rfg, const MemoryImage& memory_image,
                   std::map<Edge, T>& edge_states, size_t step_limit = 0x1000) {
  std::unique_ptr<InstructionProvider> ip =
      InstructionProvider::Create(memory_image);
  return SolveFunction<T>(rfg, *ip, edge_states, T::Merge, step_limit,
                          basic_block_limit);
}
}  // namespace analysis
}  // namespace reil

#define REIL_ANALYSIS_SOLVER_H_
#endif  // REIL_ANALYSIS_SOLVER_H_
