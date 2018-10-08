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

#ifndef REIL_FLOW_GRAPH_NATIVE_FLOW_GRAPH_H_

#include <map>
#include <memory>
#include <set>

#include "flow_graph/native_edge.h"

#include "external/com_google_binexport/binexport2.pb.h"

namespace reil {
class NativeFlowGraph {
  std::map<uint64_t, std::set<NativeEdge>> outgoing_edges_;
  std::map<uint64_t, std::set<NativeEdge>> incoming_edges_;

 public:
  void AddEdge(const NativeEdge& edge);
  void AddEdge(uint64_t source, uint64_t target, NativeEdgeKind kind);

  void RemoveEdge(const NativeEdge& edge);
  void RemoveEdge(uint64_t source, uint64_t target, NativeEdgeKind kind);

  bool resolved() const;

  const std::map<uint64_t, std::set<NativeEdge>>& outgoing_edges() const {
    return outgoing_edges_;
  }

  const std::map<uint64_t, std::set<NativeEdge>>& incoming_edges() const {
    return incoming_edges_;
  }

  const std::set<NativeEdge>& outgoing_edges(uint64_t node) const;
  const std::set<NativeEdge>& incoming_edges(uint64_t node) const;

  uint64_t Entry() const;
  uint64_t BasicBlockStart(uint64_t node) const;
  uint64_t BasicBlockEnd(uint64_t node) const;

  void Save(std::string path);
  static std::unique_ptr<NativeFlowGraph> Load(std::string path);
  static std::map<uint64_t, std::unique_ptr<NativeFlowGraph>> LoadBinexport2(
      const BinExport2& bx2);
  static std::map<uint64_t, std::unique_ptr<NativeFlowGraph>> LoadBinexport2(
      std::string path);
};
}  // namespace reil

#define REIL_FLOW_GRAPH_NATIVE_FLOW_GRAPH_H_
#endif  // REIL_FLOW_GRAPH_NATIVE_FLOW_GRAPH_H_
