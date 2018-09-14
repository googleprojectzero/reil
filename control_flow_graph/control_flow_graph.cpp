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
}  // namespace reil
