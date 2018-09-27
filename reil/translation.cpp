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

#include "translation.h"

namespace reil {
Translation::Translation(uint32_t flags) : flags_(flags) {
  translation_.reserve(0x100);
}

Translation::~Translation() {}

Operand Translation::Tmp(uint16_t size) {
  struct Temporary tmp = {size, tmp_index_++};
  return tmp;
}

Operand Translation::Label() {
  struct Label label = {label_index_++};
  return label;
}

Operand Translation::Add(const Operand& input0, const Operand& input1,
                         const Operand& output) {
  translation_.push_back(reil::Add(input0, input1, output));
  return output;
}

Operand Translation::And(const Operand& input0, const Operand& input1,
                         const Operand& output) {
  translation_.push_back(reil::And(input0, input1, output));
  return output;
}

Operand Translation::Bisz(const Operand& input0, const Operand& output) {
  if (flags_ & kPureReil) {
    translation_.push_back(reil::Bisz(input0, output));
  } else {
    Ite(input0, Immediate(Size(output), 0), Immediate(Size(output), 1), output);
  }
  return output;
}

Operand Translation::Bsh(const Operand& input0, const Operand& input1,
                         const Operand& output) {
  translation_.push_back(reil::Bsh(input0, input1, output));
  return output;
}

Operand Translation::Div(const Operand& input0, const Operand& input1,
                         const Operand& output) {
  translation_.push_back(reil::Div(input0, input1, output));
  return output;
}

Operand Translation::Sdiv(const Operand& input0, const Operand& input1,
                          const Operand& output) {
  uint16_t size = Size(input0);

  Operand unsigned_lhs, sign_lhs;
  Operand unsigned_rhs, sign_rhs;

  std::tie(unsigned_lhs, sign_lhs) = SignedToUnsigned(input0);
  std::tie(unsigned_rhs, sign_rhs) = SignedToUnsigned(input1);

  auto unsigned_result = Div(unsigned_lhs, unsigned_rhs, Tmp(size));
  auto sign_result = Xor(sign_lhs, sign_rhs, Tmp(size));
  auto result = UnsignedToSigned(unsigned_result, sign_result);

  return Str(result, output);
}

void Translation::Jcc(const Operand& input0, const Operand& output) {
  translation_.push_back(reil::Jcc(input0, output));
}

void Translation::JccHint(const Operand& input0, const Operand& input1,
                          const Operand& output) {
  if (flags_ & kPureReil) {
    translation_.push_back(reil::Jcc(input0, output));
  } else {
    translation_.push_back(reil::JccHint(input0, input1, output));
  }
}

void Translation::Jump(const Operand& input0, const Operand& output) {
  JccHint(input0, kJump, output);
}

void Translation::Jump(const Operand& output) {
  JccHint(Imm8(1), kJump, output);
}

void Translation::Call(const Operand& output) {
  JccHint(Imm8(1), kCall, output);
}

void Translation::Return(const Operand& output) {
  JccHint(Imm8(1), kReturn, output);
}

Operand Translation::Ldm(const Operand& input0, const Operand& output) {
  translation_.push_back(reil::Ldm(input0, output));
  return output;
}

Operand Translation::Mod(const Operand& input0, const Operand& input1,
                         const Operand& output) {
  translation_.push_back(reil::Mod(input0, input1, output));
  return output;
}

Operand Translation::Mul(const Operand& input0, const Operand& input1,
                         const Operand& output) {
  translation_.push_back(reil::Mul(input0, input1, output));
  return output;
}

Operand Translation::Smul(const Operand& input0, const Operand& input1,
                          const Operand& output) {
  uint16_t size = Size(input0);
  return Str(Mul(Sex(input0, Tmp(size * 2)), Sex(input1, Tmp(size * 2))),
             Tmp(size * 2));
}

void Translation::Nop() { translation_.push_back(reil::Nop()); }

Operand Translation::Nop(const Operand& input0) {
  translation_.push_back(reil::Nop(input0));
  return input0;
}

Operand Translation::Not(const Operand& input0, const Operand& output) {
  translation_.push_back(
      reil::Xor(input0, Immediate::Mask(Size(input0)), output));
  return output;
}

Operand Translation::Or(const Operand& input0, const Operand& input1,
                        const Operand& output) {
  translation_.push_back(reil::Or(input0, input1, output));
  return output;
}

Operand Translation::Stm(const Operand& input0, const Operand& output) {
  translation_.push_back(reil::Stm(input0, output));
  return output;
}

Operand Translation::Str(const Operand& input0, const Operand& output) {
  translation_.push_back(reil::Str(input0, output));
  return output;
}

Operand Translation::Sub(const Operand& input0, const Operand& input1,
                         const Operand& output) {
  translation_.push_back(reil::Sub(input0, input1, output));
  return output;
}

void Translation::Undef(const Operand& output) {
  translation_.push_back(reil::Undef(output));
}

void Translation::Unkn() { translation_.push_back(reil::Unkn()); }

Operand Translation::Xor(const Operand& input0, const Operand& input1,
                         const Operand& output) {
  translation_.push_back(reil::Xor(input0, input1, output));
  return output;
}

Operand Translation::Bisnz(const Operand& input0, const Operand& output) {
  if (flags_ & kPureReil) {
    Bisz(Bisz(input0), output);
  } else {
    Ite(input0, Immediate(Size(output), 1), Immediate(Size(output), 0), output);
  }
  return output;
}

Operand Translation::Equ(const Operand& input0, const Operand& input1,
                         const Operand& output) {
  translation_.push_back(reil::Equ(input0, input1, output));
  return output;
}

Operand Translation::Lshl(const Operand& input0, const Operand& input1,
                          const Operand& output) {
  translation_.push_back(reil::Lshl(input0, input1, output));
  return output;
}

Operand Translation::Lshr(const Operand& input0, const Operand& input1,
                          const Operand& output) {
  translation_.push_back(reil::Lshr(input0, input1, output));
  return output;
}

Operand Translation::Ashr(const Operand& input0, const Operand& input1,
                          const Operand& output) {
  translation_.push_back(reil::Ashr(input0, input1, output));
  return output;
}

Operand Translation::Sex(const Operand& input0, const Operand& output) {
  translation_.push_back(reil::Sex(input0, output));
  return output;
}

void Translation::Sys(const Operand& input0) {
  translation_.push_back(reil::Sys(input0));
}

Operand Translation::Ite(const Operand& input0, const Operand& input1,
                         const Operand& input2, const Operand& output) {
  if (flags_ & kPureReil) {
    abort();
  } else {
    translation_.push_back(reil::Ite(input0, input1, input2, output));
    return output;
  }
}

Operand Translation::Add(const Operand& input0, const Operand& input1) {
  return Add(input0, input1, Tmp(Size(input0) * 2));
}

Operand Translation::Mul(const Operand& input0, const Operand& input1) {
  return Mul(input0, input1, Tmp(Size(input0) * 2));
}

Operand Translation::Smul(const Operand& input0, const Operand& input1) {
  return Smul(input0, input1, Tmp(Size(input0) * 2));
}

Operand Translation::Sub(const Operand& input0, const Operand& input1) {
  return Sub(input0, input1, Tmp(Size(input0) * 2));
}

Operand Translation::And(const Operand& input0, const Operand& input1) {
  return And(input0, input1, Tmp(Size(input0)));
}

Operand Translation::Div(const Operand& input0, const Operand& input1) {
  return Div(input0, input1, Tmp(Size(input0)));
}

Operand Translation::Sdiv(const Operand& input0, const Operand& input1) {
  return Sdiv(input0, input1, Tmp(Size(input0)));
}

Operand Translation::Mod(const Operand& input0, const Operand& input1) {
  return Mod(input0, input1, Tmp(Size(input0)));
}

Operand Translation::Not(const Operand& input0) {
  return Not(input0, Tmp(Size(input0)));
}

Operand Translation::Or(const Operand& input0, const Operand& input1) {
  return Or(input0, input1, Tmp(Size(input0)));
}

Operand Translation::Xor(const Operand& input0, const Operand& input1) {
  return Xor(input0, input1, Tmp(Size(input0)));
}

Operand Translation::Lshl(const Operand& input0, const Operand& input1) {
  return Lshl(input0, input1, Tmp(Size(input0)));
}

Operand Translation::Lshr(const Operand& input0, const Operand& input1) {
  return Lshr(input0, input1, Tmp(Size(input0)));
}

Operand Translation::Ashr(const Operand& input0, const Operand& input1) {
  return Ashr(input0, input1, Tmp(Size(input0)));
}

Operand Translation::Ite(const Operand& input0, const Operand& input1,
                         const Operand& input2) {
  return Ite(input0, input1, input2, Tmp(Size(input1)));
}

Operand Translation::Bisz(const Operand& input0) {
  return Bisz(input0, Tmp(8));
}

Operand Translation::Bisnz(const Operand& input0) {
  return Bisnz(input0, Tmp(8));
}

Operand Translation::Equ(const Operand& input0, const Operand& input1) {
  return Equ(input0, input1, Tmp(8));
}

std::tuple<Operand, Operand> Translation::SignedToUnsigned(
    const Operand& value) {
  uint16_t size = Size(value);

  auto sign_bit = And(value, Immediate::SignBit(size, size));

  auto negative = Bisnz(sign_bit, Tmp(size));
  auto tmp1 = Str(Add(Not(value), Imm(size, 1)), Tmp(size));
  auto negative_value = Str(Mul(negative, tmp1), Tmp(size));

  auto positive = Bisz(sign_bit, Tmp(size));
  auto tmp2 = And(value, Immediate::SignedMax(size));
  auto positive_value = Str(Mul(positive, tmp2), Tmp(size));

  auto result = Or(negative_value, positive_value);

  return {result, negative};
}

Operand Translation::UnsignedToSigned(const Operand& value,
                                      const Operand& sign) {
  uint16_t size = Size(value);

  auto positive = Bisz(sign, Tmp(size));
  auto tmp1 = And(value, Immediate::SignedMax(size), Tmp(size));
  auto tmp2 = Str(Add(Not(value), Imm(size, 1)), Tmp(size));
  auto negative_value = Str(Mul(sign, tmp2), Tmp(size));
  auto positive_value = Str(Mul(positive, tmp1), Tmp(size));
  auto result = Or(negative_value, positive_value, Tmp(size));

  return result;
}

std::vector<Instruction> Translation::Finalise() {
  // TODO: Optimise this.
  for (size_t i = 0; i < translation_.size(); ++i) {
    auto ri = translation_[i];
    if (ri.opcode == Opcode::Nop && ri.input0.index() == kLabel) {
      auto li = absl::get<reil::Label>(ri.input0);
      for (auto& rj : translation_) {
        if (rj.opcode == Opcode::Jcc && rj.output.index() == kLabel) {
          auto lj = absl::get<reil::Label>(rj.output);
          if (li.index == lj.index) {
            rj.output = Offset(i);
          }
        }
      }

      if (i != translation_.size() - 1) {
        translation_.erase(translation_.begin() + i);
      }
    }
  }

  // std::cerr << tmp_index_ << std::endl;

  return std::move(translation_);
}
}  // namespace reil
