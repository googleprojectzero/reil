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

#ifndef REIL_FLOW_GRAPH_FLOW_GRAPH_H_

#include <functional>
#include <map>
#include <memory>
#include <set>

#include "flow_graph/edge.h"
#include "flow_graph/instruction_provider.h"
#include "flow_graph/native_flow_graph.h"
#include "flow_graph/node.h"
#include "memory_image/memory_image.h"
#include "reil/reil.h"

namespace reil {
class FlowGraph {
  std::map<Node, std::set<Edge>> outgoing_edges_;
  std::map<Node, std::set<Edge>> incoming_edges_;

 public:
  void AddEdge(const Edge& edge);
  void AddEdge(const Node& source, const Node& target, EdgeKind kind);

  void RemoveEdge(const Edge& edge);
  void RemoveEdge(const Node& source, const Node& target, EdgeKind kind);

  bool resolved() const;

  const std::map<Node, std::set<Edge>>& outgoing_edges() const {
    return outgoing_edges_;
  }

  const std::map<Node, std::set<Edge>>& incoming_edges() const {
    return incoming_edges_;
  }

  const std::set<Edge>& outgoing_edges(const Node& node) const;
  const std::set<Edge>& incoming_edges(const Node& node) const;

  Node Entry() const;

  Node BasicBlockStart(const Node& node) const;
  Node BasicBlockEnd(const Node& node) const;

  static std::unique_ptr<FlowGraph> Create(const MemoryImage& memory_image,
                                           InstructionProvider& ip,
                                           const NativeFlowGraph& nfg,
                                           uint64_t basic_block_limit = 0x1000);

  static std::unique_ptr<FlowGraph> Create(const MemoryImage& memory_image,
                                           const NativeFlowGraph& nfg,
                                           uint64_t basic_block_limit = 0x1000);

  static std::unique_ptr<FlowGraph> Load(const MemoryImage& memory_image,
                                         InstructionProvider& ip,
                                         std::string path,
                                         uint64_t basic_block_limit = 0x1000);

  static std::unique_ptr<FlowGraph> Load(const MemoryImage& memory_image,
                                         std::string path,
                                         uint64_t basic_block_limit = 0x1000);
};
}  // namespace reil

#define REIL_FLOW_GRAPH_FLOW_GRAPH_H_
#endif  // REIL_FLOW_GRAPH_FLOW_GRAPH_H_
