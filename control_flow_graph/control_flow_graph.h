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

#ifndef REIL_CONTROL_FLOW_GRAPH_CONTROL_FLOW_GRAPH_H_

#include <map>
#include <memory>
#include <set>

namespace reil {
struct Node {
  uint64_t address;
  uint16_t offset;

  Node();
  Node(uint64_t address_);
  Node(uint64_t address_, uint16_t offset_);
};

bool operator==(const Node& lhs, const Node& rhs);
bool operator!=(const Node& lhs, const Node& rhs);
bool operator<(const Node& lhs, const Node& rhs);
bool operator>(const Node& lhs, const Node& rhs);
bool operator<=(const Node& lhs, const Node& rhs);
bool operator>=(const Node& lhs, const Node& rhs);
std::ostream& operator<<(std::ostream& stream, const Node& node);

enum EdgeKind {
  kReilFlow,
  kReilJump,
  kNativeFlow,
  kNativeJump,
  kNativeCall,
  kNativeReturn,
  kNativeBreak,
  kInvalid,
};

struct Edge {
  Node source;
  Node target;
  EdgeKind kind;

  Edge();
  Edge(Node source, Node target, EdgeKind kind);
};

bool operator==(const Edge& lhs, const Edge& rhs);
bool operator!=(const Edge& lhs, const Edge& rhs);
bool operator<(const Edge& lhs, const Edge& rhs);
bool operator>(const Edge& lhs, const Edge& rhs);
bool operator<=(const Edge& lhs, const Edge& rhs);
bool operator>=(const Edge& lhs, const Edge& rhs);
std::ostream& operator<<(std::ostream& stream, const Edge& edge);

class ControlFlowGraph {
  std::map<Node, std::set<Edge>> outgoing_edges_;
  std::map<Node, std::set<Edge>> incoming_edges_;

 public:
  virtual ~ControlFlowGraph();

  void AddEdge(const Edge& edge);
  void AddEdge(const Node& source, const Node& target, EdgeKind kind);

  virtual void Add(const ControlFlowGraph& other);

  const std::map<Node, std::set<Edge>>& outgoing_edges() const {
    return outgoing_edges_;
  }

  std::set<Edge> outgoing_edges(const Node& node) const {
    std::set<Edge> edges;
    auto outgoing_edges_iter = outgoing_edges_.find(node);
    if (outgoing_edges_iter != outgoing_edges_.end()) {
      edges = outgoing_edges_iter->second;
    }
    return edges;
  }

  const std::map<Node, std::set<Edge>>& incoming_edges() const {
    return incoming_edges_;
  }

  std::set<Edge> incoming_edges(const Node& node) const {
    std::set<Edge> edges;
    auto incoming_edges_iter = incoming_edges_.find(node);
    if (incoming_edges_iter != incoming_edges_.end()) {
      edges = incoming_edges_iter->second;
    }
    return edges;
  }

  void Save(std::string path);
  static std::unique_ptr<ControlFlowGraph> Load(std::string path);
  static std::map<uint64_t, std::unique_ptr<ControlFlowGraph>> LoadBinexport2(std::string path);
};
}  // namespace reil

#define REIL_CONTROL_FLOW_GRAPH_CONTROL_FLOW_GRAPH_H_
#endif  // REIL_CONTROL_FLOW_GRAPH_CONTROL_FLOW_GRAPH_H_
