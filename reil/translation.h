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

#ifndef REIL_TRANSLATION_H_

#include <cstdint>

#include <memory>
#include <vector>

#include "reil.h"

namespace reil {
enum TranslationFlags {
  kDefaultFlags = 0,
  kMinimalBranches = 1,
  kPureReil = 1 << 1,
  kNoMnemonics = 1 << 2,

  kPerArchitectureFlag = 1 << 24,
};

class Translation {
 protected:
  uint32_t flags_;
  bool valid_;

  uint16_t tmp_index_ = 0;
  uint8_t label_index_ = 0;
  std::vector<Instruction> translation_;

  Operand Tmp(uint16_t size);
  Operand Label();

  // actual REIL instructions - depending on flags, some of these will be
  // translated to multiple instructions.
  Operand Add(Operand input0, Operand input1, Operand output);
  Operand And(Operand input0, Operand input1, Operand output);
  Operand Bisz(Operand input0, Operand output);
  Operand Bsh(Operand input0, Operand input1, Operand output);
  Operand Div(Operand input0, Operand input1, Operand output);
  Operand Sdiv(Operand input0, Operand input1, Operand output);
  void Jcc(Operand input0, Operand output);
  void JccHint(Operand input0, Operand input1, Operand output);
  void Jump(Operand input0, Operand output);
  void Jump(Operand output);
  void Call(Operand output);
  void Return(Operand output);
  Operand Ldm(Operand input0, Operand output);
  Operand Mod(Operand input0, Operand input1, Operand output);
  Operand Mul(Operand input0, Operand input1, Operand output);
  Operand Smul(Operand input0, Operand input1, Operand output);
  void Nop();
  Operand Nop(Operand input0);
  Operand Not(Operand input0, Operand output);
  Operand Or(Operand input0, Operand input1, Operand output);
  Operand Stm(Operand input0, Operand output);
  Operand Str(Operand input0, Operand output);
  Operand Sub(Operand input0, Operand input1, Operand output);
  void Undef(Operand output);
  void Unkn();
  Operand Xor(Operand input0, Operand input1, Operand output);
  Operand Bisnz(Operand input0, Operand output);
  Operand Equ(Operand input0, Operand input1, Operand output);
  Operand Lshl(Operand input0, Operand input1, Operand output);
  Operand Lshr(Operand input0, Operand input1, Operand output);
  Operand Ashr(Operand input0, Operand input1, Operand output);
  Operand Sex(Operand input0, Operand output);
  void Sys(Operand input0);
  Operand Ite(Operand input0, Operand input1, Operand input2, Operand output);

  // assigns result to a new temporary of input size * 2
  Operand Add(Operand input0, Operand input1);
  Operand Mul(Operand input0, Operand input1);
  Operand Smul(Operand input0, Operand input1);
  Operand Sub(Operand input0, Operand input1);

  // assigns result to a new temporary of input size
  Operand And(Operand input0, Operand input1);
  Operand Div(Operand input0, Operand input1);
  Operand Sdiv(Operand input0, Operand input1);
  Operand Mod(Operand input0, Operand input1);
  Operand Not(Operand input0);
  Operand Or(Operand input0, Operand input1);
  Operand Xor(Operand input0, Operand input1);
  Operand Lshl(Operand input0, Operand input1);
  Operand Lshr(Operand input0, Operand input1);
  Operand Ashr(Operand input0, Operand input1);
  Operand Ite(Operand input0, Operand input1, Operand input2);

  // assigns result to a new temporary of size 8
  Operand Bisz(Operand input0);
  Operand Bisnz(Operand input0);
  Operand Equ(Operand input0, Operand input1);

  std::tuple<Operand, Operand> SignedToUnsigned(Operand value);
  Operand UnsignedToSigned(Operand value, Operand sign);

 public:
  explicit Translation(uint32_t flags);
  virtual ~Translation();

  bool valid() const { return valid_; }

  std::vector<Instruction> Finalise();
};
}  // namespace reil

#define REIL_TRANSLATION_H_
#endif  // REIL_TRANSLATION_H_
