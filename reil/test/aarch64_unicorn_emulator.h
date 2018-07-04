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

#ifndef REIL_TEST_AARCH64_UNICORN_EMULATOR_H_

#include <unicorn/arm64.h>
#include <unicorn/unicorn.h>

#include <reil/aarch64.h>
#include <reil/reil.h>

namespace reil {
namespace test {
class AArch64UnicornEmulator {
 private:
  uc_engine *unicorn_;

  Immediate GetCoreRegister(uint32_t index) const;
  Immediate GetFlagRegister(uint32_t index) const;
  Immediate GetSIMDRegister(uint32_t index) const;

  void SetCoreRegister(uint32_t index, uint64_t value);
  void SetFlagRegister(uint32_t index, uint8_t value);
  void SetSIMDRegister(uint32_t index, Immediate value);

 public:
  AArch64UnicornEmulator();
  ~AArch64UnicornEmulator();

  bool SingleStep();

  Immediate GetRegister(uint32_t index) const;
  void SetRegister(uint32_t index, Immediate value);

  std::vector<uint8_t> GetMemory(uint64_t address, size_t size);
  void SetMemory(uint64_t address, const std::vector<uint8_t> &bytes);
  void SetMemory(uint64_t address, const uint8_t *bytes, size_t bytes_len);
};

std::ostream &operator<<(std::ostream &stream,
                         const AArch64UnicornEmulator &emu);
}  // namespace test
}  // namespace reil

#define REIL_TEST_AARCH64_UNICORN_EMULATOR_H_
#endif  // REIL_TEST_AARCH64_UNICORN_EMULATOR_H_
