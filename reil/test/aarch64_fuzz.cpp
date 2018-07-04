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

#include <reil/aarch64.h>

#include <iostream>
#include <vector>

#include <cstdint>

int main(int argc, char **argv) {
  std::vector<uint8_t> bytes({0, 0, 0, 0, 0, 0, 0, 0});
  uint64_t x0 = 0, x1 = 0, x2 = 0, x3 = 0;

  std::cin.read((char *)bytes.data(), bytes.size());

  std::cin >> x0;
  std::cin >> x1;
  std::cin >> x2;
  std::cin >> x3;

  reil::aarch64::Emulator reil_emu(reil::kDefaultFlags);
  reil_emu.SetMemory(0x1000, bytes);
  reil_emu.SetRegister(reil::aarch64::kX0, reil::Imm64(x0));
  reil_emu.SetRegister(reil::aarch64::kX1, reil::Imm64(x1));
  reil_emu.SetRegister(reil::aarch64::kX2, reil::Imm64(x2));
  reil_emu.SetRegister(reil::aarch64::kX3, reil::Imm64(x3));
  reil_emu.SetRegister(reil::aarch64::kPc, reil::Imm64(0x1000));

  reil_emu.SingleStep();
  reil_emu.SingleStep();
  reil_emu.SingleStep();
  reil_emu.SingleStep();
}
