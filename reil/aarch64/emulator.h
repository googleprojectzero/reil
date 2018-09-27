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

#ifndef REIL_AARCH64_EMULATOR_H_

#include "../emulator.h"
#include "../interpreter.h"
#include "translator.h"

#include "absl/types/span.h"

namespace reil {
namespace aarch64 {
class Emulator : public reil::Emulator {
 private:
  uint32_t flags_;
  reil::Interpreter interpreter_;

 public:
  explicit Emulator(uint32_t flags = kDefaultFlags);
  ~Emulator();

  uint32_t flags() const { return flags_; }

  void Run() override;
  bool SingleStep() override;
  Immediate GetRegister(uint32_t index) const override;
  void SetRegister(uint32_t index, Immediate value) override;
  std::vector<uint8_t> GetMemory(uint64_t address, size_t size) override;
  void SetMemory(uint64_t address, const absl::Span<uint8_t> &bytes) override;
  void SetMemory(uint64_t address, uint8_t *bytes, size_t bytes_len) override;
};

std::ostream &operator<<(std::ostream &stream, const Emulator &emu);
}  // namespace aarch64
}  // namespace reil

#define REIL_AARCH64_EMULATOR_H_
#endif  // REIL_AARCH64_EMULATOR_H_
