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

#include <fstream>

#include "memory_image/memory_image.h"
#include "memory_image/memory_image.pb.h"

#include "absl/memory/memory.h"
#include "glog/logging.h"

namespace reil {
MemoryImage::MemoryImage(std::string architecture_name)
    : architecture_name_(architecture_name) {}

bool MemoryImage::readable(uint64_t address, uint64_t size) {
  return AccessOk(address, size, true, false, false);
}

bool MemoryImage::writable(uint64_t address, uint64_t size) {
  return AccessOk(address, size, false, true, false);
}

bool MemoryImage::executable(uint64_t address, uint64_t size) {
  return AccessOk(address, size, false, false, true);
}

bool MemoryImage::AccessOk(uint64_t address, uint64_t size, bool read,
                           bool write, bool execute) {
  bool found = false;
  bool readable = read;
  bool writable = write;
  bool executable = execute;
  for (const auto& mapping : mappings()) {
    if (mapping.address <= address &&
        address + size <= mapping.address + mapping.data.size()) {
      found = true;
      readable &= mapping.readable;
      writable &= mapping.writable;
      executable &= mapping.executable;
    }
  }

  if (found) {
    return read == readable && write == writable && execute == executable;
  }

  return false;
}

std::pair<const uint8_t*, uint64_t> MemoryImage::Read(uint64_t address) {
  std::pair<const uint8_t*, uint64_t> result;
  result.first = nullptr;
  result.second = 0;
  for (auto& mapping : mappings_) {
    if (mapping.address <= address) {
      uint64_t offset = address - mapping.address;
      if (offset <= mapping.data.size()) {
        result.first = (const uint8_t*)(&mapping.data[offset]);
        result.second = mapping.data.size() - offset;
      }
    }
  }
  return result;
}

std::vector<uint8_t> MemoryImage::Read(uint64_t address, uint64_t size) {
  std::vector<uint8_t> bytes;
  // TODO: implement
  return bytes;
}

const std::vector<Mapping>& MemoryImage::mappings() const { return mappings_; }

std::unique_ptr<MemoryImage> MemoryImage::Load(std::string path) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::unique_ptr<MemoryImage> memory_image = nullptr;
  proto::MemoryImage proto_memory_image;

  std::fstream input(path, std::ios::in | std::ios::binary);
  if (input && proto_memory_image.ParseFromIstream(&input)) {
    memory_image = absl::make_unique<MemoryImage>(
        proto_memory_image.architecture_name().data());
    for (int i = 0; i < proto_memory_image.mappings_size(); ++i) {
      const proto::MemoryImage::Mapping& proto_mapping =
          proto_memory_image.mappings(i);

      Mapping mapping({
          proto_mapping.address(),
          std::vector<uint8_t>(proto_mapping.data().begin(),
                               proto_mapping.data().end()),
          proto_mapping.readable(),
          proto_mapping.writable(),
          proto_mapping.executable(),
      });

      memory_image->mappings_.push_back(mapping);
    }
  }
  return memory_image;
}
}  // namespace reil
