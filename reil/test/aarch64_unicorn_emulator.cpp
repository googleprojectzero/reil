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

#include "aarch64_unicorn_emulator.h"

namespace reil {
namespace test {

Immediate AArch64UnicornEmulator::GetCoreRegister(uint32_t index) const {
  uint64_t value = 0;
  static std::map<uint32_t, uint32_t> register_mapping = {
      {aarch64::kX0, UC_ARM64_REG_X0},   {aarch64::kX1, UC_ARM64_REG_X1},
      {aarch64::kX2, UC_ARM64_REG_X2},   {aarch64::kX3, UC_ARM64_REG_X3},
      {aarch64::kX4, UC_ARM64_REG_X4},   {aarch64::kX5, UC_ARM64_REG_X5},
      {aarch64::kX6, UC_ARM64_REG_X6},   {aarch64::kX7, UC_ARM64_REG_X7},
      {aarch64::kX8, UC_ARM64_REG_X8},   {aarch64::kX9, UC_ARM64_REG_X9},
      {aarch64::kX10, UC_ARM64_REG_X10}, {aarch64::kX11, UC_ARM64_REG_X11},
      {aarch64::kX12, UC_ARM64_REG_X12}, {aarch64::kX13, UC_ARM64_REG_X13},
      {aarch64::kX14, UC_ARM64_REG_X14}, {aarch64::kX15, UC_ARM64_REG_X15},
      {aarch64::kX16, UC_ARM64_REG_X16}, {aarch64::kX17, UC_ARM64_REG_X17},
      {aarch64::kX18, UC_ARM64_REG_X18}, {aarch64::kX19, UC_ARM64_REG_X19},
      {aarch64::kX20, UC_ARM64_REG_X20}, {aarch64::kX21, UC_ARM64_REG_X21},
      {aarch64::kX22, UC_ARM64_REG_X22}, {aarch64::kX23, UC_ARM64_REG_X23},
      {aarch64::kX24, UC_ARM64_REG_X24}, {aarch64::kX25, UC_ARM64_REG_X25},
      {aarch64::kX26, UC_ARM64_REG_X26}, {aarch64::kX27, UC_ARM64_REG_X27},
      {aarch64::kX28, UC_ARM64_REG_X28}, {aarch64::kX29, UC_ARM64_REG_X29},
      {aarch64::kX30, UC_ARM64_REG_X30}, {aarch64::kSp, UC_ARM64_REG_SP},
      {aarch64::kPc, UC_ARM64_REG_PC},
  };

  uc_reg_read(unicorn_, register_mapping[index], &value);

  return Imm64(value);
}

Immediate AArch64UnicornEmulator::GetFlagRegister(uint32_t index) const {
  uint32_t nzcv = 0;
  uint8_t value = 0;

  uc_reg_read(unicorn_, UC_ARM64_REG_NZCV, &nzcv);

  switch (index) {
    case aarch64::kN: {
      value = (nzcv >> 31) & 1;
    } break;

    case aarch64::kZ: {
      value = (nzcv >> 30) & 1;
    } break;

    case aarch64::kC: {
      value = (nzcv >> 29) & 1;
    } break;

    case aarch64::kV: {
      value = (nzcv >> 28) & 1;
    } break;
  }

  return Imm8(value);
}

Immediate AArch64UnicornEmulator::GetSIMDRegister(uint32_t index) const {
  uint8_t value[16] = {0};
  static std::map<uint32_t, uint32_t> register_mapping = {
      {aarch64::kV0, UC_ARM64_REG_Q0},   {aarch64::kV1, UC_ARM64_REG_Q1},
      {aarch64::kV2, UC_ARM64_REG_Q2},   {aarch64::kV3, UC_ARM64_REG_Q3},
      {aarch64::kV4, UC_ARM64_REG_Q4},   {aarch64::kV5, UC_ARM64_REG_Q5},
      {aarch64::kV6, UC_ARM64_REG_Q6},   {aarch64::kV7, UC_ARM64_REG_Q7},
      {aarch64::kV8, UC_ARM64_REG_Q8},   {aarch64::kV9, UC_ARM64_REG_Q9},
      {aarch64::kV10, UC_ARM64_REG_Q10}, {aarch64::kV11, UC_ARM64_REG_Q11},
      {aarch64::kV12, UC_ARM64_REG_Q12}, {aarch64::kV13, UC_ARM64_REG_Q13},
      {aarch64::kV14, UC_ARM64_REG_Q14}, {aarch64::kV15, UC_ARM64_REG_Q15},
      {aarch64::kV16, UC_ARM64_REG_Q16}, {aarch64::kV17, UC_ARM64_REG_Q17},
      {aarch64::kV18, UC_ARM64_REG_Q18}, {aarch64::kV19, UC_ARM64_REG_Q19},
      {aarch64::kV20, UC_ARM64_REG_Q20}, {aarch64::kV21, UC_ARM64_REG_Q21},
      {aarch64::kV22, UC_ARM64_REG_Q22}, {aarch64::kV23, UC_ARM64_REG_Q23},
      {aarch64::kV24, UC_ARM64_REG_Q24}, {aarch64::kV25, UC_ARM64_REG_Q25},
      {aarch64::kV26, UC_ARM64_REG_Q26}, {aarch64::kV27, UC_ARM64_REG_Q27},
      {aarch64::kV28, UC_ARM64_REG_Q28}, {aarch64::kV29, UC_ARM64_REG_Q29},
      {aarch64::kV30, UC_ARM64_REG_Q30}, {aarch64::kV31, UC_ARM64_REG_Q31},
  };

  uc_reg_read(unicorn_, register_mapping[index], value);

  return Immediate(value, sizeof(value));
}

void AArch64UnicornEmulator::SetCoreRegister(uint32_t index, uint64_t value) {
  static std::map<uint32_t, uint32_t> register_mapping = {
      {aarch64::kX0, UC_ARM64_REG_X0},   {aarch64::kX1, UC_ARM64_REG_X1},
      {aarch64::kX2, UC_ARM64_REG_X2},   {aarch64::kX3, UC_ARM64_REG_X3},
      {aarch64::kX4, UC_ARM64_REG_X4},   {aarch64::kX5, UC_ARM64_REG_X5},
      {aarch64::kX6, UC_ARM64_REG_X6},   {aarch64::kX7, UC_ARM64_REG_X7},
      {aarch64::kX8, UC_ARM64_REG_X8},   {aarch64::kX9, UC_ARM64_REG_X9},
      {aarch64::kX10, UC_ARM64_REG_X10}, {aarch64::kX11, UC_ARM64_REG_X11},
      {aarch64::kX12, UC_ARM64_REG_X12}, {aarch64::kX13, UC_ARM64_REG_X13},
      {aarch64::kX14, UC_ARM64_REG_X14}, {aarch64::kX15, UC_ARM64_REG_X15},
      {aarch64::kX16, UC_ARM64_REG_X16}, {aarch64::kX17, UC_ARM64_REG_X17},
      {aarch64::kX18, UC_ARM64_REG_X18}, {aarch64::kX19, UC_ARM64_REG_X19},
      {aarch64::kX20, UC_ARM64_REG_X20}, {aarch64::kX21, UC_ARM64_REG_X21},
      {aarch64::kX22, UC_ARM64_REG_X22}, {aarch64::kX23, UC_ARM64_REG_X23},
      {aarch64::kX24, UC_ARM64_REG_X24}, {aarch64::kX25, UC_ARM64_REG_X25},
      {aarch64::kX26, UC_ARM64_REG_X26}, {aarch64::kX27, UC_ARM64_REG_X27},
      {aarch64::kX28, UC_ARM64_REG_X28}, {aarch64::kX29, UC_ARM64_REG_X29},
      {aarch64::kX30, UC_ARM64_REG_X30}, {aarch64::kSp, UC_ARM64_REG_SP},
      {aarch64::kPc, UC_ARM64_REG_PC},
  };

  uc_reg_write(unicorn_, register_mapping[index], &value);
}

void AArch64UnicornEmulator::SetFlagRegister(uint32_t index, uint8_t value) {
  uint32_t nzcv = 0;
  uc_reg_read(unicorn_, UC_ARM64_REG_NZCV, &nzcv);

  switch (index) {
    case aarch64::kN: {
      if (value) {
        nzcv |= (1 << 31);
      } else {
        nzcv &= ~(1 << 31);
      }
    } break;

    case aarch64::kZ: {
      if (value) {
        nzcv |= (1 << 30);
      } else {
        nzcv &= ~(1 << 30);
      }
    } break;

    case aarch64::kC: {
      if (value) {
        nzcv |= (1 << 29);
      } else {
        nzcv &= ~(1 << 29);
      }
    } break;

    case aarch64::kV: {
      if (value) {
        nzcv |= (1 << 28);
      } else {
        nzcv &= ~(1 << 28);
      }
    } break;
  }

  uc_reg_write(unicorn_, UC_ARM64_REG_NZCV, &nzcv);
}

void AArch64UnicornEmulator::SetSIMDRegister(uint32_t index, Immediate value) {
  static std::map<uint32_t, uint32_t> register_mapping = {
      {aarch64::kV0, UC_ARM64_REG_Q0},   {aarch64::kV1, UC_ARM64_REG_Q1},
      {aarch64::kV2, UC_ARM64_REG_Q2},   {aarch64::kV3, UC_ARM64_REG_Q3},
      {aarch64::kV4, UC_ARM64_REG_Q4},   {aarch64::kV5, UC_ARM64_REG_Q5},
      {aarch64::kV6, UC_ARM64_REG_Q6},   {aarch64::kV7, UC_ARM64_REG_Q7},
      {aarch64::kV8, UC_ARM64_REG_Q8},   {aarch64::kV9, UC_ARM64_REG_Q9},
      {aarch64::kV10, UC_ARM64_REG_Q10}, {aarch64::kV11, UC_ARM64_REG_Q11},
      {aarch64::kV12, UC_ARM64_REG_Q12}, {aarch64::kV13, UC_ARM64_REG_Q13},
      {aarch64::kV14, UC_ARM64_REG_Q14}, {aarch64::kV15, UC_ARM64_REG_Q15},
      {aarch64::kV16, UC_ARM64_REG_Q16}, {aarch64::kV17, UC_ARM64_REG_Q17},
      {aarch64::kV18, UC_ARM64_REG_Q18}, {aarch64::kV19, UC_ARM64_REG_Q19},
      {aarch64::kV20, UC_ARM64_REG_Q20}, {aarch64::kV21, UC_ARM64_REG_Q21},
      {aarch64::kV22, UC_ARM64_REG_Q22}, {aarch64::kV23, UC_ARM64_REG_Q23},
      {aarch64::kV24, UC_ARM64_REG_Q24}, {aarch64::kV25, UC_ARM64_REG_Q25},
      {aarch64::kV26, UC_ARM64_REG_Q26}, {aarch64::kV27, UC_ARM64_REG_Q27},
      {aarch64::kV28, UC_ARM64_REG_Q28}, {aarch64::kV29, UC_ARM64_REG_Q29},
      {aarch64::kV30, UC_ARM64_REG_Q30}, {aarch64::kV31, UC_ARM64_REG_Q31},
  };

  uc_reg_write(unicorn_, register_mapping[index], value.bytes().data());
}

AArch64UnicornEmulator::AArch64UnicornEmulator() {
  uint64_t zero[2] = {0};

  uc_open(UC_ARCH_ARM64, UC_MODE_ARM, &unicorn_);
  uc_mem_map(unicorn_, 0, 0x100000000, UC_PROT_ALL);

  uc_reg_write(unicorn_, UC_ARM64_REG_X0, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X1, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X2, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X3, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X4, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X5, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X6, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X7, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X8, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X9, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X10, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X11, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X12, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X13, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X14, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X15, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X16, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X17, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X18, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X19, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X20, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X21, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X22, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X23, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X24, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X25, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X26, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X27, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X28, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X29, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_X30, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_SP, zero);

  uc_reg_write(unicorn_, UC_ARM64_REG_Q0, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q1, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q2, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q3, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q4, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q5, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q6, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q7, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q8, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q9, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q10, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q11, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q12, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q13, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q14, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q15, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q16, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q17, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q18, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q19, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q20, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q21, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q22, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q23, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q24, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q25, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q26, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q27, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q28, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q29, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q30, zero);
  uc_reg_write(unicorn_, UC_ARM64_REG_Q31, zero);

  // we need to disable trapping on simd instructions
  uint64_t cpacr_el1 = 0;
  uc_reg_read(unicorn_, UC_ARM64_REG_CPACR_EL1, &cpacr_el1);
  cpacr_el1 |= 0x00300000;
  uc_reg_write(unicorn_, UC_ARM64_REG_CPACR_EL1, &cpacr_el1);
}

AArch64UnicornEmulator::~AArch64UnicornEmulator() {
  uc_mem_unmap(unicorn_, 0, 0x100000000);
  uc_close(unicorn_);
}

bool AArch64UnicornEmulator::SingleStep() {
  uint64_t pc = static_cast<uint64_t>(GetCoreRegister(aarch64::kPc));
  if (uc_emu_start(unicorn_, pc, 0xffffffff, 0, 1)) {
    return false;
  }
  return true;
}

Immediate AArch64UnicornEmulator::GetRegister(uint32_t index) const {
  if (aarch64::kX0 <= index && index <= aarch64::kPc) {
    return GetCoreRegister(index);
  } else if (aarch64::kN <= index && index <= aarch64::kV) {
    return GetFlagRegister(index);
  } else if (aarch64::kV0 <= index && index <= aarch64::kV31) {
    return GetSIMDRegister(index);
  } else {
    abort();
  }
}

void AArch64UnicornEmulator::SetRegister(uint32_t index, Immediate value) {
  if (aarch64::kX0 <= index && index <= aarch64::kPc) {
    SetCoreRegister(index, static_cast<uint64_t>(value));
  } else if (aarch64::kN <= index && index <= aarch64::kV) {
    SetFlagRegister(index, static_cast<uint64_t>(value));
  } else if (aarch64::kV0 <= index && index <= aarch64::kV31) {
    SetSIMDRegister(index, value);
  } else {
    abort();
  }
}

std::vector<uint8_t> AArch64UnicornEmulator::GetMemory(uint64_t address,
                                                       size_t size) {
  std::vector<uint8_t> bytes(size);
  uc_mem_read(unicorn_, address, bytes.data(), bytes.size());
  return bytes;
}

void AArch64UnicornEmulator::SetMemory(uint64_t address,
                                       const std::vector<uint8_t> &bytes) {
  SetMemory(address, bytes.data(), bytes.size());
}

void AArch64UnicornEmulator::SetMemory(uint64_t address, const uint8_t *bytes,
                                       size_t bytes_len) {
  uc_mem_write(unicorn_, address, bytes, bytes_len);
}

std::ostream &operator<<(std::ostream &stream,
                         const AArch64UnicornEmulator &emu) {
  stream << "  x0: " << emu.GetRegister(aarch64::kX0);
  stream << "  x1: " << emu.GetRegister(aarch64::kX1);
  stream << "  x2: " << emu.GetRegister(aarch64::kX2);
  stream << std::endl;

  stream << "  x3: " << emu.GetRegister(aarch64::kX3);
  stream << "  x4: " << emu.GetRegister(aarch64::kX4);
  stream << "  x5: " << emu.GetRegister(aarch64::kX5);
  stream << std::endl;

  stream << "  x6: " << emu.GetRegister(aarch64::kX6);
  stream << "  x7: " << emu.GetRegister(aarch64::kX7);
  stream << "  x8: " << emu.GetRegister(aarch64::kX8);
  stream << std::endl;

  stream << "  x9: " << emu.GetRegister(aarch64::kX9);
  stream << " x10: " << emu.GetRegister(aarch64::kX10);
  stream << " x11: " << emu.GetRegister(aarch64::kX11);
  stream << std::endl;

  stream << " x12: " << emu.GetRegister(aarch64::kX12);
  stream << " x13: " << emu.GetRegister(aarch64::kX13);
  stream << " x14: " << emu.GetRegister(aarch64::kX14);
  stream << std::endl;

  stream << " x15: " << emu.GetRegister(aarch64::kX15);
  stream << " x16: " << emu.GetRegister(aarch64::kX16);
  stream << " x17: " << emu.GetRegister(aarch64::kX17);
  stream << std::endl;

  stream << " x18: " << emu.GetRegister(aarch64::kX18);
  stream << " x19: " << emu.GetRegister(aarch64::kX19);
  stream << " x20: " << emu.GetRegister(aarch64::kX20);
  stream << std::endl;

  stream << " x21: " << emu.GetRegister(aarch64::kX21);
  stream << " x22: " << emu.GetRegister(aarch64::kX22);
  stream << " x23: " << emu.GetRegister(aarch64::kX23);
  stream << std::endl;

  stream << " x24: " << emu.GetRegister(aarch64::kX24);
  stream << " x25: " << emu.GetRegister(aarch64::kX25);
  stream << " x26: " << emu.GetRegister(aarch64::kX26);
  stream << std::endl;

  stream << " x27: " << emu.GetRegister(aarch64::kX27);
  stream << " x28: " << emu.GetRegister(aarch64::kX28);
  stream << " x29: " << emu.GetRegister(aarch64::kX29);
  stream << std::endl;

  stream << " x30: " << emu.GetRegister(aarch64::kX30);
  stream << "  sp: " << emu.GetRegister(aarch64::kSp);
  stream << "  pc: " << emu.GetRegister(aarch64::kPc);
  stream << std::endl;

  stream << "   n: " << static_cast<uint64_t>(emu.GetRegister(aarch64::kN));
  stream << "    z: " << static_cast<uint64_t>(emu.GetRegister(aarch64::kZ));
  stream << "    c: " << static_cast<uint64_t>(emu.GetRegister(aarch64::kC));
  stream << "    v: " << static_cast<uint64_t>(emu.GetRegister(aarch64::kV));
  stream << std::endl;

  stream << "  v0: " << emu.GetRegister(aarch64::kV0);
  stream << "  v1: " << emu.GetRegister(aarch64::kV1);
  stream << std::endl;

  stream << "  v2: " << emu.GetRegister(aarch64::kV2);
  stream << "  v3: " << emu.GetRegister(aarch64::kV3);
  stream << std::endl;

  stream << "  v4: " << emu.GetRegister(aarch64::kV4);
  stream << "  v5: " << emu.GetRegister(aarch64::kV5);
  stream << std::endl;

  stream << "  v6: " << emu.GetRegister(aarch64::kV6);
  stream << "  v7: " << emu.GetRegister(aarch64::kV7);
  stream << std::endl;

  stream << "  v8: " << emu.GetRegister(aarch64::kV8);
  stream << "  v9: " << emu.GetRegister(aarch64::kV9);
  stream << std::endl;

  stream << " v10: " << emu.GetRegister(aarch64::kV10);
  stream << " v11: " << emu.GetRegister(aarch64::kV11);
  stream << std::endl;

  stream << " v12: " << emu.GetRegister(aarch64::kV12);
  stream << " v13: " << emu.GetRegister(aarch64::kV13);
  stream << std::endl;

  stream << " v14: " << emu.GetRegister(aarch64::kV14);
  stream << " v15: " << emu.GetRegister(aarch64::kV15);
  stream << std::endl;

  stream << " v16: " << emu.GetRegister(aarch64::kV16);
  stream << " v17: " << emu.GetRegister(aarch64::kV17);
  stream << std::endl;

  stream << " v18: " << emu.GetRegister(aarch64::kV18);
  stream << " v19: " << emu.GetRegister(aarch64::kV19);
  stream << std::endl;

  stream << " v20: " << emu.GetRegister(aarch64::kV20);
  stream << " v21: " << emu.GetRegister(aarch64::kV21);
  stream << std::endl;

  stream << " v22: " << emu.GetRegister(aarch64::kV22);
  stream << " v23: " << emu.GetRegister(aarch64::kV23);
  stream << std::endl;

  stream << " v24: " << emu.GetRegister(aarch64::kV24);
  stream << " v25: " << emu.GetRegister(aarch64::kV25);
  stream << std::endl;

  stream << " v26: " << emu.GetRegister(aarch64::kV26);
  stream << " v27: " << emu.GetRegister(aarch64::kV27);
  stream << std::endl;

  stream << " v28: " << emu.GetRegister(aarch64::kV28);
  stream << " v29: " << emu.GetRegister(aarch64::kV29);
  stream << std::endl;

  stream << " v30: " << emu.GetRegister(aarch64::kV30);
  stream << " v31: " << emu.GetRegister(aarch64::kV31);
  stream << std::endl;

  return stream;
}
}  // namespace test
}  // namespace reil
