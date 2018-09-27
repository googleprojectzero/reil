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

#ifndef REIL_ANALYSIS_VALUE_IMMEDIATE_H_

#include "analysis/value/operations.h"
#include "analysis/value/top.h"

#include "reil/reil.h"

namespace reil {
namespace analysis {
absl::variant<Top, Immediate> MergeImmediates(const Immediate& lhs,
                                              const Immediate& rhs);
Immediate AddImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate SubtractImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate MultiplyImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate DivideImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate ModuloImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate AndImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate OrImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate XorImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate EqualImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate NotEqualImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate LeftShiftImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate RightShiftImmediates(const Immediate& lhs, const Immediate& rhs);
Immediate SignedRightShiftImmediates(const Immediate& lhs,
                                     const Immediate& rhs);

Immediate ZeroExtendImmediate(const Immediate& value, uint16_t size);
Immediate SignExtendImmediate(const Immediate& value, uint16_t size);
Immediate ExtractImmediate(const Immediate& value, uint16_t size);

uint16_t SizeImmediate(const Immediate& value);
std::string PrintImmediate(const Immediate& value);

// clang-format off
using ImmediateOperations =
  Operations<absl::variant<Top, Immediate>>
      ::SetMerge<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(MergeImmediates)>>
      ::SetAdd<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(AddImmediates)>>
      ::SetSubtract<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(SubtractImmediates)>>
      ::SetMultiply<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(MultiplyImmediates)>>
      ::SetDivide<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(DivideImmediates)>>
      ::SetModulo<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(ModuloImmediates)>>
      ::SetAnd<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(AndImmediates)>>
      ::SetOr<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(OrImmediates)>>
      ::SetXor<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(XorImmediates)>>
      ::SetEqual<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(EqualImmediates)>>
      ::SetNotEqual<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(NotEqualImmediates)>>
      ::SetLeftShift<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(LeftShiftImmediates)>>
      ::SetRightShift<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(RightShiftImmediates)>>
      ::SetSignedRightShift<
          BinaryOperation<absl::variant<Top, Immediate>,
              binary_operation(SignedRightShiftImmediates)>>
      ::SetZeroExtend<
          ResizeOperation<absl::variant<Top, Immediate>,
              resize_operation(ZeroExtendImmediate)>>
      ::SetSignExtend<
          ResizeOperation<absl::variant<Top, Immediate>,
              resize_operation(SignExtendImmediate)>>
      ::SetExtract<
          ResizeOperation<absl::variant<Top, Immediate>,
              resize_operation(ExtractImmediate)>>
      ::SetPrint<
          UnaryOperation<std::string, absl::variant<Top, Immediate>,
              unary_operation(PrintTop),
              unary_operation(PrintImmediate)>>
      ::SetSize<
          UnaryOperation<uint16_t, absl::variant<Top, Immediate>,
              unary_operation(SizeImmediate)>>;
// clang-format on
}  // namespace analysis
}  // namespace reil

#define REIL_ANALYSIS_VALUE_IMMEDIATE_H_
#endif  // REIL_ANALYSIS_VALUE_IMMEDIATE_H_