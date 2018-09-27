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

#include "disassembler/aarch64/disassembler.h"

#include "reil/aarch64/decoder.h"

#define GOOGLE_STRIP_LOG 1
#include "glog/logging.h"

namespace reil {
namespace disassembler {
namespace aarch64 {
std::set<uint64_t> FindFunctions(const MemoryImage& memory_image) {
  std::set<uint64_t> results;

  for (auto& mapping : memory_image.mappings()) {
    if (mapping.executable) {
      uint64_t start_address = 0;
      bool pacsp = false, sub = false, stp = false;

      for (size_t offset = 0; offset + 4 < mapping.data.size(); offset += 4) {
        uint64_t address = mapping.address + offset;
        uint32_t opcode;

        memcpy(&opcode, &mapping.data[offset], sizeof(opcode));

        VLOG(3) << std::hex << address << ": " << opcode << " "
                << reil::aarch64::decoder::DecodeInstruction(address, opcode);

        if (!pacsp && (opcode & 0b11111111111111111111111110111111) ==
                          0b11010101000000110010001100111111) {
          // paci[a|b]sp
          DCHECK(!sub && !stp);
          start_address = address;
          pacsp = true;
          sub = false;
          stp = false;
        } else if (!sub && (opcode & 0b11111111000000000000001111111111) ==
                               0b11010001000000000000001111111111) {
          // sub sp, sp, #value
          if (!pacsp) {
            start_address = address;
          }
          sub = true;
          stp = false;
        } else if ((opcode & 0b11111111010000000000001111100000) ==
                   0b10101001000000000000001111100000) {
          // stp x?, x?, [sp, #offset] [!]
          if (!pacsp && !sub && !stp) {
            start_address = address;
          }
          stp = true;
        } else if ((opcode & 0b11111111010000000000001111100000) ==
                   0b01101101000000000000001111100000) {
          // stp d?, d?, [sp, #offset] [!]
          if (!sub && !stp) {
            start_address = address;
          }
          stp = true;
        } else if (stp && (opcode & 0b11111111000000000000001111111111) ==
                              0b10010001000000000000001111111101) {
          // add x29, sp, #value
          VLOG(2) << "function_start: " << std::hex << start_address;
          pacsp = false;
          sub = false;
          stp = false;
          results.insert(start_address);
        } else if (pacsp || sub || stp) {
          // this isn't a match, clear state
          VLOG(1) << "match_failed: " << std::hex << start_address;
          start_address = 0;
          pacsp = false;
          sub = false;
          stp = false;
        }
      }
    }
  }

  return results;
}
}  // namespace aarch64
}  // namespace disassembler
}  // namespace reil
