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

#ifndef REIL_META_DATAFLOW_H_

#include <functional>

#include "glog/logging.h"

#include "analysis/edge_dependent_analysis.h"
#include "analysis/register_state.h"
#include "analysis/temporary_state.h"
#include "analysis/value/immediate.h"
#include "analysis/value/operations.h"
#include "analysis/value/top.h"
#include "memory_image/memory_image.h"

namespace reil {
namespace analysis {
template <typename Operations = ImmediateOperations, typename... Ts>
class DataflowState
    : public EdgeDependentAnalysis,
      public AArch64RegisterState<absl::variant<Top, Immediate, Ts...>,
                                  Operations>,
      public TemporaryState<absl::variant<Top, Immediate, Ts...>, Operations> {
 private:
  const MemoryImage* memory_image_;

 public:
  using ValueType = absl::variant<Top, Immediate, Ts...>;
  using StateType = DataflowState<Operations, Ts...>;

  DataflowState(const MemoryImage& memory_image)
      : EdgeDependentAnalysis(), memory_image_(&memory_image) {}

  ValueType GetOperand(const Operand& operand);
  void SetOperand(const Operand& operand, ValueType&& value);
  void SetOperand(const Operand& operand, const ValueType& value);
  void ClearOperand(const Operand& operand);

  void ConstrainZero(ValueType& value);
  void ConstrainNonZero(ValueType& value);

  void TransformAdd(const Edge& edge, const Instruction& ri) override;
  void TransformAnd(const Edge& edge, const Instruction& ri) override;
  void TransformDiv(const Edge& edge, const Instruction& ri) override;
  void TransformJcc(const Edge& edge, const Instruction& ri) override;
  void TransformLdm(const Edge& edge, const Instruction& ri) override;
  void TransformMod(const Edge& edge, const Instruction& ri) override;
  void TransformMul(const Edge& edge, const Instruction& ri) override;
  void TransformOr(const Edge& edge, const Instruction& ri) override;
  void TransformStr(const Edge& edge, const Instruction& ri) override;
  void TransformSub(const Edge& edge, const Instruction& ri) override;
  void TransformUndef(const Edge& edge, const Instruction& ri) override;
  void TransformUnkn(const Edge& edge, const Instruction& ri) override;
  void TransformXor(const Edge& edge, const Instruction& ri) override;
  void TransformEqu(const Edge& edge, const Instruction& ri) override;
  void TransformLshl(const Edge& edge, const Instruction& ri) override;
  void TransformLshr(const Edge& edge, const Instruction& ri) override;
  void TransformAshr(const Edge& edge, const Instruction& ri) override;
  void TransformSex(const Edge& edge, const Instruction& ri) override;
  void TransformSys(const Edge& edge, const Instruction& ri) override;
  void TransformIte(const Edge& edge, const Instruction& ri) override;

  void OnNativeInstructionEnd() override;

  std::string Print() const override;

  template <typename... Ss>
  friend bool operator==(const DataflowState<Ss...>& lhs,
                         const DataflowState<Ss...>& rhs);

  static StateType Merge(absl::Span<const StateType*> in_states);
};

template <typename Operations, typename... Ts>
auto DataflowState<Operations, Ts...>::GetOperand(const Operand& operand)
    -> ValueType {
  ValueType value;
  if (operand.index() == kImmediate) {
    value = absl::get<Immediate>(operand);
  } else if (operand.index() == kTemporary) {
    value = this->GetTemporary(absl::get<Temporary>(operand));
  } else if (operand.index() == kRegister) {
    value = this->GetRegister(absl::get<Register>(operand));
  } else {
    CHECK(false);
  }

  return value;
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::SetOperand(const Operand& operand,
                                                  ValueType&& value) {
  if (operand.index() == kTemporary) {
    this->SetTemporary(absl::get<Temporary>(operand), std::move(value));
  } else if (operand.index() == kRegister) {
    this->SetRegister(absl::get<Register>(operand), std::move(value));
  } else {
    CHECK(false);
  }
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::SetOperand(const Operand& operand,
                                                  const ValueType& value) {
  if (operand.index() == kTemporary) {
    this->SetTemporary(absl::get<Temporary>(operand), value);
  } else if (operand.index() == kRegister) {
    this->SetRegister(absl::get<Register>(operand), value);
  } else {
    CHECK(false);
  }
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::ClearOperand(const Operand& operand) {
  if (operand.index() == kTemporary) {
    this->ClearTemporary(absl::get<Temporary>(operand));
  } else if (operand.index() == kRegister) {
    this->ClearRegister(absl::get<Register>(operand));
  } else {
    CHECK(false);
  }
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::ConstrainZero(ValueType& value) {
  if (absl::holds_alternative<Immediate>(value)) {
    if (absl::get<Immediate>(value)) {
      this->Invalidate();
    }
  } else {
    // TODO: handle properly
  }
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::ConstrainNonZero(ValueType& value) {
  if (absl::holds_alternative<Immediate>(value)) {
    if (!absl::get<Immediate>(value)) {
      this->Invalidate();
    }
  } else {
    // TODO: handle properly
  }
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformAdd(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::Add(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformAnd(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::And(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformDiv(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::Divide(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformJcc(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType condition = GetOperand(ri.input0);
  if (edge.kind == EdgeKind::kFlow || edge.kind == EdgeKind::kNativeFlow) {
    if (ri.input1.index() == kImmediate) {
      if (absl::get<Immediate>(ri.input1) == kCall) {
        // call flow edges are unconditionally not taken, but for the purposes
        // of analysis they are actually always taken. This is a bit messy, and
        // maybe I should address this differently in the FlowGraph.
        return;
      }
    }
    ConstrainZero(condition);
  } else {
    ConstrainNonZero(condition);
    if (edge.target != 0 && !absl::holds_alternative<Offset>(ri.output)) {
      ValueType target = GetOperand(ri.output);
      ValueType edge_target = Immediate(64, edge.target.address);
      ValueType constraint = Operations::Equal(target, edge_target);
      ConstrainNonZero(constraint);
    }
  }
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformLdm(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType address_value = GetOperand(ri.input0);
  if (absl::holds_alternative<Immediate>(address_value)) {
    uint64_t address = (uint64_t)absl::get<Immediate>(address_value);
    if (memory_image_->readable(address, Size(ri.output)) &&
        !memory_image_->writable(address, Size(ri.output))) {
      Immediate value =
          Immediate(memory_image_->Read(address).data(), Size(ri.output) / 8);
      SetOperand(ri.output, value);
    }
  } else {
    ClearOperand(ri.output);
  }
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformMod(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::Modulo(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformMul(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::Multiply(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformOr(const Edge& edge,
                                                   const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::Or(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformStr(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType value = GetOperand(ri.input0);
  if (Size(ri.output) > Operations::Size(value)) {
    value = Operations::ZeroExtend(value, Size(ri.output));
  } else if (Size(ri.output) < Operations::Size(value)) {
    value = Operations::Extract(value, Size(ri.output));
  }
  SetOperand(ri.output, std::move(value));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformSub(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::Subtract(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformUndef(const Edge& edge,
                                                      const Instruction& ri) {
  ClearOperand(ri.output);
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformUnkn(const Edge& edge,
                                                     const Instruction& ri) {
  Invalidate();
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformXor(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::Xor(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformEqu(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::Equal(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformLshl(const Edge& edge,
                                                     const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::LeftShift(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformLshr(const Edge& edge,
                                                     const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::RightShift(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformAshr(const Edge& edge,
                                                     const Instruction& ri) {
  ValueType lhs = GetOperand(ri.input0);
  ValueType rhs = GetOperand(ri.input1);
  SetOperand(ri.output, Operations::SignedRightShift(lhs, rhs));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformSex(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType value = GetOperand(ri.input0);
  if (Size(ri.output) > Operations::Size(value)) {
    value = Operations::SignExtend(value, Size(ri.output));
  } else if (Size(ri.output) < Operations::Size(value)) {
    value = Operations::Extract(value, Size(ri.output));
  }
  SetOperand(ri.output, std::move(value));
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformSys(const Edge& edge,
                                                    const Instruction& ri) {
  // TODO: implement
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::TransformIte(const Edge& edge,
                                                    const Instruction& ri) {
  ValueType condition = GetOperand(ri.input0);
  ValueType if_value = GetOperand(ri.input1);
  ValueType else_value = GetOperand(ri.input2);
  if (absl::holds_alternative<Immediate>(condition)) {
    if (absl::get<Immediate>(condition)) {
      SetOperand(ri.output, std::move(if_value));
    } else {
      SetOperand(ri.output, std::move(else_value));
    }
  } else {
    ValueType zero = Immediate(Operations::Size(condition));
    ValueType if_condition = Operations::NotEqual(condition, zero);
    if_condition =
        Operations::ZeroExtend(if_condition, Operations::Size(if_value));
    ValueType else_condition = Operations::NotEqual(condition, zero);
    else_condition =
        Operations::ZeroExtend(else_condition, Operations::Size(else_value));
    if_value = Operations::Multiply(if_condition, if_value);
    else_value = Operations::Multiply(else_condition, else_value);
    SetOperand(ri.output, Operations::Merge(if_value, else_value));
  }
}

template <typename Operations, typename... Ts>
void DataflowState<Operations, Ts...>::OnNativeInstructionEnd() {
  this->ClearTemporaries();
}

template <typename Operations, typename... Ts>
std::string DataflowState<Operations, Ts...>::Print() const {
  std::stringstream stream;
  if (this->RegisterCount() == 0 && this->TemporaryCount() == 0) {
    stream << "{}";
  } else {
    stream << "{\n";
    for (auto reg_iter = this->RegistersBegin();
         reg_iter != this->RegistersEnd(); ++reg_iter) {
      stream << "\"" << this->RegisterName(reg_iter->first)
             << "\": " << Operations::Print(reg_iter->second) << ",\n";
    }
    for (auto tmp_iter = this->TemporariesBegin();
         tmp_iter != this->TemporariesEnd(); ++tmp_iter) {
      stream << "\"t" << std::dec << tmp_iter->first
             << "\": " << Operations::Print(tmp_iter->second) << ",\n";
    }
    stream << "}";
  }
  return stream.str();
}

template <typename Operations, typename... Ts>
auto DataflowState<Operations, Ts...>::Merge(
    absl::Span<const StateType*> in_states) -> StateType {
  CHECK(in_states.size() != 0);
  StateType out_state = *in_states[0];

  for (size_t i = 1; i < in_states.size(); ++i) {
    auto& in_state = *in_states[i];
    if (!in_state.Valid()) {
      out_state.Invalidate();
      break;
    }

    out_state.MergeRegisters(in_state);
    out_state.MergeTemporaries(in_state);
  }

  return out_state;
}

template <typename Operations, typename... Ts>
bool operator==(const DataflowState<Operations, Ts...>& lhs,
                const DataflowState<Operations, Ts...>& rhs) {
  if (lhs.RegisterCount() != rhs.RegisterCount() ||
      lhs.TemporaryCount() != rhs.TemporaryCount() ||
      !lhs.CompareTemporaries(rhs) || !lhs.CompareRegisters(rhs)) {
    return false;
  }
  return true;
}

template <typename Operations, typename... Ts>
bool operator!=(const DataflowState<Operations, Ts...>& lhs,
                const DataflowState<Operations, Ts...>& rhs) {
  return !(lhs == rhs);
}
}  // namespace analysis
}  // namespace reil

#define REIL_ANALYSIS_DATAFLOW_H_
#endif  // REIL_ANALYSIS_DATAFLOW_H_
