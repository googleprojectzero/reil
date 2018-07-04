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

#ifndef REIL_REIL_H_

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include <absl/types/variant.h>

#include "immediate.h"

namespace reil {

enum class Opcode : uint8_t {
  Add,
  And,
  Bisz,
  Bsh,
  Div,
  Jcc,
  Ldm,
  Mod,
  Mul,
  Nop,
  Or,
  Stm,
  Str,
  Sub,
  Undef,
  Unkn,
  Xor,

  Bisnz,
  Equ,
  Lshl,
  Lshr,
  Ashr,
  Sex,
  Sys,

  Ite,
};

enum OperandType {
  kNone,
  kImmediate,
  kOffset,
  kRegister,
  kTemporary,
  kLabel,
};

struct Offset {
  uint16_t offset;

  Offset(uint16_t);
};

struct Register {
  uint16_t size;
  uint8_t index;
  std::string name;

  Register(uint16_t size_, uint8_t index_, std::string name_);
};

struct Temporary {
  uint16_t size;
  uint16_t index;

  Temporary(uint16_t size_, uint16_t index_);
};

struct Label {
  uint8_t index;

  Label(uint8_t index_);
};

typedef absl::variant<int, Immediate, Offset, Register, Temporary, Label>
    Operand;

// hints for the jcc instruction
extern const Operand kJump;
extern const Operand kCall;
extern const Operand kReturn;

struct Instruction {
  Opcode opcode;
  Operand input0;
  Operand input1;
  Operand input2;
  Operand output;
};

struct NativeInstruction {
  uint64_t address;
  uint8_t size;
  std::string mnemonic;
  std::vector<Instruction> reil;
};

Instruction Add(Operand input0, Operand input1, Operand output);
Instruction And(Operand input0, Operand input1, Operand output);
Instruction Bisz(Operand input0, Operand output);
Instruction Bsh(Operand input0, Operand input1, Operand output);
Instruction Div(Operand input0, Operand input1, Operand output);
Instruction Jcc(Operand input0, Operand output);
Instruction JccHint(Operand input0, Operand input1, Operand output);
Instruction Ldm(Operand input0, Operand output);
Instruction Mod(Operand input0, Operand input1, Operand output);
Instruction Mul(Operand input0, Operand input1, Operand output);
Instruction Nop();
Instruction Nop(Operand input0);
Instruction Or(Operand input0, Operand input1, Operand output);
Instruction Stm(Operand input0, Operand output);
Instruction Str(Operand input0, Operand output);
Instruction Sub(Operand input0, Operand input1, Operand output);
Instruction Undef(Operand output);
Instruction Unkn();
Instruction Xor(Operand input0, Operand input1, Operand output);
Instruction Bisnz(Operand input0, Operand output);
Instruction Equ(Operand input0, Operand input1, Operand output);
Instruction Lshl(Operand input0, Operand input1, Operand output);
Instruction Lshr(Operand input0, Operand input1, Operand output);
Instruction Ashr(Operand input0, Operand input1, Operand output);
Instruction Sex(Operand input0, Operand output);
Instruction Sys(Operand input0);
Instruction Ite(Operand input0, Operand input1, Operand input2, Operand output);

inline Immediate Imm(uint16_t size, uint64_t value) {
  return Immediate(size, value);
}

inline Immediate Imm8(uint64_t value) { return Immediate(8, value); }

inline Immediate Imm16(uint64_t value) { return Immediate(16, value); }

inline Immediate Imm32(uint64_t value) { return Immediate(32, value); }

inline Immediate Imm64(uint64_t value) { return Immediate(64, value); }

inline uint16_t Size(const Operand &operand) {
  switch (operand.index()) {
    case kImmediate:
      return absl::get<Immediate>(operand).size();

    case kRegister:
      return absl::get<Register>(operand).size;

    case kTemporary:
      return absl::get<Temporary>(operand).size;
  }

  return 0;
}

std::ostream &operator<<(std::ostream &stream, Operand operand);
std::ostream &operator<<(std::ostream &stream, Instruction instruction);
std::ostream &operator<<(std::ostream &stream, NativeInstruction instruction);
}  // namespace reil

#define REIL_REIL_H_
#endif  // REIL_REIL_H_
