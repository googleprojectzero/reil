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

#ifndef REIL_AARCH64_TRANSLATOR_H_

#include <cstdint>
#include <tuple>

#include "reil/aarch64/decoder.h"
#include "reil/reil.h"
#include "reil/translation.h"

namespace reil {
namespace aarch64 {
enum TranslationFlags {
  kAdvancedSIMD = kPerArchitectureFlag,

  kDefaultFlags = reil::kDefaultFlags | kAdvancedSIMD
};

enum Registers {
  kX0 = 0,
  kX1,
  kX2,
  kX3,
  kX4,
  kX5,
  kX6,
  kX7,
  kX8,
  kX9,
  kX10,
  kX11,
  kX12,
  kX13,
  kX14,
  kX15,
  kX16,
  kX17,
  kX18,
  kX19,
  kX20,
  kX21,
  kX22,
  kX23,
  kX24,
  kX25,
  kX26,
  kX27,
  kX28,
  kX29,
  kX30,
  kXzr,

  kSp,
  kPc,

  kN,
  kZ,
  kC,
  kV,

  kV0,
  kV1,
  kV2,
  kV3,
  kV4,
  kV5,
  kV6,
  kV7,
  kV8,
  kV9,
  kV10,
  kV11,
  kV12,
  kV13,
  kV14,
  kV15,
  kV16,
  kV17,
  kV18,
  kV19,
  kV20,
  kV21,
  kV22,
  kV23,
  kV24,
  kV25,
  kV26,
  kV27,
  kV28,
  kV29,
  kV30,
  kV31,
};

std::string RegisterName(uint8_t index);
uint8_t RegisterIndex(std::string name);

NativeInstruction TranslateInstruction(const decoder::Instruction& di,
                            uint32_t flags = kDefaultFlags);
NativeInstruction TranslateInstruction(uint64_t address, std::vector<uint8_t> bytes,
                            uint32_t flags = kDefaultFlags);
NativeInstruction TranslateInstruction(uint64_t address, const uint8_t* bytes,
                            size_t bytes_len, uint32_t flags = kDefaultFlags);
}  // namespace aarch64
}  // namespace reil

#define REIL_AARCH64_TRANSLATOR_H_
#endif  // REIL_AARCH64_TRANSLATOR_H_
