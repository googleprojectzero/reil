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

#include "emulator.h"

namespace reil {
namespace aarch64 {
Emulator::Emulator(uint32_t flags) : flags_(flags) {
  interpreter_.SetRegister(kX0, Immediate(64, 0));
  interpreter_.SetRegister(kX1, Immediate(64, 0));
  interpreter_.SetRegister(kX2, Immediate(64, 0));
  interpreter_.SetRegister(kX3, Immediate(64, 0));
  interpreter_.SetRegister(kX4, Immediate(64, 0));
  interpreter_.SetRegister(kX5, Immediate(64, 0));
  interpreter_.SetRegister(kX6, Immediate(64, 0));
  interpreter_.SetRegister(kX7, Immediate(64, 0));
  interpreter_.SetRegister(kX8, Immediate(64, 0));
  interpreter_.SetRegister(kX9, Immediate(64, 0));
  interpreter_.SetRegister(kX10, Immediate(64, 0));
  interpreter_.SetRegister(kX11, Immediate(64, 0));
  interpreter_.SetRegister(kX12, Immediate(64, 0));
  interpreter_.SetRegister(kX13, Immediate(64, 0));
  interpreter_.SetRegister(kX14, Immediate(64, 0));
  interpreter_.SetRegister(kX15, Immediate(64, 0));
  interpreter_.SetRegister(kX16, Immediate(64, 0));
  interpreter_.SetRegister(kX17, Immediate(64, 0));
  interpreter_.SetRegister(kX18, Immediate(64, 0));
  interpreter_.SetRegister(kX19, Immediate(64, 0));
  interpreter_.SetRegister(kX20, Immediate(64, 0));
  interpreter_.SetRegister(kX21, Immediate(64, 0));
  interpreter_.SetRegister(kX22, Immediate(64, 0));
  interpreter_.SetRegister(kX23, Immediate(64, 0));
  interpreter_.SetRegister(kX24, Immediate(64, 0));
  interpreter_.SetRegister(kX25, Immediate(64, 0));
  interpreter_.SetRegister(kX26, Immediate(64, 0));
  interpreter_.SetRegister(kX27, Immediate(64, 0));
  interpreter_.SetRegister(kX28, Immediate(64, 0));
  interpreter_.SetRegister(kX29, Immediate(64, 0));
  interpreter_.SetRegister(kX30, Immediate(64, 0));

  interpreter_.SetRegister(kSp, Immediate(64, 0));
  interpreter_.SetRegister(kPc, Immediate(64, 0));

  interpreter_.SetRegister(kN, Immediate(8, 0));
  interpreter_.SetRegister(kZ, Immediate(8, 0));
  interpreter_.SetRegister(kC, Immediate(8, 0));
  interpreter_.SetRegister(kV, Immediate(8, 0));

  if (flags & kAdvancedSIMD) {
    interpreter_.SetRegister(kV0, Immediate(128, 0));
    interpreter_.SetRegister(kV1, Immediate(128, 0));
    interpreter_.SetRegister(kV2, Immediate(128, 0));
    interpreter_.SetRegister(kV3, Immediate(128, 0));
    interpreter_.SetRegister(kV4, Immediate(128, 0));
    interpreter_.SetRegister(kV5, Immediate(128, 0));
    interpreter_.SetRegister(kV6, Immediate(128, 0));
    interpreter_.SetRegister(kV7, Immediate(128, 0));
    interpreter_.SetRegister(kV8, Immediate(128, 0));
    interpreter_.SetRegister(kV9, Immediate(128, 0));
    interpreter_.SetRegister(kV10, Immediate(128, 0));
    interpreter_.SetRegister(kV11, Immediate(128, 0));
    interpreter_.SetRegister(kV12, Immediate(128, 0));
    interpreter_.SetRegister(kV13, Immediate(128, 0));
    interpreter_.SetRegister(kV14, Immediate(128, 0));
    interpreter_.SetRegister(kV15, Immediate(128, 0));
    interpreter_.SetRegister(kV16, Immediate(128, 0));
    interpreter_.SetRegister(kV17, Immediate(128, 0));
    interpreter_.SetRegister(kV18, Immediate(128, 0));
    interpreter_.SetRegister(kV19, Immediate(128, 0));
    interpreter_.SetRegister(kV20, Immediate(128, 0));
    interpreter_.SetRegister(kV21, Immediate(128, 0));
    interpreter_.SetRegister(kV22, Immediate(128, 0));
    interpreter_.SetRegister(kV23, Immediate(128, 0));
    interpreter_.SetRegister(kV24, Immediate(128, 0));
    interpreter_.SetRegister(kV25, Immediate(128, 0));
    interpreter_.SetRegister(kV26, Immediate(128, 0));
    interpreter_.SetRegister(kV27, Immediate(128, 0));
    interpreter_.SetRegister(kV28, Immediate(128, 0));
    interpreter_.SetRegister(kV29, Immediate(128, 0));
    interpreter_.SetRegister(kV30, Immediate(128, 0));
    interpreter_.SetRegister(kV31, Immediate(128, 0));
  }
}

Emulator::~Emulator() {}

void Emulator::Run() {}

bool Emulator::SingleStep() {
  uint64_t pc = static_cast<uint64_t>(GetRegister(kPc));
  auto bytes = GetMemory(pc, 4);

  if (bytes.size() == 4) {
    NativeInstruction ni = TranslateInstruction(pc, bytes, flags_);
    if (ni.reil.size()) {
      pc = interpreter_.Execute(ni);
      SetRegister(kPc, Immediate(64, pc));

      return true;
    }
  }
  return false;
}

Immediate Emulator::GetRegister(uint32_t index) const {
  return interpreter_.GetRegister(index);
}

void Emulator::SetRegister(uint32_t index, Immediate value) {
  interpreter_.SetRegister(index, value);
}

std::vector<uint8_t> Emulator::GetMemory(uint64_t address, size_t size) {
  return interpreter_.GetMemory(address, size);
}

void Emulator::SetMemory(uint64_t address, const absl::Span<uint8_t> &bytes) {
  interpreter_.SetMemory(address, bytes);
}

void Emulator::SetMemory(uint64_t address, uint8_t *bytes, size_t bytes_len) {
  interpreter_.SetMemory(address, bytes, bytes_len);
}

std::ostream &operator<<(std::ostream &stream, const Emulator &emu) {
  stream << "  x0: " << emu.GetRegister(kX0);
  stream << "  x1: " << emu.GetRegister(kX1);
  stream << "  x2: " << emu.GetRegister(kX2);
  stream << std::endl;

  stream << "  x3: " << emu.GetRegister(kX3);
  stream << "  x4: " << emu.GetRegister(kX4);
  stream << "  x5: " << emu.GetRegister(kX5);
  stream << std::endl;

  stream << "  x6: " << emu.GetRegister(kX6);
  stream << "  x7: " << emu.GetRegister(kX7);
  stream << "  x8: " << emu.GetRegister(kX8);
  stream << std::endl;

  stream << "  x9: " << emu.GetRegister(kX9);
  stream << " x10: " << emu.GetRegister(kX10);
  stream << " x11: " << emu.GetRegister(kX11);
  stream << std::endl;

  stream << " x12: " << emu.GetRegister(kX12);
  stream << " x13: " << emu.GetRegister(kX13);
  stream << " x14: " << emu.GetRegister(kX14);
  stream << std::endl;

  stream << " x15: " << emu.GetRegister(kX15);
  stream << " x16: " << emu.GetRegister(kX16);
  stream << " x17: " << emu.GetRegister(kX17);
  stream << std::endl;

  stream << " x18: " << emu.GetRegister(kX18);
  stream << " x19: " << emu.GetRegister(kX19);
  stream << " x20: " << emu.GetRegister(kX20);
  stream << std::endl;

  stream << " x21: " << emu.GetRegister(kX21);
  stream << " x22: " << emu.GetRegister(kX22);
  stream << " x23: " << emu.GetRegister(kX23);
  stream << std::endl;

  stream << " x24: " << emu.GetRegister(kX24);
  stream << " x25: " << emu.GetRegister(kX25);
  stream << " x26: " << emu.GetRegister(kX26);
  stream << std::endl;

  stream << " x27: " << emu.GetRegister(kX27);
  stream << " x28: " << emu.GetRegister(kX28);
  stream << " x29: " << emu.GetRegister(kX29);
  stream << std::endl;

  stream << " x30: " << emu.GetRegister(kX30);
  stream << "  sp: " << emu.GetRegister(kSp);
  stream << "  pc: " << emu.GetRegister(kPc);
  stream << std::endl;

  stream << "   n: " << static_cast<uint64_t>(emu.GetRegister(kN));
  stream << "    z: " << static_cast<uint64_t>(emu.GetRegister(kZ));
  stream << "    c: " << static_cast<uint64_t>(emu.GetRegister(kC));
  stream << "    v: " << static_cast<uint64_t>(emu.GetRegister(kV));
  stream << std::endl;

  if (emu.flags() & kAdvancedSIMD) {
    stream << "  v0: " << emu.GetRegister(kV0);
    stream << "  v1: " << emu.GetRegister(kV1);
    stream << std::endl;

    stream << "  v2: " << emu.GetRegister(kV2);
    stream << "  v3: " << emu.GetRegister(kV3);
    stream << std::endl;

    stream << "  v4: " << emu.GetRegister(kV4);
    stream << "  v5: " << emu.GetRegister(kV5);
    stream << std::endl;

    stream << "  v6: " << emu.GetRegister(kV6);
    stream << "  v7: " << emu.GetRegister(kV7);
    stream << std::endl;

    stream << "  v8: " << emu.GetRegister(kV8);
    stream << "  v9: " << emu.GetRegister(kV9);
    stream << std::endl;

    stream << " v10: " << emu.GetRegister(kV10);
    stream << " v11: " << emu.GetRegister(kV11);
    stream << std::endl;

    stream << " v12: " << emu.GetRegister(kV12);
    stream << " v13: " << emu.GetRegister(kV13);
    stream << std::endl;

    stream << " v14: " << emu.GetRegister(kV14);
    stream << " v15: " << emu.GetRegister(kV15);
    stream << std::endl;

    stream << " v16: " << emu.GetRegister(kV16);
    stream << " v17: " << emu.GetRegister(kV17);
    stream << std::endl;

    stream << " v18: " << emu.GetRegister(kV18);
    stream << " v19: " << emu.GetRegister(kV19);
    stream << std::endl;

    stream << " v20: " << emu.GetRegister(kV20);
    stream << " v21: " << emu.GetRegister(kV21);
    stream << std::endl;

    stream << " v22: " << emu.GetRegister(kV22);
    stream << " v23: " << emu.GetRegister(kV23);
    stream << std::endl;

    stream << " v24: " << emu.GetRegister(kV24);
    stream << " v25: " << emu.GetRegister(kV25);
    stream << std::endl;

    stream << " v26: " << emu.GetRegister(kV26);
    stream << " v27: " << emu.GetRegister(kV27);
    stream << std::endl;

    stream << " v28: " << emu.GetRegister(kV28);
    stream << " v29: " << emu.GetRegister(kV29);
    stream << std::endl;

    stream << " v30: " << emu.GetRegister(kV30);
    stream << " v31: " << emu.GetRegister(kV31);
    stream << std::endl;
  }

  return stream;
}
}  // namespace aarch64
}  // namespace reil
