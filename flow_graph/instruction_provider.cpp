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

#include "flow_graph/instruction_provider.h"

#include "absl/memory/memory.h"

#include "reil/aarch64.h"

#define GOOGLE_STRIP_LOG 1
#include "glog/logging.h"

namespace reil {
uint64_t InstructionProvider::NextNativeInstruction(uint64_t address) {
  uint64_t next_address = 0;

  if (memory_image_.executable(address)) {
    absl::Span<const uint8_t> bytes = memory_image_.Read(address);
    next_address = NextNativeInstruction(address, bytes);
  }

  return next_address;
}

std::shared_ptr<reil::NativeInstruction> InstructionProvider::NativeInstruction(
    uint64_t address) {
  std::shared_ptr<reil::NativeInstruction> ni = nullptr;

  auto cache_iter = cache_.find(address);
  if (cache_iter != cache_.end()) {
    ni = cache_iter->second.lock();
  }

  if (!ni) {
    if (memory_image_.executable(address)) {
      absl::Span<const uint8_t> bytes = memory_image_.Read(address);
      ni = std::make_shared<reil::NativeInstruction>(std::move(NativeInstruction(address, bytes)));
      cache_[address] = ni;
      cache__[cache__index_++ % cache__.size()] = ni;
    }
  }

  return ni;
}

Node InstructionProvider::NextInstruction(const Node& node) {
  auto ni = NativeInstruction(node.address);
  if ((uint64_t)node.offset + 1 < ni->reil.size()) {
    return Node(node.address, node.offset + 1);
  } else {
    return Node(NextNativeInstruction(node.address));
  }
}

reil::Instruction InstructionProvider::Instruction(const Node& node) {
  auto ni = NativeInstruction(node.address);
  DCHECK(node.offset < ni->reil.size());
  return ni->reil[node.offset];
}

InstructionProvider::InstructionProvider(const MemoryImage& memory_image,
                                   enum TranslationFlags flags)
    : memory_image_(memory_image), flags_(flags) {}

InstructionProvider::~InstructionProvider() {}

class AArch64InstructionProvider : public InstructionProvider {
 protected:
  uint64_t NextNativeInstruction(uint64_t address,
                                 absl::Span<const uint8_t> bytes) override;
  reil::NativeInstruction NativeInstruction(uint64_t address,
                                      absl::Span<const uint8_t> bytes) override;

 public:
  AArch64InstructionProvider(const MemoryImage& memory_image,
                          enum TranslationFlags flags)
      : InstructionProvider(memory_image, flags) {}
};

uint64_t AArch64InstructionProvider::NextNativeInstruction(
    uint64_t address, absl::Span<const uint8_t> bytes) {
  return address + 4;
}

reil::NativeInstruction AArch64InstructionProvider::NativeInstruction(
    uint64_t address, absl::Span<const uint8_t> bytes) {
  return reil::aarch64::TranslateInstruction(address, bytes.data(),
                                             bytes.size(), flags_);
}

std::unique_ptr<InstructionProvider> InstructionProvider::Create(
    const MemoryImage& memory_image, enum TranslationFlags flags) {
  if (memory_image.architecture_name() == "aarch64") {
    return absl::make_unique<AArch64InstructionProvider>(memory_image, flags);
  } else {
    LOG(FATAL) << "Unsupported architecture";
  }
}
}  // namespace reil