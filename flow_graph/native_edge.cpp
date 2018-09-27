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

#include <iostream>

#include "flow_graph/native_edge.h"

namespace reil {
NativeEdge::NativeEdge() : source(), target(), kind() {}

NativeEdge::NativeEdge(uint64_t source, uint64_t target, NativeEdgeKind kind)
    : source(source), target(target), kind(kind) {}

bool operator==(const NativeEdge& lhs, const NativeEdge& rhs) {
  return lhs.source == rhs.source && lhs.target == rhs.target &&
         lhs.kind == rhs.kind;
}

bool operator!=(const NativeEdge& lhs, const NativeEdge& rhs) {
  return !(lhs == rhs);
}

bool operator<(const NativeEdge& lhs, const NativeEdge& rhs) {
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

bool operator>(const NativeEdge& lhs, const NativeEdge& rhs) {
  return rhs < lhs;
}
bool operator<=(const NativeEdge& lhs, const NativeEdge& rhs) {
  return !(lhs > rhs);
}
bool operator>=(const NativeEdge& lhs, const NativeEdge& rhs) {
  return !(lhs < rhs);
}

std::ostream& operator<<(std::ostream& stream, const NativeEdge& edge) {
  stream << std::hex << edge.source << " -> " << edge.target;
  if (edge.kind == NativeEdgeKind::kInvalid) {
    stream << " [invalid]";
  } else if (edge.kind == NativeEdgeKind::kFlow) {
    stream << " [flow]";
  } else if (edge.kind == NativeEdgeKind::kJump) {
    stream << " [jump]";
  } else if (edge.kind == NativeEdgeKind::kCall) {
    stream << " [call]";
  } else if (edge.kind == NativeEdgeKind::kReturn) {
    stream << " [return]";
  } else if (edge.kind == NativeEdgeKind::kBreak) {
    stream << " [break]";
  }
  return stream;
}
}  // namespace reil
