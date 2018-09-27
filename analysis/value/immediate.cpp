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

#include "analysis/value/immediate.h"

#include <sstream>

namespace reil {
namespace analysis {
absl::variant<Top, Immediate> MergeImmediates(const Immediate& lhs,
                                              const Immediate& rhs) {
  if (lhs == rhs) {
    return lhs;
  } else {
    return Top();
  }
}

Immediate AddImmediates(const Immediate& lhs, const Immediate& rhs) {
  return lhs + rhs;
}

Immediate SubtractImmediates(const Immediate& lhs, const Immediate& rhs) {
  return lhs - rhs;
}

Immediate MultiplyImmediates(const Immediate& lhs, const Immediate& rhs) {
  return lhs * rhs;
}

Immediate DivideImmediates(const Immediate& lhs, const Immediate& rhs) {
  return lhs / rhs;
}

Immediate ModuloImmediates(const Immediate& lhs, const Immediate& rhs) {
  return lhs % rhs;
}

Immediate AndImmediates(const Immediate& lhs, const Immediate& rhs) {
  return lhs & rhs;
}

Immediate OrImmediates(const Immediate& lhs, const Immediate& rhs) {
  return lhs | rhs;
}

Immediate XorImmediates(const Immediate& lhs, const Immediate& rhs) {
  return lhs ^ rhs;
}

Immediate EqualImmediates(const Immediate& lhs, const Immediate& rhs) {
  if (lhs == rhs) {
    return Immediate(8, 1);
  } else {
    return Immediate(8, 0);
  }
}

Immediate NotEqualImmediates(const Immediate& lhs, const Immediate& rhs) {
  if (lhs != rhs) {
    return Immediate(8, 0);
  } else {
    return Immediate(8, 1);
  }
}

Immediate LeftShiftImmediates(const Immediate& lhs, const Immediate& rhs) {
  return lhs << rhs;
}

Immediate RightShiftImmediates(const Immediate& lhs, const Immediate& rhs) {
  return lhs >> rhs;
}

Immediate SignedRightShiftImmediates(const Immediate& lhs,
                                     const Immediate& rhs) {
  Immediate result = lhs;
  uint16_t size = result.size();
  if (Immediate::SignBit(size, size) & lhs) {
    result >>= rhs;
    result |= Immediate::Mask(size, size) << (size - (uint64_t)rhs);
  } else {
    result >>= rhs;
  }
  return result;
}

Immediate ZeroExtendImmediate(const Immediate& value, uint16_t size) {
  return value.ZeroExtend(size);
}

Immediate SignExtendImmediate(const Immediate& value, uint16_t size) {
  return value.SignExtend(size);
}

Immediate ExtractImmediate(const Immediate& value, uint16_t size) {
  return value.Extract(size);
}

uint16_t SizeImmediate(const Immediate& value) { return value.size(); }

std::string PrintImmediate(const Immediate& value) {
  std::stringstream stream;
  stream << value;
  return stream.str();
}
}  // namespace analysis
}  // namespace reil
