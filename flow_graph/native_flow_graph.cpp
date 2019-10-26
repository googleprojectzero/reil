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

bool NativeFlowGraph::RemoveEdge(const NativeEdge& edge) {
  VLOG(1) << "remove " << edge;
  bool found = false;
  auto out_edge_iter = outgoing_edges_.find(edge.source);
  if (out_edge_iter != outgoing_edges_.end()) {
    found |= out_edge_iter->second.erase(edge);
    if (out_edge_iter->second.empty()) {
      outgoing_edges_.erase(out_edge_iter);
    }
  }

  auto in_edge_iter = incoming_edges_.find(edge.target);
  if (in_edge_iter != incoming_edges_.end()) {
    found |= in_edge_iter->second.erase(edge);
    if (in_edge_iter->second.empty()) {
      incoming_edges_.erase(in_edge_iter);
    }
  }

  return found;
}

bool NativeFlowGraph::RemoveEdge(uint64_t source, uint64_t target,
                                 NativeEdgeKind kind) {
  return RemoveEdge(NativeEdge(source, target, kind));
}

bool NativeFlowGraph::resolved() const {
  auto zero_edge_iter = incoming_edges_.find(0);
  if (zero_edge_iter == incoming_edges_.end() ||
      zero_edge_iter->second.empty()) {
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

uint64_t NativeFlowGraph::Entry() const {
  auto zero_edge_iter = outgoing_edges_.find(0);
  CHECK(zero_edge_iter != outgoing_edges_.end());
  CHECK(zero_edge_iter->second.size() == 1);
  return zero_edge_iter->second.begin()->target;
}

uint64_t NativeFlowGraph::BasicBlockStart(uint64_t node) const {
  uint64_t bb_start = 0;
  uint64_t next_bb_start = 0xffffffffffffffffull;

  for (auto in_edge_riter = incoming_edges_.rbegin();
       in_edge_riter != incoming_edges_.rend(); ++in_edge_riter) {
    // check that this incoming edge is at or before node
    if (in_edge_riter->first <= node) {
      auto out_edge_iter = outgoing_edges_.lower_bound(in_edge_riter->first);
      // if there is no outgoing edge after the incoming edge, or the outgoing
      // edge is after the start of the next basic block then this is an
      // incomplete basic block
      if (out_edge_iter == outgoing_edges_.end() ||
          next_bb_start <= out_edge_iter->first) {
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
       in_edge_riter != incoming_edges_.rend(); ++in_edge_riter) {
    // check that this incoming edge is at or before node
    if (in_edge_riter->first <= node) {
      auto out_edge_iter = outgoing_edges_.lower_bound(in_edge_riter->first);
      // if there is no outgoing edge after the incoming edge, or the outgoing
      // edge is after the start of the next basic block then this is an
      // incomplete basic block
      if (out_edge_iter == outgoing_edges_.end() ||
          next_bb_start <= out_edge_iter->first) {
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
  std::map<uint64_t, std::set<uint64_t>> call_edges;
  uint64_t next_address = 0;

  // TODO: This is sadly architecture specific. Need to check up on the details
  // of what BinExport2 mandates for eg. basic block end addresses, use of
  // different edge types etc.

  for (int i = 0; i < bx2.instruction_size(); ++i) {
    uint64_t address = bx2.instruction(i).address();
    uint8_t size = bx2.instruction(i).raw_bytes().size();
    if (!address) {
      address = next_address;
    }
    next_address = address + size;

    // BinExport2 stores call edges in the Instruction objects instead of the
    // CallGraph or FlowGraph.
    if (bx2.instruction(i).call_target_size()) {
      std::set<uint64_t> insn_calls;
      for (int j = 0; j < bx2.instruction(i).call_target_size(); ++j) {
        insn_calls.insert(bx2.instruction(i).call_target(j));
      }
      call_edges.emplace(address, std::move(insn_calls));
    }

    insn_address[i] = address;
    insn_size[i] = size;
  }

  std::map<int, uint64_t> bb_start;
  std::map<int, uint64_t> bb_end;

  for (int i = 0; i < bx2.basic_block_size(); ++i) {
    int begin_index = bx2.basic_block(i).instruction_index(0).begin_index();
    int end_index = bx2.basic_block(i).instruction_index(0).end_index();
    bb_start[i] = insn_address[begin_index];
    bb_end[i] = insn_address[end_index] - 4;
  }

  for (int i = 0; i < bx2.flow_graph_size(); ++i) {
    const BinExport2::FlowGraph& bx2_fg = bx2.flow_graph(i);

    if (bb_start[bx2_fg.entry_basic_block_index()] != 0x004A9CA8) {
      continue;
    }

    std::unique_ptr<NativeFlowGraph> nfg = absl::make_unique<NativeFlowGraph>();

    // add the entry edge.
    nfg->AddEdge(0, bb_start[bx2_fg.entry_basic_block_index()],
                 NativeEdgeKind::kCall);

    // add the flow edges from the BinExport2 FlowGraph.
    for (int j = 0; j < bx2_fg.edge_size(); ++j) {
      int src_index = bx2_fg.edge(j).source_basic_block_index();
      int tgt_index = bx2_fg.edge(j).target_basic_block_index();
      if (bx2_fg.edge(j).type() ==
          BinExport2_FlowGraph_Edge_Type_CONDITION_FALSE) {
        nfg->AddEdge(bb_end[src_index], bb_start[tgt_index],
                     NativeEdgeKind::kFlow);
      } else {
        nfg->AddEdge(bb_end[src_index], bb_start[tgt_index],
                     NativeEdgeKind::kJump);
      }
    }

    // add the call edges from the BinExport2 Instructions.
    for (int j = 0; j < bx2_fg.basic_block_index_size(); ++j) {
      int index = bx2_fg.basic_block_index(j);
      for (auto call_edge_iter = call_edges.lower_bound(bb_start[index]);
           call_edge_iter != call_edges.end() &&
           call_edge_iter != call_edges.upper_bound(bb_end[index]);
           ++call_edge_iter) {
        nfg->AddEdge(call_edge_iter->first, call_edge_iter->first + 4,
                     NativeEdgeKind::kFlow);
        for (auto call_target : call_edge_iter->second) {
          nfg->AddEdge(call_edge_iter->first, call_target,
                       NativeEdgeKind::kCall);
        }
      }
    }

    // add return edges to any basic blocks that don't have any outgoing edges.

    // TODO: this is not perfect, we don't handle noreturn functions in a way
    // that matches the REIL disassembler.

    // TODO: not sure why we need the -4 here, but it seems maybe the BinExport2
    // basic block ends for returns are not consistent with calls/jumps?

    for (int j = 0; j < bx2_fg.basic_block_index_size(); ++j) {
      int index = bx2_fg.basic_block_index(j);
      if (nfg->outgoing_edges(bb_end[index]).empty()) {
        nfg->AddEdge(bb_end[index] - 4, 0, NativeEdgeKind::kReturn);
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
