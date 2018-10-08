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

#include "analysis/constants_analysis.h"
#include "disassembler/disassembler.h"
#include "flow_graph/flow_graph.h"

namespace reil {
namespace disassembler {
bool ResolveBranches(const MemoryImage& memory_image, InstructionProvider& ip,
                     NativeFlowGraph& nfg) {
  bool resolved = false;
  bool jump_added = false;

  do {
    reil::analysis::Session session(memory_image);
    session.AddFlowGraph(FlowGraph::Create(memory_image, ip, nfg));

    auto flow_graph = session.flow_graph(nfg.Entry());

    auto constants = reil::analysis::LocalConstantsAnalysis(
        session, flow_graph->Entry().address);

    if (constants) {
      resolved = true;
      jump_added = false;

      for (auto edge : flow_graph->incoming_edges(0)) {
        auto state = constants->On(edge);
        if (!state) {
          VLOG(1) << "no state :-(";
          continue;
        }

        auto ri = ip.Instruction(edge.source);
        auto value = state->GetOperand(ri.output);
        if (value) {
          uint64_t address = static_cast<uint64_t>(*value);
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
