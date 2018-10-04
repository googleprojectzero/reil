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

#include "flow_graph/flow_graph.h"

#include "absl/memory/memory.h"
#include "glog/logging.h"

#include "reil/aarch64.h"

namespace reil {
static std::set<Edge> no_edges;

void FlowGraph::AddEdge(const Edge& edge) {
  outgoing_edges_[edge.source].insert(edge);
  incoming_edges_[edge.target].insert(edge);
}

void FlowGraph::AddEdge(const Node& source, const Node& target, EdgeKind kind) {
  AddEdge(Edge(source, target, kind));
}

void FlowGraph::RemoveEdge(const Edge& edge) {
  auto out_edge_iter = outgoing_edges_.find(edge.source);
  if (out_edge_iter != outgoing_edges_.end()) {
    out_edge_iter->second.erase(edge);
    if (out_edge_iter->second.empty()) {
      outgoing_edges_.erase(out_edge_iter);
    }
  }

  auto in_edge_iter = incoming_edges_.find(edge.target);
  if (in_edge_iter != incoming_edges_.end()) {
    in_edge_iter->second.erase(edge);
    if (in_edge_iter->second.empty()) {
      incoming_edges_.erase(in_edge_iter);
    }
  }
}

void FlowGraph::RemoveEdge(const Node& source, const Node& target,
                           EdgeKind kind) {
  RemoveEdge(Edge(source, target, kind));
}

bool FlowGraph::resolved() const {
  auto zero_edge_iter = incoming_edges_.find(0);
  if (zero_edge_iter == incoming_edges_.end()) {
    return true;
  }

  for (auto& zero_edge : zero_edge_iter->second) {
    if (zero_edge.kind != EdgeKind::kNativeReturn &&
        zero_edge.kind != EdgeKind::kNativeBreak) {
      return false;
    }
  }

  return true;
}

const std::set<Edge>& FlowGraph::outgoing_edges(const Node& node) const {
  auto outgoing_edges_iter = outgoing_edges_.find(node);
  if (outgoing_edges_iter != outgoing_edges_.end()) {
    return outgoing_edges_iter->second;
  }
  return no_edges;
}

const std::set<Edge>& FlowGraph::incoming_edges(const Node& node) const {
  auto incoming_edges_iter = incoming_edges_.find(node);
  if (incoming_edges_iter != incoming_edges_.end()) {
    return incoming_edges_iter->second;
  }
  return no_edges;
}

std::unique_ptr<FlowGraph> FlowGraph::Create(const MemoryImage& memory_image,
                                             InstructionProvider& ip,
                                             const NativeFlowGraph& nfg,
                                             uint64_t basic_block_limit) {
  std::unique_ptr<FlowGraph> rfg = absl::make_unique<FlowGraph>();
  std::shared_ptr<NativeInstruction> ni = nullptr;

  for (auto& in_iter : nfg.incoming_edges()) {
    if (in_iter.first == 0) {
      continue;
    }

    // filter out calls to other functions.

    // NB: this shouldn't affect recursion, since we should have other edges
    // that reach any basic blocks that we can also tail-call.

    bool call_target = true;
    for (auto& edge : in_iter.second) {
      if (edge.kind == NativeEdgeKind::kCall) {
        if (edge.source == 0) {
          rfg->AddEdge(edge.source, edge.target, EdgeKind::kNativeCall);
          call_target = false;
        }
      } else {
        call_target = false;
      }
    }

    if (call_target) {
      continue;
    }

    Node bb_start = nfg.BasicBlockStart(in_iter.first);
    Node bb_end = ip.NextNativeInstruction(nfg.BasicBlockEnd(in_iter.first));

    if (bb_end.address - bb_start.address > basic_block_limit) {
      LOG(WARNING) << "basic_block_too_large: " << bb_start << " " << bb_end;
      continue;
    }

    VLOG(1) << "basic_block " << bb_start << " - " << bb_end;

    Node next_node = 0;
    for (Node node = bb_start; node < bb_end; node = next_node) {
      if (!ni || ni->address != node.address) {
        ni = ip.NativeInstruction(node.address);
        VLOG(3) << *ni;
      }

      if ((uint64_t)node.offset + 1 < ni->reil.size()) {
        next_node.address = node.address;
        next_node.offset = node.offset + 1;
      } else {
        next_node = ip.NextNativeInstruction(node.address);
      }

      auto ri = ni->reil[node.offset];
      VLOG(4) << "  " << ri;
      if (ri.opcode == reil::Opcode::Jcc) {
        std::set<NativeEdge> edges;
        bool flow = true;

        // NB: BinExport2 NativeFlowGraphs may have missing edges on call
        // instructions where target could not be resolved.
        if (nfg.outgoing_edges().count(node.address)) {
          edges = nfg.outgoing_edges().at(node.address);
        }

        if (ri.output.index() == kOffset) {
          if (ri.input0.index() == kImmediate &&
              (bool)absl::get<Immediate>(ri.input0)) {
            flow = false;
          }

          Offset offset = absl::get<Offset>(ri.output);
          rfg->AddEdge(
              Edge(node, Node(node.address, offset.offset), EdgeKind::kJump));
          DCHECK(edges.size() == 1);
        } else {
          // all non-local jcc instructions should have a hint.
          DCHECK(ri.input1.index() == kImmediate);
          Immediate hint = absl::get<Immediate>(ri.input1);
          if (hint == kJump) {
            if (ri.input0.index() == kImmediate &&
                (bool)absl::get<Immediate>(ri.input0)) {
              flow = false;
            }

            for (auto edge : edges) {
              if (edge.kind == NativeEdgeKind::kJump) {
                rfg->AddEdge(node, edge.target, EdgeKind::kNativeJump);
              } else {
                DCHECK(edge.kind == NativeEdgeKind::kFlow);
              }
            }
          } else if (hint == kCall) {
            for (auto edge : edges) {
              if (edge.kind == NativeEdgeKind::kCall) {
                rfg->AddEdge(node, edge.target, EdgeKind::kNativeCall);
              } else {
                DCHECK(edge.kind == NativeEdgeKind::kFlow);
              }
            }
          } else if (hint == kReturn) {
            flow = false;
            for (auto edge : edges) {
              if (edge.kind == NativeEdgeKind::kReturn) {
                rfg->AddEdge(node, edge.target, EdgeKind::kNativeReturn);
              } else {
                DCHECK(false);
              }
            }
          }
        }

        if (flow) {
          if (node.address != next_node.address) {
            rfg->AddEdge(node, next_node, EdgeKind::kNativeFlow);
          } else {
            rfg->AddEdge(node, next_node, EdgeKind::kFlow);
          }
        }
      } else {
        if (node.address != next_node.address) {
          rfg->AddEdge(node, next_node, EdgeKind::kNativeFlow);
        }
      }
    }
  }

  return rfg;
}

std::unique_ptr<FlowGraph> FlowGraph::Create(const MemoryImage& memory_image,
                                             const NativeFlowGraph& nfg,
                                             uint64_t basic_block_limit) {
  auto ip = InstructionProvider::Create(memory_image);
  return Create(memory_image, *ip, nfg, basic_block_limit);
}

std::unique_ptr<FlowGraph> FlowGraph::Load(const MemoryImage& memory_image,
                                           InstructionProvider& ip,
                                           std::string path,
                                           uint64_t basic_block_limit) {
  auto nfg = NativeFlowGraph::Load(path);
  return Create(memory_image, ip, *nfg, basic_block_limit);
}

std::unique_ptr<FlowGraph> FlowGraph::Load(const MemoryImage& memory_image,
                                           std::string path,
                                           uint64_t basic_block_limit) {
  auto ip = InstructionProvider::Create(memory_image);
  return Load(memory_image, *ip, path, basic_block_limit);
}
}  // namespace reil
