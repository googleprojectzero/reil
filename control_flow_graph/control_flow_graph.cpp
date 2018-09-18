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

#include "control_flow_graph/control_flow_graph.h"

#include <fstream>

#include "absl/memory/memory.h"
#include "control_flow_graph/control_flow_graph.pb.h"
#include "external/com_google_binexport/binexport2.pb.h"
#include "glog/logging.h"

namespace reil {
Node::Node() : address(0), offset(0) {}

Node::Node(uint64_t address_) : address(address_), offset(0) {}

Node::Node(uint64_t address_, uint16_t offset_)
    : address(address_), offset(offset_) {
  DCHECK(offset_ != 0xffff);
}

bool operator==(const Node& lhs, const Node& rhs) {
  return lhs.address == rhs.address && lhs.offset == rhs.offset;
}

bool operator!=(const Node& lhs, const Node& rhs) { return !(lhs == rhs); }

bool operator<(const Node& lhs, const Node& rhs) {
  if (lhs.address < rhs.address) {
    return true;
  } else if (lhs.address > rhs.address) {
    return false;
  }

  return lhs.offset < rhs.offset;
}

bool operator>(const Node& lhs, const Node& rhs) { return rhs < lhs; }
bool operator<=(const Node& lhs, const Node& rhs) { return !(lhs > rhs); }
bool operator>=(const Node& lhs, const Node& rhs) { return !(lhs < rhs); }

std::ostream& operator<<(std::ostream& stream, const Node& node) {
  return stream << std::hex << node.address << "." << node.offset;
}

Edge::Edge() : source(), target(), kind(kReilFlow) {}

Edge::Edge(Node source, Node target, EdgeKind kind)
    : source(source), target(target), kind(kind) {
  // TODO: DCHECKS here.
}

bool operator==(const Edge& lhs, const Edge& rhs) {
  return lhs.source == rhs.source && lhs.target == rhs.target &&
         lhs.kind == rhs.kind;
}

bool operator!=(const Edge& lhs, const Edge& rhs) { return !(lhs == rhs); }

bool operator<(const Edge& lhs, const Edge& rhs) {
  if (lhs.source < rhs.source) {
    return true;
  } else if (lhs.source > rhs.source) {
    return false;
  }

  if (lhs.target < rhs.target) {
    return true;
  } else if (lhs.target > rhs.target) {
    return false;
  }

  return lhs.kind < rhs.kind;
}

bool operator>(const Edge& lhs, const Edge& rhs) { return rhs < lhs; }
bool operator<=(const Edge& lhs, const Edge& rhs) { return !(lhs > rhs); }
bool operator>=(const Edge& lhs, const Edge& rhs) { return !(lhs < rhs); }

std::ostream& operator<<(std::ostream& stream, const Edge& edge) {
  return stream << edge.source << " -> " << edge.target << " [" << edge.kind
                << "]";
}

ControlFlowGraph::~ControlFlowGraph() {}

void ControlFlowGraph::AddEdge(const Edge& edge) {
  outgoing_edges_[edge.source].insert(edge);
  if (edge.target != 0) {
    incoming_edges_[edge.target].insert(edge);
  }
}

void ControlFlowGraph::AddEdge(const Node& source, const Node& target,
                               EdgeKind kind) {
  AddEdge(Edge(source, target, kind));
}

void ControlFlowGraph::Add(const ControlFlowGraph& other) {
  outgoing_edges_.insert(other.outgoing_edges_.cbegin(),
                         other.outgoing_edges_.cend());
  incoming_edges_.insert(other.incoming_edges_.cbegin(),
                         other.incoming_edges_.cend());
}

void ControlFlowGraph::Save(std::string path) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  proto::ControlFlowGraph proto_control_flow_graph;

  for (auto out_entry : outgoing_edges_) {
    for (auto edge : out_entry.second) {
      proto::Edge* proto_edge = proto_control_flow_graph.add_edges();
      proto::Node* proto_source = proto_edge->mutable_source();
      proto_source->set_address(edge.source.address);
      proto_source->set_offset(edge.source.offset);

      proto::Node* proto_target = proto_edge->mutable_target();
      proto_target->set_address(edge.target.address);
      proto_target->set_offset(edge.target.offset);

      proto_edge->set_kind(static_cast<reil::proto::EdgeKind>(edge.kind));
    }
  }

  std::fstream output(path, std::ios::out | std::ios::binary);
  if (output) {
    proto_control_flow_graph.SerializeToOstream(&output);
  }
}

std::unique_ptr<ControlFlowGraph> ControlFlowGraph::Load(std::string path) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::unique_ptr<ControlFlowGraph> control_flow_graph = nullptr;
  proto::ControlFlowGraph proto_control_flow_graph;

  std::fstream input(path, std::ios::in | std::ios::binary);
  if (input && proto_control_flow_graph.ParseFromIstream(&input)) {
    control_flow_graph = absl::make_unique<ControlFlowGraph>();

    for (int i = 0; i < proto_control_flow_graph.edges_size(); ++i) {
      const proto::Edge& proto_edge = proto_control_flow_graph.edges(i);

      Edge edge(
          Node(proto_edge.source().address(), proto_edge.source().offset()),
          Node(proto_edge.target().address(), proto_edge.target().offset()),
          static_cast<EdgeKind>(proto_edge.kind()));

      control_flow_graph->AddEdge(edge);
    }
  }
  return control_flow_graph;
}

std::map<uint64_t, std::unique_ptr<ControlFlowGraph>> ControlFlowGraph::LoadBinexport2(std::string path) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::map<uint64_t, std::unique_ptr<ControlFlowGraph>> result;
  BinExport2 bx2;
  
  std::fstream input(path, std::ios::in | std::ios::binary);
  if (input && bx2.ParseFromIstream(&input)) {
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
      const BinExport2::FlowGraph& bx2_fn = bx2.flow_graph(i);
      std::unique_ptr<ControlFlowGraph> cfg = absl::make_unique<ControlFlowGraph>();

      cfg->AddEdge(0, bb_start[bx2_fn.entry_basic_block_index()], kNativeCall);
      for (int j = 0; j < bx2_fn.edge_size(); ++j) {
        int src_index = bx2_fn.edge(j).source_basic_block_index();
        int tgt_index = bx2_fn.edge(j).target_basic_block_index();
        cfg->AddEdge(bb_end[src_index], bb_start[tgt_index], kNativeJump);
      }

      // TODO: this is probably not ideal, but can't think of a better solution
      // for right now. we expect to have a return edge to 0 at all function 
      // return edges, but binexport doesn't contain return edges at all.
      for (int j = 0; j < bx2_fn.basic_block_index_size(); ++j) {
        int index = bx2_fn.basic_block_index(j);
        if (cfg->outgoing_edges(bb_end[index]).empty()) {
          cfg->AddEdge(bb_end[index], 0, kNativeReturn);
        }
      }

      result[bb_start[bx2_fn.entry_basic_block_index()]] = std::move(cfg);
    }
  }

  return result;
}
}  // namespace reil
