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

#ifndef REIL_FLOW_GRAPH_INSTRUCTION_PROVIDER_H_

#include <array>
#include <map>
#include <memory>

#include "absl/types/span.h"

#include "flow_graph/node.h"
#include "memory_image/memory_image.h"
#include "reil/reil.h"
#include "reil/translation.h"

namespace reil {
class InstructionProvider {
  const MemoryImage& memory_image_;

  std::map<uint64_t, std::weak_ptr<reil::NativeInstruction>> cache_;
  std::array<std::shared_ptr<reil::NativeInstruction>, 0x100> cache__;
  int cache__index_ = 0;

 protected:
  enum TranslationFlags flags_;

  virtual reil::NativeInstruction NativeInstruction(
      uint64_t address, absl::Span<const uint8_t> bytes) = 0;
  virtual uint64_t NextNativeInstruction(uint64_t address,
                                         absl::Span<const uint8_t> bytes) = 0;

  InstructionProvider(const MemoryImage& memory_image,
                      enum TranslationFlags flags);

 public:
  InstructionProvider(const InstructionProvider&) = delete;
  InstructionProvider(InstructionProvider&&) = delete;

  virtual ~InstructionProvider();

  uint64_t NextNativeInstruction(uint64_t address);
  std::shared_ptr<reil::NativeInstruction> NativeInstruction(uint64_t address);

  Node NextInstruction(const Node& node);
  reil::Instruction Instruction(const Node& address);

  static std::unique_ptr<InstructionProvider> Create(
      const MemoryImage& memory_image,
      enum TranslationFlags flags = kDefaultFlags);
};
}  // namespace reil

#define REIL_FLOW_GRAPH_INSTRUCTION_PROVIDER_H_
#endif  // REIL_FLOW_GRAPH_INSTRUCTION_PROVIDER_H_