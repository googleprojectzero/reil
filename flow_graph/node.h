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

#ifndef REIL_FLOW_GRAPH_NODE_H_

#include <cstdint>
#include <iostream>

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
}  // namespace reil

#define REIL_FLOW_GRAPH_NODE_H_
#endif  // REIL_FLOW_GRAPH_NODE_H_
