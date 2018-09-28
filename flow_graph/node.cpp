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

#include "flow_graph/node.h"

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
}  // namespace reil
