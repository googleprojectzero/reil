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

#ifndef REIL_INTERPRETER_H_

#include <map>
#include <vector>

#include "reil.h"

namespace reil {
class Interpreter {
 private:
  std::map<uint64_t, uint8_t> memory_;
  std::map<uint32_t, Immediate> registers_;
  std::map<uint32_t, Immediate> temporaries_;

  std::vector<Instruction> instructions_;
  uint64_t pc_;
  uint16_t offset_;

  Immediate GetOperand(const Operand &op) const;
  void SetOperand(const Operand &op, const Immediate &value);

  void Add(const Instruction &ri);
  void And(const Instruction &ri);
  void Bisz(const Instruction &ri);
  void Bsh(const Instruction &ri);
  void Div(const Instruction &ri);
  void Jcc(const Instruction &ri);
  void Ldm(const Instruction &ri);
  void Mod(const Instruction &ri);
  void Mul(const Instruction &ri);
  void Nop(const Instruction &ri);
  void Or(const Instruction &ri);
  void Stm(const Instruction &ri);
  void Str(const Instruction &ri);
  void Sub(const Instruction &ri);
  void Undef(const Instruction &ri);
  void Unkn(const Instruction &ri);
  void Xor(const Instruction &ri);
  void Equ(const Instruction &ri);
  void Lshl(const Instruction &ri);
  void Lshr(const Instruction &ri);
  void Ashr(const Instruction &ri);
  void Sex(const Instruction &ri);
  void Sys(const Instruction &ri);
  void Ite(const Instruction &ri);

 public:
  Interpreter();
  ~Interpreter();

  void Start(NativeInstruction ni);
  uint16_t SingleStep();
  uint64_t Execute(NativeInstruction ni);

  Immediate GetRegister(uint32_t index) const;
  void SetRegister(uint32_t index, const Immediate &value);

  std::vector<uint8_t> GetMemory(uint64_t address, size_t size);
  void SetMemory(uint64_t address, const std::vector<uint8_t> &bytes);
  void SetMemory(uint64_t address, const uint8_t *bytes, size_t bytes_len);
};
}  // namespace reil

#define REIL_INTERPRETER_H_
#endif  // REIL_INTERPRETER_H_
