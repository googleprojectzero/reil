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

#include "reil/interpreter.h"

#include <cassert>
#include <stdexcept>

namespace reil {

Interpreter::Interpreter() {}

Interpreter::~Interpreter() {}

Immediate Interpreter::GetOperand(const Operand &op) const {
  if (absl::holds_alternative<Immediate>(op)) {
    return absl::get<Immediate>(op);
  } else if (absl::holds_alternative<Register>(op)) {
    return registers_.at(absl::get<Register>(op).index);
  } else if (absl::holds_alternative<Temporary>(op)) {
    return temporaries_.at(absl::get<Temporary>(op).index);
  } else {
    // unreachable
    abort();
  }
}

void Interpreter::SetOperand(const Operand &op, const Immediate &value) {
  std::cerr << op << " = " << value << std::endl;
  if (absl::holds_alternative<Register>(op)) {
    registers_[absl::get<Register>(op).index] = value;
  } else if (absl::holds_alternative<Temporary>(op)) {
    temporaries_[absl::get<Temporary>(op).index] = value;
  } else {
    // unreachable
    abort();
  }
}

void Interpreter::Add(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);
  SetOperand(ri.output, a + b);
}

void Interpreter::And(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);
  SetOperand(ri.output, a & b);
}

void Interpreter::Bisz(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  SetOperand(ri.output, Immediate(Size(ri.output), a ? 0 : 1));
}

void Interpreter::Bsh(const Instruction &ri) {}

void Interpreter::Div(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);
  SetOperand(ri.output, a / b);
}

void Interpreter::Jcc(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  if (a) {
    if (ri.output.index() == kOffset) {
      Offset target = absl::get<Offset>(ri.output);
      offset_ = target.offset;
    } else {
      Immediate target = GetOperand(ri.output);
      pc_ = static_cast<uint64_t>(target);
      offset_ = 0xffff;
    }
  }
}

void Interpreter::Ldm(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  uint64_t address = static_cast<uint64_t>(a);
  std::vector<uint8_t> bytes = GetMemory(address, Size(ri.output) / 8);
  SetOperand(ri.output, Immediate(absl::Span<uint8_t>(bytes)));
}

void Interpreter::Mod(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);
  SetOperand(ri.output, a % b);
}

void Interpreter::Mul(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);
  SetOperand(ri.output, a * b);
}

void Interpreter::Nop(const Instruction &ri) {}

void Interpreter::Or(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);
  SetOperand(ri.output, a | b);
}

void Interpreter::Stm(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.output);
  uint64_t address = static_cast<uint64_t>(b);
  SetMemory(address, a.bytes());
}

void Interpreter::Str(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  uint16_t result_size = Size(ri.output);
  if (result_size < a.size()) {
    SetOperand(ri.output, a.Extract(result_size));
  } else if (a.size() < result_size) {
    SetOperand(ri.output, a.ZeroExtend(Size(ri.output)));
  } else {
    SetOperand(ri.output, a);
  }
}

void Interpreter::Sub(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);
  SetOperand(ri.output, a - b);
}

void Interpreter::Undef(const Instruction &ri) {
  if (absl::holds_alternative<Register>(ri.output)) {
    // TODO:
  } else if (absl::holds_alternative<Temporary>(ri.output)) {
    // TODO:
  }
}

void Interpreter::Unkn(const Instruction &ri) {}

void Interpreter::Xor(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);
  SetOperand(ri.output, a ^ b);
}

void Interpreter::Equ(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);
  SetOperand(ri.output, Immediate(Size(ri.output), a == b ? 1 : 0));
}

void Interpreter::Lshl(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);
  SetOperand(ri.output, a << b);
}

void Interpreter::Lshr(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);
  SetOperand(ri.output, a >> b);
}

void Interpreter::Ashr(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  Immediate b = GetOperand(ri.input1);

  uint16_t size = a.size();

  if (Immediate::SignBit(size, size) & a) {
    a >>= b;
    a |= Immediate::Mask(size, size) << (size - (uint64_t)b);
  } else {
    a >>= b;
  }

  SetOperand(ri.output, a);
}

void Interpreter::Sex(const Instruction &ri) {
  Immediate a = GetOperand(ri.input0);
  uint16_t result_size = Size(ri.output);
  if (result_size < a.size()) {
    SetOperand(ri.output, a.Extract(result_size));
  } else if (a.size() < result_size) {
    SetOperand(ri.output, a.SignExtend(Size(ri.output)));
  } else {
    SetOperand(ri.output, a);
  }
}

void Interpreter::Sys(const Instruction &ri) {}

void Interpreter::Ite(const Instruction &ri) {
  Immediate cond = GetOperand(ri.input0);
  Immediate if_value = GetOperand(ri.input1);
  Immediate else_value = GetOperand(ri.input2);

  if (cond) {
    SetOperand(ri.output, if_value);
  } else {
    SetOperand(ri.output, else_value);
  }
}

void Interpreter::Start(NativeInstruction ni) {
  assert(ni.reil.size() < 0xffff);

  instructions_ = ni.reil;
  offset_ = 0;
  pc_ = ni.address;
  temporaries_.clear();
}

uint16_t Interpreter::SingleStep() {
  const Instruction &ri = instructions_[offset_++];
  std::cerr << ri << std::endl;

  switch (ri.opcode) {
    case Opcode::Add: {
      Add(ri);
    } break;

    case Opcode::And: {
      And(ri);
    } break;

    case Opcode::Bisz: {
      Bisz(ri);
    } break;

    case Opcode::Bsh: {
      Bsh(ri);
    } break;

    case Opcode::Div: {
      Div(ri);
    } break;

    case Opcode::Jcc: {
      Jcc(ri);
    } break;

    case Opcode::Ldm: {
      Ldm(ri);
    } break;

    case Opcode::Mod: {
      Mod(ri);
    } break;

    case Opcode::Mul: {
      Mul(ri);
    } break;

    case Opcode::Nop: {
      Nop(ri);
    } break;

    case Opcode::Or: {
      Or(ri);
    } break;

    case Opcode::Stm: {
      Stm(ri);
    } break;

    case Opcode::Str: {
      Str(ri);
    } break;

    case Opcode::Sub: {
      Sub(ri);
    } break;

    case Opcode::Undef: {
      Undef(ri);
    } break;

    case Opcode::Unkn: {
      Unkn(ri);
    } break;

    case Opcode::Xor: {
      Xor(ri);
    } break;

    case Opcode::Equ: {
      Equ(ri);
    } break;

    case Opcode::Lshl: {
      Lshl(ri);
    } break;

    case Opcode::Lshr: {
      Lshr(ri);
    } break;

    case Opcode::Ashr: {
      Ashr(ri);
    } break;

    case Opcode::Sex: {
      Sex(ri);
    } break;

    case Opcode::Sys: {
      Sys(ri);
    } break;

    case Opcode::Ite: {
      Ite(ri);
    }
  }

  return offset_;
}

uint64_t Interpreter::Execute(NativeInstruction ni) {
  Start(ni);
  while (offset_ < instructions_.size()) {
    SingleStep();
  }
  if (offset_ != 0xffff) {
    pc_ = ni.address + ni.size;
  }
  return pc_;
}

Immediate Interpreter::GetRegister(uint32_t index) const {
  return registers_.at(index);
}

void Interpreter::SetRegister(uint32_t index, const Immediate &value) {
  registers_[index] = value;
}

std::vector<uint8_t> Interpreter::GetMemory(uint64_t address, size_t size) {
  std::vector<uint8_t> bytes;
  for (size_t i = 0; i < size; ++i) {
    bytes.push_back(memory_[address + i]);
  }
  return bytes;
}

void Interpreter::SetMemory(uint64_t address,
                            const absl::Span<uint8_t> &bytes) {
  SetMemory(address, bytes.data(), bytes.size());
}

void Interpreter::SetMemory(uint64_t address, const uint8_t *bytes,
                            size_t bytes_len) {
  for (size_t i = 0; i < bytes_len; ++i) {
    memory_[address + i] = bytes[i];
  }
}
}  // namespace reil
