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

#ifndef REIL_MEMORY_IMAGE_MEMORY_IMAGE_H_

#include <memory>
#include <set>
#include <vector>

namespace reil {
struct Mapping {
  uint64_t address;
  std::vector<uint8_t> data;
  bool readable;
  bool writable;
  bool executable;
};

class MemoryImage {
  std::vector<Mapping> mappings_;
  std::string architecture_name_;

 public:
  MemoryImage(std::string architecture_name);

  std::string architecture_name() const { return architecture_name_; }

  bool readable(uint64_t address, uint64_t size = 0);
  bool writable(uint64_t address, uint64_t size = 0);
  bool executable(uint64_t address, uint64_t size = 0);

  bool AccessOk(uint64_t address, uint64_t size, bool read, bool write,
                bool execute);
  std::pair<const uint8_t*, uint64_t> Read(uint64_t address);
  std::vector<uint8_t> Read(uint64_t address, uint64_t size);

  const std::vector<Mapping>& mappings() const;

  static std::unique_ptr<MemoryImage> Load(std::string path);
};
}  // namespace reil

#define REIL_MEMORY_IMAGE_MEMORY_IMAGE_H_
#endif  // REIL_MEMORY_IMAGE_MEMORY_IMAGE_H_
