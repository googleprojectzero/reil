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

#include "absl/types/variant.h"

#include "reil/immediate.h"

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

  Offset(uint16_t offset);
};

struct Register {
  uint16_t size;
  uint8_t index;
  std::string name;

  Register(uint16_t size, uint8_t index, const std::string& name);
};

struct Temporary {
  uint16_t size;
  uint16_t index;

  Temporary(uint16_t size, uint16_t index);
};

struct Label {
  uint8_t index;

  Label(uint8_t index);
};

typedef absl::variant<int, Immediate, Offset, Register, Temporary, Label>
    Operand;

// hints for the jcc instruction
extern const Immediate kJump;
extern const Immediate kCall;
extern const Immediate kReturn;

struct Instruction {
  Opcode opcode;
  Operand input0;
  Operand input1;
  Operand input2;
  Operand output;
};

struct NativeInstruction {
  uint64_t address = 0;
  uint8_t size = 0;
  std::string mnemonic = "";
  std::vector<Instruction> reil;
};

Instruction Add(const Operand& input0, const Operand& input1,
                const Operand& output);
Instruction And(const Operand& input0, const Operand& input1,
                const Operand& output);
Instruction Bisz(const Operand& input0, const Operand& output);
Instruction Bsh(const Operand& input0, const Operand& input1,
                const Operand& output);
Instruction Div(const Operand& input0, const Operand& input1,
                const Operand& output);
Instruction Jcc(const Operand& input0, const Operand& output);
Instruction JccHint(const Operand& input0, const Operand& input1,
                    const Operand& output);
Instruction Ldm(const Operand& input0, const Operand& output);
Instruction Mod(const Operand& input0, const Operand& input1,
                const Operand& output);
Instruction Mul(const Operand& input0, const Operand& input1,
                const Operand& output);
Instruction Nop();
Instruction Nop(const Operand& input0);
Instruction Or(const Operand& input0, const Operand& input1,
               const Operand& output);
Instruction Stm(const Operand& input0, const Operand& output);
Instruction Str(const Operand& input0, const Operand& output);
Instruction Sub(const Operand& input0, const Operand& input1,
                const Operand& output);
Instruction Undef(const Operand& output);
Instruction Unkn();
Instruction Xor(const Operand& input0, const Operand& input1,
                const Operand& output);
Instruction Equ(const Operand& input0, const Operand& input1,
                const Operand& output);
Instruction Lshl(const Operand& input0, const Operand& input1,
                 const Operand& output);
Instruction Lshr(const Operand& input0, const Operand& input1,
                 const Operand& output);
Instruction Ashr(const Operand& input0, const Operand& input1,
                 const Operand& output);
Instruction Sex(const Operand& input0, const Operand& output);
Instruction Sys(const Operand& input0);
Instruction Ite(const Operand& input0, const Operand& input1,
                const Operand& input2, const Operand& output);

inline Immediate Imm(uint16_t size, uint64_t value) {
  return Immediate(size, value);
}

inline Immediate Imm8(uint64_t value) { return Immediate(8, value); }

inline Immediate Imm16(uint64_t value) { return Immediate(16, value); }

inline Immediate Imm32(uint64_t value) { return Immediate(32, value); }

inline Immediate Imm64(uint64_t value) { return Immediate(64, value); }

inline uint16_t Size(const Operand& operand) {
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

std::ostream& operator<<(std::ostream& stream, const Operand& opnd);
std::ostream& operator<<(std::ostream& stream, const Instruction& ri);
std::ostream& operator<<(std::ostream& stream, const NativeInstruction& ni);
}  // namespace reil

#define REIL_REIL_H_
#endif  // REIL_REIL_H_
