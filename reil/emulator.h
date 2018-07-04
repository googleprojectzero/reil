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

#ifndef REIL_EMULATOR_H_

#include <vector>

#include "reil.h"

namespace reil {
class Emulator {
 public:
  Emulator();
  virtual ~Emulator();

  virtual void Run() = 0;
  virtual bool SingleStep() = 0;
  virtual Immediate GetRegister(uint32_t index) const = 0;
  virtual void SetRegister(uint32_t index, Immediate value) = 0;
  virtual std::vector<uint8_t> GetMemory(uint64_t address, size_t size) = 0;
  virtual void SetMemory(uint64_t address,
                         const std::vector<uint8_t>& bytes) = 0;
  virtual void SetMemory(uint64_t address, uint8_t* bytes,
                         size_t bytes_len) = 0;
};
}  // namespace reil

#define REIL_EMULATOR_H_
#endif  // REIL_EMULATOR_H_
