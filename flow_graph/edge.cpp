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

#include "flow_graph/edge.h"
#include "flow_graph/node.h"

namespace reil {
Edge::Edge() : source(), target(), kind() {}

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
  stream << edge.source << " -> " << edge.target;
  if (edge.kind == EdgeKind::kInvalid) {
    stream << " [invalid]";
  } else if (edge.kind == EdgeKind::kFlow) {
    stream << " [flow]";
  } else if (edge.kind == EdgeKind::kJump) {
    stream << " [jump]";
  } else if (edge.kind == EdgeKind::kNativeFlow) {
    stream << " [native_flow]";
  } else if (edge.kind == EdgeKind::kNativeJump) {
    stream << " [native_jump]";
  } else if (edge.kind == EdgeKind::kNativeCall) {
    stream << " [native_call]";
  } else if (edge.kind == EdgeKind::kNativeReturn) {
    stream << " [native_return]";
  } else if (edge.kind == EdgeKind::kNativeBreak) {
    stream << " [native_break]";
  }
  return stream;
}
}  // namespace reil
