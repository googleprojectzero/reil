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

#include "flow_graph/native_flow_graph.h"

#include <algorithm>
#include <fstream>

#include "absl/memory/memory.h"
#include "glog/logging.h"

#include "external/com_google_binexport/binexport2.pb.h"
#include "flow_graph/native_flow_graph.pb.h"

namespace reil {
std::set<NativeEdge> no_edges;

void NativeFlowGraph::AddEdge(const NativeEdge& edge) {
  VLOG(1) << "add " << edge;
  outgoing_edges_[edge.source].insert(edge);
  incoming_edges_[edge.target].insert(edge);
}

void NativeFlowGraph::AddEdge(uint64_t source, uint64_t target,
                              NativeEdgeKind kind) {
  AddEdge(NativeEdge(source, target, kind));
}

void NativeFlowGraph::RemoveEdge(const NativeEdge& edge) {
  VLOG(1) << "remove " << edge;
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

void NativeFlowGraph::RemoveEdge(uint64_t source, uint64_t target,
                                 NativeEdgeKind kind) {
  RemoveEdge(NativeEdge(source, target, kind));
}

bool NativeFlowGraph::resolved() const {
  auto zero_edge_iter = incoming_edges_.find(0);
  if (zero_edge_iter == incoming_edges_.end()
      || zero_edge_iter->second.empty()) {
    return true;
  }

  for (auto& zero_edge : zero_edge_iter->second) {
    if (zero_edge.kind != NativeEdgeKind::kReturn &&
        zero_edge.kind != NativeEdgeKind::kBreak) {
      return false;
    }
  }

  return true;
}

const std::set<NativeEdge>& NativeFlowGraph::outgoing_edges(
    uint64_t node) const {
  auto outgoing_edges_iter = outgoing_edges_.find(node);
  if (outgoing_edges_iter != outgoing_edges_.end()) {
    return outgoing_edges_iter->second;
  }
  return no_edges;
}

const std::set<NativeEdge>& NativeFlowGraph::incoming_edges(
    uint64_t node) const {
  auto incoming_edges_iter = incoming_edges_.find(node);
  if (incoming_edges_iter != incoming_edges_.end()) {
    return incoming_edges_iter->second;
  }
  return no_edges;
}

uint64_t NativeFlowGraph::BasicBlockStart(uint64_t node) const {
  uint64_t bb_start = 0;
  uint64_t next_bb_start = 0xffffffffffffffffull;

  for (auto in_edge_riter = incoming_edges_.rbegin();
       in_edge_riter != incoming_edges_.rend();
       ++in_edge_riter) {
    // check that this incoming edge is at or before node
    if (in_edge_riter->first <= node) {
      auto out_edge_iter = outgoing_edges_.lower_bound(in_edge_riter->first);
      // if there is no outgoing edge after the incoming edge, or the outgoing
      // edge is after the start of the next basic block then this is an 
      // incomplete basic block
      if (out_edge_iter == outgoing_edges_.end()
          || next_bb_start <= out_edge_iter->first) {
        break;
      }

      // if the next outgoing edge is before node, then there is no basic block
      // containing node
      if (out_edge_iter->first < node) {
        break;
      }

      bb_start = in_edge_riter->first;
      break;
    }

    next_bb_start = in_edge_riter->first;
  }

  return bb_start;
}

uint64_t NativeFlowGraph::BasicBlockEnd(uint64_t node) const {
  uint64_t bb_end = 0;
  uint64_t next_bb_start = 0xffffffffffffffffull;

  for (auto in_edge_riter = incoming_edges_.rbegin();
       in_edge_riter != incoming_edges_.rend();
       ++in_edge_riter) {
    // check that this incoming edge is at or before node
    if (in_edge_riter->first <= node) {
      auto out_edge_iter = outgoing_edges_.lower_bound(in_edge_riter->first);
      // if there is no outgoing edge after the incoming edge, or the outgoing
      // edge is after the start of the next basic block then this is an 
      // incomplete basic block
      if (out_edge_iter == outgoing_edges_.end()
          || next_bb_start <= out_edge_iter->first) {
        break;
      }

      // if the next outgoing edge is before node, then there is no basic block
      // containing node
      if (out_edge_iter->first < node) {
        break;
      }

      bb_end = out_edge_iter->first;
      break;
    }

    next_bb_start = in_edge_riter->first;
  }

  return bb_end;
}

void NativeFlowGraph::Save(std::string path) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  proto::NativeFlowGraph proto_control_flow_graph;

  for (auto out_entry : outgoing_edges_) {
    for (auto edge : out_entry.second) {
      proto::NativeEdge* proto_edge = proto_control_flow_graph.add_edges();
      proto_edge->set_source(edge.source);
      proto_edge->set_target(edge.target);

      // INVALID is the default
      if (edge.kind == NativeEdgeKind::kFlow) {
        proto_edge->set_kind(proto::NativeEdgeKind::FLOW);
      } else if (edge.kind == NativeEdgeKind::kJump) {
        proto_edge->set_kind(proto::NativeEdgeKind::JUMP);
      } else if (edge.kind == NativeEdgeKind::kCall) {
        proto_edge->set_kind(proto::NativeEdgeKind::CALL);
      } else if (edge.kind == NativeEdgeKind::kReturn) {
        proto_edge->set_kind(proto::NativeEdgeKind::RETURN);
      } else if (edge.kind == NativeEdgeKind::kBreak) {
        proto_edge->set_kind(proto::NativeEdgeKind::BREAK);
      }
    }
  }

  std::fstream output(path, std::ios::out | std::ios::binary);
  if (output) {
    proto_control_flow_graph.SerializeToOstream(&output);
  }
}

std::unique_ptr<NativeFlowGraph> NativeFlowGraph::Load(std::string path) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::unique_ptr<NativeFlowGraph> control_flow_graph = nullptr;
  proto::NativeFlowGraph proto_control_flow_graph;

  std::fstream input(path, std::ios::in | std::ios::binary);
  if (input && proto_control_flow_graph.ParseFromIstream(&input)) {
    control_flow_graph = absl::make_unique<NativeFlowGraph>();

    for (int i = 0; i < proto_control_flow_graph.edges_size(); ++i) {
      const proto::NativeEdge& proto_edge = proto_control_flow_graph.edges(i);

      NativeEdge edge(proto_edge.source(), proto_edge.target(),
                      static_cast<NativeEdgeKind>(proto_edge.kind()));

      control_flow_graph->AddEdge(edge);
    }
  }
  return control_flow_graph;
}

std::map<uint64_t, std::unique_ptr<NativeFlowGraph>>
NativeFlowGraph::LoadBinexport2(const BinExport2& bx2) {
  std::map<uint64_t, std::unique_ptr<NativeFlowGraph>> result;
  std::map<int, uint64_t> insn_address;
  std::map<int, uint8_t> insn_size;
  uint64_t next_address = 0;

  for (int i = 0; i < bx2.instruction_size(); ++i) {
    uint64_t address = bx2.instruction(i).address();
    uint8_t size = bx2.instruction(i).raw_bytes().size();
    if (!address) {
      address = next_address;
    }
    next_address = address + size;
    insn_address[i] = address;
    insn_size[i] = size;
  }

  std::map<int, uint64_t> bb_start;
  std::map<int, uint64_t> bb_end;

  for (int i = 0; i < bx2.basic_block_size(); ++i) {
    int begin_index = bx2.basic_block(i).instruction_index(0).begin_index();
    int end_index = bx2.basic_block(i).instruction_index(0).end_index();
    bb_start[i] = insn_address[begin_index];
    bb_end[i] = insn_address[end_index];
  }

  // TODO: Add call edges to the resulting cfgs (from binexport callgraph)

  for (int i = 0; i < bx2.flow_graph_size(); ++i) {
    const BinExport2::FlowGraph& bx2_fg = bx2.flow_graph(i);
    std::unique_ptr<NativeFlowGraph> nfg =
        absl::make_unique<NativeFlowGraph>();

    nfg->AddEdge(0, bb_start[bx2_fg.entry_basic_block_index()],
                 NativeEdgeKind::kCall);
    for (int j = 0; j < bx2_fg.edge_size(); ++j) {
      int src_index = bx2_fg.edge(j).source_basic_block_index();
      int tgt_index = bx2_fg.edge(j).target_basic_block_index();
      nfg->AddEdge(bb_end[src_index], bb_start[tgt_index],
                   NativeEdgeKind::kJump);
    }

    // TODO: this is probably not ideal, but can't think of a better solution
    // for right now. we expect to have a return edge to 0 at all function
    // return edges, but binexport doesn't contain return edges at all.
    for (int j = 0; j < bx2_fg.basic_block_index_size(); ++j) {
      int index = bx2_fg.basic_block_index(j);
      if (nfg->outgoing_edges(bb_end[index]).empty()) {
        nfg->AddEdge(bb_end[index], 0, NativeEdgeKind::kReturn);
      }
    }

    result[bb_start[bx2_fg.entry_basic_block_index()]] = std::move(nfg);
  }

  return result;
}

std::map<uint64_t, std::unique_ptr<NativeFlowGraph>>
NativeFlowGraph::LoadBinexport2(std::string path) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::map<uint64_t, std::unique_ptr<NativeFlowGraph>> result;
  BinExport2 bx2;

  std::fstream input(path, std::ios::in | std::ios::binary);
  if (input && bx2.ParseFromIstream(&input)) {
    result = LoadBinexport2(bx2);
  }

  return result;
}
}  // namespace reil
