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

#include "analysis/constants_analysis.h"

#include "absl/memory/memory.h"
#include "glog/logging.h"

#include "reil/aarch64.h"

namespace reil {
namespace analysis {
class ConstantsStateImpl : public ConstantsState {
  // we use nullptr to represent `top` in this analysis
  std::map<uint8_t, std::shared_ptr<Immediate>> registers_;
  std::map<uint16_t, std::shared_ptr<Immediate>> temporaries_;
  bool valid_ = true;

 public:
  std::shared_ptr<Immediate> GetRegister(uint8_t index) const override;
  std::shared_ptr<Immediate> GetTemporary(uint16_t index) const override;
  std::shared_ptr<Immediate> GetOperand(const Operand& operand) const override;

  void SetRegister(uint8_t index, std::shared_ptr<Immediate> value) override;
  void SetRegister(uint8_t index, Immediate&& value) override;
  void SetRegister(uint8_t index, const Immediate& value) override;

  void SetTemporary(uint16_t index, std::shared_ptr<Immediate> value) override;
  void SetTemporary(uint16_t index, Immediate&& value) override;
  void SetTemporary(uint16_t index, const Immediate& value) override;

  void SetOperand(const Operand& operand,
                  std::shared_ptr<Immediate> value) override;
  void SetOperand(const Operand& operand, Immediate&& value) override;
  void SetOperand(const Operand& operand, const Immediate& value) override;

 private:
  std::shared_ptr<Immediate> GetRegisterImpl(uint8_t index) const;
  std::shared_ptr<Immediate> GetTemporaryImpl(uint16_t index) const;
  std::shared_ptr<Immediate> GetOperandImpl(const Operand& operand) const;

  void SetRegisterImpl(uint8_t index, std::shared_ptr<Immediate> value);
  void SetRegisterImpl(uint8_t index, Immediate&& value);
  void SetRegisterImpl(uint8_t index, const Immediate& value);

  void SetTemporaryImpl(uint16_t index, std::shared_ptr<Immediate> value);
  void SetTemporaryImpl(uint16_t index, Immediate&& value);
  void SetTemporaryImpl(uint16_t index, const Immediate& value);

  void SetOperandImpl(const Operand& operand, std::shared_ptr<Immediate> value);
  void SetOperandImpl(const Operand& operand, Immediate&& value);
  void SetOperandImpl(const Operand& operand, const Immediate& value);

  void ClearRegister(uint8_t index);
  void ClearTemporary(uint16_t index);
  void ClearOperand(const Operand& operand);

  void Clear();

  void ClearTemporaries();

  void TransformAdd(const Instruction& ri);
  void TransformAnd(const Instruction& ri);
  void TransformDiv(const Instruction& ri);
  void TransformJcc(const Instruction& ri, const Edge& edge);
  void TransformLdm(const Instruction& ri, const MemoryImage& memory_image);
  void TransformMod(const Instruction& ri);
  void TransformMul(const Instruction& ri);
  void TransformOr(const Instruction& ri);
  void TransformStr(const Instruction& ri);
  void TransformSub(const Instruction& ri);
  void TransformUndef(const Instruction& ri);
  void TransformUnkn(const Instruction& ri);
  void TransformXor(const Instruction& ri);
  void TransformEqu(const Instruction& ri);
  void TransformLshl(const Instruction& ri);
  void TransformLshr(const Instruction& ri);
  void TransformAshr(const Instruction& ri);
  void TransformSex(const Instruction& ri);
  void TransformSys(const Instruction& ri);
  void TransformIte(const Instruction& ri);

 public:
  void Transform(const Edge& edge, const Instruction& instruction,
                 const MemoryImage& memory_image);
  void Merge(const ConstantsStateImpl& other);
  bool Valid() const;

  friend bool operator==(const ConstantsStateImpl& lhs,
                         const ConstantsStateImpl& rhs);

  friend std::ostream& operator<<(std::ostream& stream,
                                  const ConstantsStateImpl& state);
};

std::shared_ptr<Immediate> ConstantsStateImpl::GetRegister(
    uint8_t index) const {
  return GetRegisterImpl(index);
}

std::shared_ptr<Immediate> ConstantsStateImpl::GetTemporary(
    uint16_t index) const {
  return GetTemporaryImpl(index);
}

std::shared_ptr<Immediate> ConstantsStateImpl::GetOperand(
    const Operand& operand) const {
  return GetOperandImpl(operand);
}

void ConstantsStateImpl::SetRegister(uint8_t index,
                                     std::shared_ptr<Immediate> value) {
  SetRegisterImpl(index, value);
}

void ConstantsStateImpl::SetRegister(uint8_t index, Immediate&& value) {
  SetRegisterImpl(index, std::move(value));
}

void ConstantsStateImpl::SetRegister(uint8_t index, const Immediate& value) {
  SetRegisterImpl(index, value);
}

void ConstantsStateImpl::SetTemporary(uint16_t index,
                                      std::shared_ptr<Immediate> value) {
  SetTemporaryImpl(index, value);
}

void ConstantsStateImpl::SetTemporary(uint16_t index, Immediate&& value) {
  SetTemporaryImpl(index, std::move(value));
}

void ConstantsStateImpl::SetTemporary(uint16_t index, const Immediate& value) {
  SetTemporaryImpl(index, value);
}

void ConstantsStateImpl::SetOperand(const Operand& operand,
                                    std::shared_ptr<Immediate> value) {
  SetOperandImpl(operand, value);
}

void ConstantsStateImpl::SetOperand(const Operand& operand, Immediate&& value) {
  SetOperandImpl(operand, std::move(value));
}

void ConstantsStateImpl::SetOperand(const Operand& operand,
                                    const Immediate& value) {
  SetOperandImpl(operand, value);
}

std::shared_ptr<Immediate> ConstantsStateImpl::GetRegisterImpl(
    uint8_t index) const {
  std::shared_ptr<Immediate> value = nullptr;
  auto reg_iter = registers_.find(index);
  if (reg_iter != registers_.end()) {
    value = reg_iter->second;
  }
  return value;
}

std::shared_ptr<Immediate> ConstantsStateImpl::GetTemporaryImpl(
    uint16_t index) const {
  std::shared_ptr<Immediate> value = nullptr;
  auto tmp_iter = temporaries_.find(index);
  if (tmp_iter != temporaries_.end()) {
    value = tmp_iter->second;
  }
  return value;
}

std::shared_ptr<Immediate> ConstantsStateImpl::GetOperandImpl(
    const Operand& operand) const {
  std::shared_ptr<Immediate> value = nullptr;
  if (operand.index() == kImmediate) {
    value = std::make_shared<Immediate>(absl::get<Immediate>(operand));
  } else if (operand.index() == kTemporary) {
    value = GetTemporaryImpl(absl::get<Temporary>(operand).index);
  } else if (operand.index() == kRegister) {
    value = GetRegisterImpl(absl::get<Register>(operand).index);
  } else {
    CHECK(false);
  }
  return value;
}

void ConstantsStateImpl::SetRegisterImpl(uint8_t index,
                                         std::shared_ptr<Immediate> value) {
  registers_[index] = value;
}

void ConstantsStateImpl::SetRegisterImpl(uint8_t index, Immediate&& value) {
  SetRegisterImpl(index, std::make_shared<Immediate>(std::move(value)));
}

void ConstantsStateImpl::SetRegisterImpl(uint8_t index,
                                         const Immediate& value) {
  SetRegisterImpl(index, std::make_shared<Immediate>(value));
}

void ConstantsStateImpl::SetTemporaryImpl(uint16_t index,
                                          std::shared_ptr<Immediate> value) {
  temporaries_[index] = value;
}

void ConstantsStateImpl::SetTemporaryImpl(uint16_t index, Immediate&& value) {
  SetTemporaryImpl(index, std::make_shared<Immediate>(std::move(value)));
}

void ConstantsStateImpl::SetTemporaryImpl(uint16_t index,
                                          const Immediate& value) {
  SetTemporaryImpl(index, std::make_shared<Immediate>(value));
}

void ConstantsStateImpl::SetOperandImpl(const Operand& operand,
                                        std::shared_ptr<Immediate> value) {
  if (operand.index() == kTemporary) {
    SetTemporaryImpl(absl::get<Temporary>(operand).index, value);
  } else if (operand.index() == kRegister) {
    SetRegisterImpl(absl::get<Register>(operand).index, value);
  } else {
    CHECK(false);
  }
}

void ConstantsStateImpl::SetOperandImpl(const Operand& operand,
                                        Immediate&& value) {
  if (operand.index() == kTemporary) {
    SetTemporaryImpl(absl::get<Temporary>(operand).index, std::move(value));
  } else if (operand.index() == kRegister) {
    SetRegisterImpl(absl::get<Register>(operand).index, std::move(value));
  } else {
    CHECK(false);
  }
}

void ConstantsStateImpl::SetOperandImpl(const Operand& operand,
                                        const Immediate& value) {
  if (operand.index() == kTemporary) {
    SetTemporaryImpl(absl::get<Temporary>(operand).index, value);
  } else if (operand.index() == kRegister) {
    SetRegisterImpl(absl::get<Register>(operand).index, value);
  } else {
    CHECK(false);
  }
}

void ConstantsStateImpl::ClearRegister(uint8_t index) {
  auto reg_iter = registers_.find(index);
  if (reg_iter != registers_.end()) {
    registers_.erase(reg_iter);
  }
}

void ConstantsStateImpl::ClearTemporary(uint16_t index) {
  auto tmp_iter = temporaries_.find(index);
  if (tmp_iter != temporaries_.end()) {
    temporaries_.erase(tmp_iter);
  }
}

void ConstantsStateImpl::ClearOperand(const Operand& operand) {
  if (operand.index() == kTemporary) {
    ClearTemporary(absl::get<Temporary>(operand).index);
  } else if (operand.index() == kRegister) {
    ClearRegister(absl::get<Register>(operand).index);
  } else {
    CHECK(false);
  }
}

void ConstantsStateImpl::Clear() {
  if (registers_.size() || temporaries_.size()) {
    registers_.clear();
    temporaries_.clear();
  }
}

void ConstantsStateImpl::ClearTemporaries() {
  if (temporaries_.size()) {
    temporaries_.clear();
  }
}

void ConstantsStateImpl::TransformAdd(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    SetOperandImpl(ri.output, *lhs + *rhs);
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformAnd(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    SetOperandImpl(ri.output, *lhs & *rhs);
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformDiv(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    SetOperandImpl(ri.output, *lhs / *rhs);
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformJcc(const Instruction& ri, const Edge& edge) {
  auto predicate = GetOperandImpl(ri.input0);
  auto target = GetOperandImpl(ri.output);

  // TODO: check that all the possible constraints on valid states are being
  // applied here.

  if (predicate) {
    uint64_t predicate_value = static_cast<uint64_t>(*predicate);
    if (predicate_value) {
      if (edge.kind == EdgeKind::kNativeJump ||
          edge.kind == EdgeKind::kNativeCall) {
        if (edge.target.address && target &&
            edge.target.address != static_cast<uint64_t>(*target)) {
          valid_ = false;
        }
      } else if (edge.kind == EdgeKind::kFlow ||
                 edge.kind == EdgeKind::kNativeFlow) {
        valid_ = false;
      }
    } else {
      if (edge.kind != EdgeKind::kFlow && edge.kind != EdgeKind::kNativeFlow) {
        valid_ = false;
      }
    }
  } else {
    if (edge.kind == EdgeKind::kNativeJump ||
        edge.kind == EdgeKind::kNativeJump) {
      SetOperandImpl(ri.output, Immediate(64, edge.target.address));
    }
  }
}

void ConstantsStateImpl::TransformLdm(const Instruction& ri,
                                      const MemoryImage& memory_image) {
  auto value = GetOperand(ri.input0);
  if (value) {
    uint64_t address = static_cast<uint64_t>(*value);
    if (memory_image.readable(address, Size(ri.output)) &&
        !memory_image.writable(address, Size(ri.output))) {
      SetOperand(ri.output, Immediate(memory_image.Read(address).data(),
                                      Size(ri.output) / 8));
    }
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformMod(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    SetOperandImpl(ri.output, *lhs % *rhs);
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformMul(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    SetOperandImpl(ri.output, *lhs * *rhs);
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformOr(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    SetOperandImpl(ri.output, *lhs | *rhs);
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformStr(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  if (lhs) {
    if (lhs->size() < Size(ri.output)) {
      SetOperandImpl(ri.output, lhs->ZeroExtend(Size(ri.output)));
    } else if (lhs->size() > Size(ri.output)) {
      SetOperandImpl(ri.output, lhs->Extract(Size(ri.output)));
    } else {
      SetOperandImpl(ri.output, lhs);
    }
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformSub(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    SetOperandImpl(ri.output, *lhs - *rhs);
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformUndef(const Instruction& ri) {
  ClearOperand(ri.output);
}

void ConstantsStateImpl::TransformUnkn(const Instruction& ri) { Clear(); }

void ConstantsStateImpl::TransformXor(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    SetOperandImpl(ri.output, *lhs ^ *rhs);
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformEqu(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    SetOperandImpl(ri.output, Immediate(Size(ri.output), *lhs == *rhs));
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformLshl(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    SetOperandImpl(ri.output, *lhs << *rhs);
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformLshr(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    SetOperandImpl(ri.output, *lhs >> *rhs);
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformAshr(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  auto rhs = GetOperandImpl(ri.input1);
  if (lhs && rhs) {
    Immediate result = *lhs;
    uint16_t size = result.size();
    if (Immediate::SignBit(size, size) & result) {
      result >>= *rhs;
      result |= Immediate::Mask(size, size) << (size - (uint64_t)*rhs);
    } else {
      result >>= *rhs;
    }
    std::cerr << result << std::endl;
    SetOperandImpl(ri.output, std::move(result));
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformSex(const Instruction& ri) {
  auto lhs = GetOperandImpl(ri.input0);
  if (lhs) {
    if (lhs->size() < Size(ri.output)) {
      SetOperandImpl(ri.output, lhs->SignExtend(Size(ri.output)));
    } else if (lhs->size() > Size(ri.output)) {
      SetOperandImpl(ri.output, lhs->Extract(Size(ri.output)));
    } else {
      SetOperandImpl(ri.output, lhs);
    }
  } else {
    SetOperandImpl(ri.output, nullptr);
  }
}

void ConstantsStateImpl::TransformSys(const Instruction& ri) {
  // TODO: check what registers can actually be trashed
  ClearRegister(reil::aarch64::kX0);
}

void ConstantsStateImpl::TransformIte(const Instruction& ri) {
  auto condition = GetOperandImpl(ri.input0);
  auto lhs = GetOperandImpl(ri.input1);
  auto rhs = GetOperandImpl(ri.input2);
  if (!condition) {
    if (lhs && rhs && *lhs == *rhs) {
      SetOperandImpl(ri.output, lhs);
    } else {
      SetOperandImpl(ri.output, nullptr);
    }
  } else {
    if (*condition && lhs) {
      SetOperandImpl(ri.output, lhs);
    } else if (!(*condition) && rhs) {
      SetOperandImpl(ri.output, rhs);
    } else {
      SetOperandImpl(ri.output, nullptr);
    }
  }
}

void ConstantsStateImpl::Transform(const Edge& edge, const Instruction& ri,
                                   const MemoryImage& memory_image) {
  switch (ri.opcode) {
    case Opcode::Add: {
      TransformAdd(ri);
    } break;

    case Opcode::And: {
      TransformAnd(ri);
    } break;

    case Opcode::Div: {
      TransformDiv(ri);
    } break;

    case Opcode::Jcc: {
      TransformJcc(ri, edge);
    } break;

    case Opcode::Ldm: {
      TransformLdm(ri, memory_image);
    } break;

    case Opcode::Mod: {
      TransformMod(ri);
    } break;

    case Opcode::Mul: {
      TransformMul(ri);
    } break;

    case Opcode::Nop: {
    } break;

    case Opcode::Or: {
      TransformOr(ri);
    } break;

    case Opcode::Stm: {
    } break;

    case Opcode::Str: {
      TransformStr(ri);
    } break;

    case Opcode::Sub: {
      TransformSub(ri);
    } break;

    case Opcode::Undef: {
      TransformUndef(ri);
    } break;

    case Opcode::Unkn: {
      TransformUnkn(ri);
    } break;

    case Opcode::Xor: {
      TransformXor(ri);
    } break;

    case Opcode::Equ: {
      TransformEqu(ri);
    } break;

    case Opcode::Lshl: {
      TransformLshl(ri);
    } break;

    case Opcode::Lshr: {
      TransformLshr(ri);
    } break;

    case Opcode::Ashr: {
      TransformAshr(ri);
    } break;

    case Opcode::Sex: {
      TransformSex(ri);
    } break;

    case Opcode::Sys: {
      TransformSys(ri);
    } break;

    case Opcode::Ite: {
      TransformIte(ri);
    } break;

    default:
      CHECK(false);
  }

  if (edge.kind >= EdgeKind::kNativeFlow) {
    ClearTemporaries();
  }
}

void ConstantsStateImpl::Merge(const ConstantsStateImpl& other) {
  auto reg_iter = registers_.begin();
  auto other_reg_iter = other.registers_.begin();
  while (reg_iter != registers_.end() &&
         other_reg_iter != other.registers_.end()) {
    if (reg_iter->first < other_reg_iter->first) {
      ++reg_iter;
    } else if (reg_iter->first == other_reg_iter->first) {
      // send the value to top if we are merging top or non-equal values.
      if (reg_iter->second == nullptr || other_reg_iter->second == nullptr ||
          *(reg_iter->second) != *(other_reg_iter->second)) {
        reg_iter->second = nullptr;
      }
      ++reg_iter;
      ++other_reg_iter;
    } else {
      reg_iter = registers_.insert(
          reg_iter,
          std::make_pair(other_reg_iter->first, other_reg_iter->second));
      ++reg_iter;
      ++other_reg_iter;
    }
  }

  while (other_reg_iter != other.registers_.end()) {
    registers_.emplace(other_reg_iter->first, other_reg_iter->second);
    ++other_reg_iter;
  }

  auto tmp_iter = temporaries_.begin();
  auto other_tmp_iter = other.temporaries_.begin();
  while (tmp_iter != temporaries_.end() &&
         other_tmp_iter != other.temporaries_.end()) {
    if (tmp_iter->first < other_tmp_iter->first) {
      ++tmp_iter;
    } else if (tmp_iter->first == other_tmp_iter->first) {
      // send the value to top if we are merging top or non-equal values.
      if (tmp_iter->second == nullptr || other_tmp_iter->second == nullptr ||
          *(tmp_iter->second) != *(other_tmp_iter->second)) {
        tmp_iter->second = nullptr;
      }
      ++tmp_iter;
      ++other_tmp_iter;
    } else {
      tmp_iter = temporaries_.insert(
          tmp_iter,
          std::make_pair(other_tmp_iter->first, other_tmp_iter->second));
      ++tmp_iter;
      ++other_tmp_iter;
    }
  }

  while (other_tmp_iter != other.temporaries_.end()) {
    temporaries_.emplace(other_tmp_iter->first, other_tmp_iter->second);
    ++other_tmp_iter;
  }
}

bool ConstantsStateImpl::Valid() const { return valid_; }

bool operator==(const ConstantsStateImpl& lhs, const ConstantsStateImpl& rhs) {
  if (lhs.registers_.size() != rhs.registers_.size() ||
      lhs.temporaries_.size() != rhs.temporaries_.size()) {
    return false;
  }

  auto lhs_reg_iter = lhs.registers_.begin();
  auto rhs_reg_iter = rhs.registers_.begin();
  while (lhs_reg_iter != lhs.registers_.end() &&
         rhs_reg_iter != rhs.registers_.end()) {
    if (lhs_reg_iter->first != rhs_reg_iter->first) {
      return false;
    }

    if (lhs_reg_iter->second == nullptr && rhs_reg_iter->second != nullptr) {
      return false;
    }

    if (lhs_reg_iter->second != nullptr && rhs_reg_iter->second == nullptr) {
      return false;
    }

    if (lhs_reg_iter->second != nullptr && rhs_reg_iter->second != nullptr &&
        *lhs_reg_iter->second != *rhs_reg_iter->second) {
      return false;
    }

    ++lhs_reg_iter;
    ++rhs_reg_iter;
  }

  auto lhs_tmp_iter = lhs.temporaries_.begin();
  auto rhs_tmp_iter = rhs.temporaries_.begin();
  while (lhs_tmp_iter != lhs.temporaries_.end() &&
         rhs_tmp_iter != rhs.temporaries_.end()) {
    if (lhs_tmp_iter->first != rhs_tmp_iter->first) {
      return false;
    }

    if (lhs_tmp_iter->second == nullptr && rhs_tmp_iter->second != nullptr) {
      return false;
    }

    if (lhs_tmp_iter->second != nullptr && rhs_tmp_iter->second == nullptr) {
      return false;
    }

    if (lhs_tmp_iter->second != nullptr && rhs_tmp_iter->second != nullptr &&
        *lhs_tmp_iter->second != *rhs_tmp_iter->second) {
      return false;
    }

    ++lhs_tmp_iter;
    ++rhs_tmp_iter;
  }

  return true;
}

bool operator!=(const ConstantsStateImpl& lhs, const ConstantsStateImpl& rhs) {
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& stream,
                         const ConstantsStateImpl& state) {
  if (state.registers_.empty() && state.temporaries_.empty()) {
    stream << "{}";
  } else {
    stream << "{\n";
    for (auto reg_iter : state.registers_) {
      stream << "  \"" << reil::aarch64::RegisterName(reg_iter.first) << "\": ";
      if (reg_iter.second) {
        stream << *reg_iter.second << ",\n";
      } else {
        stream << "top,\n";
      }
    }
    for (auto tmp_iter : state.temporaries_) {
      stream << "  \"t" << std::dec << tmp_iter.first << "\": ";
      if (tmp_iter.second) {
        stream << *tmp_iter.second << ",\n";
      } else {
        stream << "top,\n";
      }
    }
    stream << "}";
  }
  return stream;
}

class LocalConstantsAnalysisImpl : public ConstantsAnalysis {
  Session& session_;

  uint64_t address_;
  std::map<Edge, std::unique_ptr<ConstantsStateImpl>> edge_states_;

  std::unique_ptr<ConstantsStateImpl> AtImpl(const Node& node);

 public:
  LocalConstantsAnalysisImpl(Session& session, uint64_t address);

  std::unique_ptr<ConstantsState> At(const Node& node) override;
  std::unique_ptr<ConstantsState> On(const Edge& edge) override;
  void Update() override;
};

LocalConstantsAnalysisImpl::LocalConstantsAnalysisImpl(Session& session,
                                                       uint64_t address)
    : session_(session), address_(address) {}

std::unique_ptr<ConstantsStateImpl> LocalConstantsAnalysisImpl::AtImpl(
    const Node& node) {
  auto flow_graph = session_.flow_graph(address_);
  auto instruction_provider = session_.instruction_provider();

  // find the first node of the basic block containing this node
  Node in_node = flow_graph->BasicBlockStart(node);
  if (in_node.address != node.address) {
    LOG(WARNING) << "node not contained in flow_graph " << node;
    return nullptr;
  }

  // merge all input edge states
  std::unique_ptr<ConstantsStateImpl> in_state = nullptr;
  for (auto& in_edge : flow_graph->incoming_edges(in_node)) {
    auto in_state_iter = edge_states_.find(in_edge);
    if (in_state_iter != edge_states_.end()) {
      if (in_state) {
        in_state->Merge(*in_state_iter->second);
      } else {
        in_state =
            absl::make_unique<ConstantsStateImpl>(*in_state_iter->second.get());
      }
    }
  }

  if (!in_state) {
    in_state = absl::make_unique<ConstantsStateImpl>();
  }

  if (!in_state->Valid()) {
    LOG(WARNING) << "invalid in_state";
    return nullptr;
  }

  auto ni = instruction_provider->NativeInstruction(in_node.address);
  while (in_node != node) {
    DCHECK(in_node.offset < ni->reil.size() - 1);
    Node next_in_node(in_node.address, in_node.offset + 1);

    VLOG(4) << "in_state: " << *in_state;
    VLOG(3) << in_node << " " << ni->reil[in_node.offset];

    in_state->Transform(Edge(in_node, next_in_node, EdgeKind::kFlow),
                        ni->reil[in_node.offset], session_.memory_image());

    if (!in_state->Valid()) {
      LOG(WARNING) << "invalid in_state";
      return nullptr;
    }

    in_node = next_in_node;
  }

  return in_state;
}

void LocalConstantsAnalysisImpl::Update() {
  auto flow_graph = session_.flow_graph(address_);
  auto instruction_provider = session_.instruction_provider();

  std::set<Node> queue = {Node(address_)};

  // run the analysis
  while (!queue.empty()) {
    Node node = *queue.begin();
    queue.erase(queue.begin());

    if (node == 0) {
      continue;
    }

    // compute the input state to the node
    node = flow_graph->BasicBlockEnd(node);
    auto in_state = AtImpl(node);

    // for each out edge, transform the in state to the new out state.
    auto ri = instruction_provider->Instruction(node);
    for (auto& out_edge : flow_graph->outgoing_edges(node)) {
      auto out_state = absl::make_unique<ConstantsStateImpl>(*in_state);

      VLOG(4) << "in_state: " << *out_state;
      VLOG(3) << node << " " << ri;
      out_state->Transform(out_edge, ri, session_.memory_image());

      if (!out_state->Valid()) {
        continue;
      }

      auto state_iter = edge_states_.find(out_edge);
      if (state_iter == edge_states_.end()) {
        VLOG(1) << "new_out_state: " << out_edge << " " << *out_state;
        edge_states_.emplace(out_edge, std::move(out_state));
        if (out_edge.kind < EdgeKind::kNativeCall) {
          queue.insert(out_edge.target);
        }
      } else if (*state_iter->second == *out_state) {
        VLOG(1) << "equal_out_state: " << out_edge << " " << *out_state;
      } else {
        VLOG(1) << "updated_out_state: " << out_edge << " " << *out_state;
        state_iter->second = std::move(out_state);
        if (out_edge.kind < EdgeKind::kNativeCall) {
          queue.insert(out_edge.target);
        }
      }
    }
  }
}

std::unique_ptr<ConstantsState> LocalConstantsAnalysisImpl::At(
    const Node& node) {
  return AtImpl(node);
}

std::unique_ptr<ConstantsState> LocalConstantsAnalysisImpl::On(
    const Edge& edge) {
  std::unique_ptr<ConstantsState> state = nullptr;
  auto state_iter = edge_states_.find(edge);
  if (state_iter != edge_states_.end()) {
    state = absl::make_unique<ConstantsStateImpl>(*state_iter->second);
  }
  return state;
}

std::unique_ptr<ConstantsAnalysis> LocalConstantsAnalysis(Session& session,
                                                          uint64_t address) {
  auto analysis =
      absl::make_unique<LocalConstantsAnalysisImpl>(session, address);
  analysis->Update();
  return analysis;
}
}  // namespace analysis
}  // namespace reil