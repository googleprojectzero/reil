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
  kPositionIndependent = 1 << 2,
  kNoMnemonics = 1 << 3,

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
  Operand Add(const Operand& input0, const Operand& input1,
              const Operand& output);
  Operand And(const Operand& input0, const Operand& input1,
              const Operand& output);
  Operand Bisz(const Operand& input0, const Operand& output);
  Operand Bsh(const Operand& input0, const Operand& input1,
              const Operand& output);
  Operand Div(const Operand& input0, const Operand& input1,
              const Operand& output);
  Operand Sdiv(const Operand& input0, const Operand& input1,
               const Operand& output);
  void Jcc(const Operand& input0, const Operand& output);
  void JccHint(const Operand& input0, const Operand& input1,
               const Operand& output);
  void Jump(const Operand& input0, const Operand& output);
  void Jump(const Operand& output);
  void Call(const Operand& output);
  void Return(const Operand& output);
  Operand Ldm(const Operand& input0, const Operand& output);
  Operand Mod(const Operand& input0, const Operand& input1,
              const Operand& output);
  Operand Mul(const Operand& input0, const Operand& input1,
              const Operand& output);
  Operand Smul(const Operand& input0, const Operand& input1,
               const Operand& output);
  void Nop();
  Operand Nop(const Operand& input0);
  Operand Not(const Operand& input0, const Operand& output);
  Operand Or(const Operand& input0, const Operand& input1,
             const Operand& output);
  Operand Stm(const Operand& input0, const Operand& output);
  Operand Str(const Operand& input0, const Operand& output);
  Operand Sub(const Operand& input0, const Operand& input1,
              const Operand& output);
  void Undef(const Operand& output);
  void Unkn();
  Operand Xor(const Operand& input0, const Operand& input1,
              const Operand& output);
  Operand Bisnz(const Operand& input0, const Operand& output);
  Operand Equ(const Operand& input0, const Operand& input1,
              const Operand& output);
  Operand Lshl(const Operand& input0, const Operand& input1,
               const Operand& output);
  Operand Lshr(const Operand& input0, const Operand& input1,
               const Operand& output);
  Operand Ashr(const Operand& input0, const Operand& input1,
               const Operand& output);
  Operand Sex(const Operand& input0, const Operand& output);
  void Sys(const Operand& input0);
  Operand Ite(const Operand& input0, const Operand& input1,
              const Operand& input2, const Operand& output);

  // assigns result to a new temporary of input size * 2
  Operand Add(const Operand& input0, const Operand& input1);
  Operand Mul(const Operand& input0, const Operand& input1);
  Operand Smul(const Operand& input0, const Operand& input1);
  Operand Sub(const Operand& input0, const Operand& input1);

  // assigns result to a new temporary of input size
  Operand And(const Operand& input0, const Operand& input1);
  Operand Div(const Operand& input0, const Operand& input1);
  Operand Sdiv(const Operand& input0, const Operand& input1);
  Operand Mod(const Operand& input0, const Operand& input1);
  Operand Not(const Operand& input0);
  Operand Or(const Operand& input0, const Operand& input1);
  Operand Xor(const Operand& input0, const Operand& input1);
  Operand Lshl(const Operand& input0, const Operand& input1);
  Operand Lshr(const Operand& input0, const Operand& input1);
  Operand Ashr(const Operand& input0, const Operand& input1);
  Operand Ite(const Operand& input0, const Operand& input1,
              const Operand& input2);

  // assigns result to a new temporary of size 8
  Operand Bisz(const Operand& input0);
  Operand Bisnz(const Operand& input0);
  Operand Equ(const Operand& input0, const Operand& input1);

  std::tuple<Operand, Operand> SignedToUnsigned(const Operand& value);
  Operand UnsignedToSigned(const Operand& value, const Operand& sign);

 public:
  explicit Translation(uint32_t flags);
  virtual ~Translation();

  bool valid() const { return valid_; }

  std::vector<Instruction> Finalise();
};
}  // namespace reil

#define REIL_TRANSLATION_H_
#endif  // REIL_TRANSLATION_H_
