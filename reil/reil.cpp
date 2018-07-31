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

#include "reil.h"

#include <iomanip>

namespace reil {
Offset::Offset(uint16_t offset) : offset(offset) {}

Register::Register(uint16_t size, uint8_t index, const std::string& name)
    : size(size), index(index), name(name) {}

Temporary::Temporary(uint16_t size, uint16_t index)
    : size(size), index(index) {}

Label::Label(uint8_t index) : index(index) {}

const Immediate kJump = Imm8(0);
const Immediate kCall = Imm8(1);
const Immediate kReturn = Imm8(2);

Instruction Add(const Operand& input0, const Operand& input1,
                const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Add;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction And(const Operand& input0, const Operand& input1,
                const Operand& output) {
  Instruction i;
  i.opcode = Opcode::And;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Bisz(const Operand& input0, const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Bisz;
  i.input0 = input0;
  i.output = output;
  return i;
}

Instruction Bsh(const Operand& input0, const Operand& input1,
                const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Bsh;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Div(const Operand& input0, const Operand& input1,
                const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Div;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Jcc(const Operand& input0, const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Jcc;
  i.input0 = input0;
  i.output = output;
  return i;
}

Instruction JccHint(const Operand& input0, const Operand& input1,
                    const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Jcc;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Ldm(const Operand& input0, const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Ldm;
  i.input0 = input0;
  i.output = output;
  return i;
}

Instruction Mod(const Operand& input0, const Operand& input1,
                const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Mod;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Mul(const Operand& input0, const Operand& input1,
                const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Mul;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Nop() {
  Instruction i;
  i.opcode = Opcode::Nop;
  return i;
}

Instruction Nop(const Operand& input0) {
  Instruction i;
  i.opcode = Opcode::Nop;
  i.input0 = input0;
  return i;
}

Instruction Or(const Operand& input0, const Operand& input1,
               const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Or;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Stm(const Operand& input0, const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Stm;
  i.input0 = input0;
  i.output = output;
  return i;
}

Instruction Str(const Operand& input0, const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Str;
  i.input0 = input0;
  i.output = output;
  return i;
}

Instruction Sub(const Operand& input0, const Operand& input1,
                const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Sub;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Undef(const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Undef;
  i.output = output;
  return i;
}

Instruction Unkn() {
  Instruction i;
  i.opcode = Opcode::Unkn;
  return i;
}

Instruction Xor(const Operand& input0, const Operand& input1,
                const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Xor;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Bisnz(const Operand& input0, const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Bisnz;
  i.input0 = input0;
  i.output = output;
  return i;
}

Instruction Equ(const Operand& input0, const Operand& input1,
                const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Equ;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Lshl(const Operand& input0, const Operand& input1,
                 const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Lshl;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Lshr(const Operand& input0, const Operand& input1,
                 const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Lshr;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Ashr(const Operand& input0, const Operand& input1,
                 const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Ashr;
  i.input0 = input0;
  i.input1 = input1;
  i.output = output;
  return i;
}

Instruction Sex(const Operand& input0, const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Sex;
  i.input0 = input0;
  i.output = output;
  return i;
}

Instruction Sys(const Operand& input0) {
  Instruction i;
  i.opcode = Opcode::Sys;
  i.input0 = input0;
  return i;
}

Instruction Ite(const Operand& input0, const Operand& input1,
                const Operand& input2, const Operand& output) {
  Instruction i;
  i.opcode = Opcode::Ite;
  i.input0 = input0;
  i.input1 = input1;
  i.input2 = input2;
  i.output = output;
  return i;
}

template <typename T>
static std::ostream& hex(std::ostream& stream, T value, size_t width = 0) {
  auto old_flags = stream.flags();
  auto old_precision = stream.precision();
  auto old_fill = stream.fill();

  stream << std::nouppercase << std::internal << std::setfill('0') << std::hex;
  if (width)
    stream << std::setw(width / 4);
  else
    stream << std::setw(sizeof(T) * 2);
  stream << value;

  stream.flags(old_flags);
  stream.precision(old_precision);
  stream.fill(old_fill);

  return stream;
}

std::ostream& operator<<(std::ostream& stream, const Operand& opnd) {
  switch (opnd.index()) {
    case kImmediate: {
      Immediate imm = absl::get<Immediate>(opnd);
      stream << imm;
    } break;

    case kOffset: {
      Offset off = absl::get<Offset>(opnd);
      stream << ".";
      hex(stream, off.offset, 4);
    } break;

    case kRegister: {
      Register reg = absl::get<Register>(opnd);
      stream << "(" << reg.name << ", " << reg.size << ")";
    } break;

    case kTemporary: {
      Temporary tmp = absl::get<Temporary>(opnd);
      stream << "(t" << tmp.index << ", " << tmp.size << ")";
    } break;

    case kLabel: {
      Label lbl = absl::get<Label>(opnd);
      stream << "label_" << (int)lbl.index;
    } break;
  }
  return stream;
}

std::ostream& operator<<(std::ostream& stream, const Instruction& ri) {
  switch (ri.opcode) {
    case Opcode::Add: {
      stream << "add   ";
    } break;

    case Opcode::And: {
      stream << "and   ";
    } break;

    case Opcode::Bisz: {
      stream << "bisz  ";
    } break;

    case Opcode::Bsh: {
      stream << "bsh   ";
    } break;

    case Opcode::Div: {
      stream << "div   ";
    } break;

    case Opcode::Jcc: {
      stream << "jcc   ";
    } break;

    case Opcode::Ldm: {
      stream << "ldm   ";
    } break;

    case Opcode::Mod: {
      stream << "mod   ";
    } break;

    case Opcode::Mul: {
      stream << "mul   ";
    } break;

    case Opcode::Nop: {
      stream << "nop   ";
    } break;

    case Opcode::Or: {
      stream << "or    ";
    } break;

    case Opcode::Stm: {
      stream << "stm   ";
    } break;

    case Opcode::Str: {
      stream << "str   ";
    } break;

    case Opcode::Sub: {
      stream << "sub   ";
    } break;

    case Opcode::Undef: {
      stream << "undef ";
    } break;

    case Opcode::Unkn: {
      stream << "unkn  ";
    } break;

    case Opcode::Xor: {
      stream << "xor   ";
    } break;

    case Opcode::Bisnz: {
      stream << "bisnz ";
    } break;

    case Opcode::Equ: {
      stream << "equ   ";
    } break;

    case Opcode::Lshl: {
      stream << "lshl  ";
    } break;

    case Opcode::Lshr: {
      stream << "lshr  ";
    } break;

    case Opcode::Ashr: {
      stream << "ashr  ";
    } break;

    case Opcode::Sex: {
      stream << "sex   ";
    } break;

    case Opcode::Sys: {
      stream << "sys   ";
    } break;

    case Opcode::Ite: {
      stream << "ite   ";
    } break;
  }

  if (ri.input0.index() != kNone) {
    stream << ri.input0;
  }

  if (ri.input1.index() != kNone) {
    if (ri.input0.index() != kNone) {
      stream << ", ";
    }
    stream << ri.input1;
  }

  if (ri.input2.index() != kNone) {
    if (ri.input1.index() != kNone) {
      stream << ", ";
    }
    stream << ri.input2;
  }

  if (ri.output.index() != kNone) {
    if (ri.input0.index() != kNone) {
      stream << ", ";
    }
    stream << ri.output;
  }

  return stream;
}

std::ostream& operator<<(std::ostream& stream, const NativeInstruction& ni) {
  hex(stream, ni.address) << " " << ni.mnemonic;  // << std::endl;
  // for (uint16_t i = 0; i < ni.reil.size(); ++i) {
  //  stream << ".";
  //  hex(stream, i);
  //  stream << " " << ni.reil[i] << std::endl;
  //}
  return stream;
}
}  // namespace reil
