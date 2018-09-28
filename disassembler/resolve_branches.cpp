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

#include <cstdint>
#include <set>

#include "glog/logging.h"

#include "analysis/dataflow.h"
#include "analysis/solver.h"
#include "disassembler/disassembler.h"
#include "flow_graph/flow_graph.h"

namespace reil {
namespace disassembler {
bool ResolveBranches(const MemoryImage& memory_image, InstructionProvider& ip,
                     NativeFlowGraph& nfg) {
  bool resolved = false;
  bool jump_added = false;

  do {
    std::unique_ptr<FlowGraph> rfg = FlowGraph::Create(memory_image, ip, nfg);

    std::map<Edge, reil::analysis::DataflowState<>> edge_states;
    for (auto edge : rfg->outgoing_edges(0)) {
      VLOG(1) << edge;
      edge_states.emplace(
          std::make_pair(edge, reil::analysis::DataflowState<>(memory_image)));
    }

    if (reil::analysis::SolveFunction<reil::analysis::DataflowState<>>(
            *rfg, ip, edge_states)) {
      resolved = true;
      jump_added = false;

      for (auto edge : rfg->incoming_edges(0)) {
        auto state_iter = edge_states.find(edge);
        if (state_iter != edge_states.end()) {
          auto ri = ip.Instruction(edge.source);
          auto value = state_iter->second.GetOperand(ri.output);
          if (absl::holds_alternative<Immediate>(value)) {
            uint64_t address = (uint64_t)absl::get<Immediate>(value);
            VLOG(1) << "resolved " << *ip.NativeInstruction(edge.source.address)
                    << " [" << std::hex << address << "]";
            if (edge.kind == EdgeKind::kNativeJump) {
              nfg.RemoveEdge(edge.source.address, 0, NativeEdgeKind::kJump);
              nfg.AddEdge(edge.source.address, address, NativeEdgeKind::kJump);
              Disassemble(memory_image, nfg, address);
              jump_added = true;
            } else if (edge.kind == EdgeKind::kNativeCall) {
              nfg.RemoveEdge(edge.source.address, 0, NativeEdgeKind::kCall);
              nfg.AddEdge(edge.source.address, address, NativeEdgeKind::kCall);
            } else {
              LOG(WARNING) << "Unexpected edge resolved: " << edge << " "
                           << address;
            }
          } else {
            resolved = false;
          }
        }
      }
    } else {
      break;
    }
  } while (jump_added && !nfg.resolved());

  if (jump_added) {
    return true;
  }

  return resolved;
}

bool ResolveBranches(const MemoryImage& memory_image, NativeFlowGraph& nfg) {
  std::unique_ptr<InstructionProvider> ip =
      InstructionProvider::Create(memory_image);
  return ResolveBranches(memory_image, *ip, nfg);
}
}  // namespace disassembler
}  // namespace reil
