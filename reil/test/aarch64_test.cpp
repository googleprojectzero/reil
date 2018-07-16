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

#include <cstdio>
#include <random>

#include <gtest/gtest.h>

#include <reil/aarch64.h>

#include "aarch64_unicorn_emulator.h"

namespace reil {
namespace test {

std::mt19937_64 prng;

static Immediate prng128() {
  uint64_t value[2];

  value[0] = prng();
  value[1] = prng();

  return Immediate((uint8_t *)value, 16);
}

void Check(aarch64::Emulator &reil_emu, AArch64UnicornEmulator &unicorn_emu) {
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX0),
            unicorn_emu.GetRegister(aarch64::kX0));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX1),
            unicorn_emu.GetRegister(aarch64::kX1));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX2),
            unicorn_emu.GetRegister(aarch64::kX2));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX3),
            unicorn_emu.GetRegister(aarch64::kX3));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX4),
            unicorn_emu.GetRegister(aarch64::kX4));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX5),
            unicorn_emu.GetRegister(aarch64::kX5));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX6),
            unicorn_emu.GetRegister(aarch64::kX6));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX7),
            unicorn_emu.GetRegister(aarch64::kX7));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX8),
            unicorn_emu.GetRegister(aarch64::kX8));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX9),
            unicorn_emu.GetRegister(aarch64::kX9));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX10),
            unicorn_emu.GetRegister(aarch64::kX10));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX11),
            unicorn_emu.GetRegister(aarch64::kX11));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX12),
            unicorn_emu.GetRegister(aarch64::kX12));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX13),
            unicorn_emu.GetRegister(aarch64::kX13));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX14),
            unicorn_emu.GetRegister(aarch64::kX14));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX15),
            unicorn_emu.GetRegister(aarch64::kX15));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX16),
            unicorn_emu.GetRegister(aarch64::kX16));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX17),
            unicorn_emu.GetRegister(aarch64::kX17));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX18),
            unicorn_emu.GetRegister(aarch64::kX18));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX19),
            unicorn_emu.GetRegister(aarch64::kX19));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX20),
            unicorn_emu.GetRegister(aarch64::kX20));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX21),
            unicorn_emu.GetRegister(aarch64::kX21));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX22),
            unicorn_emu.GetRegister(aarch64::kX22));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX23),
            unicorn_emu.GetRegister(aarch64::kX23));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX24),
            unicorn_emu.GetRegister(aarch64::kX24));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX25),
            unicorn_emu.GetRegister(aarch64::kX25));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX26),
            unicorn_emu.GetRegister(aarch64::kX26));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX27),
            unicorn_emu.GetRegister(aarch64::kX27));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX28),
            unicorn_emu.GetRegister(aarch64::kX28));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX29),
            unicorn_emu.GetRegister(aarch64::kX29));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kX30),
            unicorn_emu.GetRegister(aarch64::kX30));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kSp),
            unicorn_emu.GetRegister(aarch64::kSp));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kPc),
            unicorn_emu.GetRegister(aarch64::kPc));

  ASSERT_EQ(reil_emu.GetRegister(aarch64::kN),
            unicorn_emu.GetRegister(aarch64::kN));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kZ),
            unicorn_emu.GetRegister(aarch64::kZ));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kC),
            unicorn_emu.GetRegister(aarch64::kC));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV),
            unicorn_emu.GetRegister(aarch64::kV));

  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV0),
            unicorn_emu.GetRegister(aarch64::kV0));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV1),
            unicorn_emu.GetRegister(aarch64::kV1));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV2),
            unicorn_emu.GetRegister(aarch64::kV2));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV3),
            unicorn_emu.GetRegister(aarch64::kV3));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV4),
            unicorn_emu.GetRegister(aarch64::kV4));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV5),
            unicorn_emu.GetRegister(aarch64::kV5));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV6),
            unicorn_emu.GetRegister(aarch64::kV6));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV7),
            unicorn_emu.GetRegister(aarch64::kV7));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV8),
            unicorn_emu.GetRegister(aarch64::kV8));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV9),
            unicorn_emu.GetRegister(aarch64::kV9));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV10),
            unicorn_emu.GetRegister(aarch64::kV10));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV11),
            unicorn_emu.GetRegister(aarch64::kV11));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV12),
            unicorn_emu.GetRegister(aarch64::kV12));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV13),
            unicorn_emu.GetRegister(aarch64::kV13));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV14),
            unicorn_emu.GetRegister(aarch64::kV14));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV15),
            unicorn_emu.GetRegister(aarch64::kV15));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV16),
            unicorn_emu.GetRegister(aarch64::kV16));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV17),
            unicorn_emu.GetRegister(aarch64::kV17));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV18),
            unicorn_emu.GetRegister(aarch64::kV18));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV19),
            unicorn_emu.GetRegister(aarch64::kV19));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV20),
            unicorn_emu.GetRegister(aarch64::kV20));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV21),
            unicorn_emu.GetRegister(aarch64::kV21));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV22),
            unicorn_emu.GetRegister(aarch64::kV22));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV23),
            unicorn_emu.GetRegister(aarch64::kV23));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV24),
            unicorn_emu.GetRegister(aarch64::kV24));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV25),
            unicorn_emu.GetRegister(aarch64::kV25));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV26),
            unicorn_emu.GetRegister(aarch64::kV26));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV27),
            unicorn_emu.GetRegister(aarch64::kV27));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV28),
            unicorn_emu.GetRegister(aarch64::kV28));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV29),
            unicorn_emu.GetRegister(aarch64::kV29));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV30),
            unicorn_emu.GetRegister(aarch64::kV30));
  ASSERT_EQ(reil_emu.GetRegister(aarch64::kV31),
            unicorn_emu.GetRegister(aarch64::kV31));

  auto reil_memory = reil_emu.GetMemory(0x1000, 0x100);
  ASSERT_EQ(reil_memory.size(), 0x100);

  auto unicorn_memory = unicorn_emu.GetMemory(0x1000, 0x100);
  ASSERT_EQ(unicorn_memory.size(), 0x100);

  for (size_t i = 0; i < 0x100; ++i) {
    ASSERT_EQ(reil_memory[i], unicorn_memory[i]);
  }
}

void AArch64Test(uint32_t instruction_count, std::vector<uint8_t> bytes,
                 bool can_fail = false) {
  uint64_t x0 = prng();
  uint64_t x1 = prng();
  uint64_t x2 = prng();
  uint64_t x3 = prng();
  uint8_t nzcv = prng();

  bytes.reserve(0x1000);
  while (bytes.size() < 0x1000) {
    bytes.push_back(0);
  }

  AArch64UnicornEmulator unicorn_emu;
  unicorn_emu.SetMemory(0x1000, bytes);
  unicorn_emu.SetRegister(aarch64::kX0, Imm64(x0));
  unicorn_emu.SetRegister(aarch64::kX1, Imm64(x1));
  unicorn_emu.SetRegister(aarch64::kX2, Imm64(x2));
  unicorn_emu.SetRegister(aarch64::kX3, Imm64(x3));
  unicorn_emu.SetRegister(aarch64::kN, Imm8((nzcv >> 3) & 0b1));
  unicorn_emu.SetRegister(aarch64::kZ, Imm8((nzcv >> 2) & 0b1));
  unicorn_emu.SetRegister(aarch64::kC, Imm8((nzcv >> 1) & 0b1));
  unicorn_emu.SetRegister(aarch64::kV, Imm8((nzcv >> 0) & 0b1));
  unicorn_emu.SetRegister(aarch64::kV0, prng128());
  unicorn_emu.SetRegister(aarch64::kV1, prng128());
  unicorn_emu.SetRegister(aarch64::kV2, prng128());
  unicorn_emu.SetRegister(aarch64::kV3, prng128());
  unicorn_emu.SetRegister(aarch64::kPc, Imm64(0x1000));

  reil::aarch64::Emulator reil_emu(aarch64::kAdvancedSIMD);
  reil_emu.SetMemory(0x1000, bytes);
  reil_emu.SetRegister(aarch64::kX0, unicorn_emu.GetRegister(aarch64::kX0));
  reil_emu.SetRegister(aarch64::kX1, unicorn_emu.GetRegister(aarch64::kX1));
  reil_emu.SetRegister(aarch64::kX2, unicorn_emu.GetRegister(aarch64::kX2));
  reil_emu.SetRegister(aarch64::kX3, unicorn_emu.GetRegister(aarch64::kX3));
  reil_emu.SetRegister(aarch64::kN, unicorn_emu.GetRegister(aarch64::kN));
  reil_emu.SetRegister(aarch64::kZ, unicorn_emu.GetRegister(aarch64::kZ));
  reil_emu.SetRegister(aarch64::kC, unicorn_emu.GetRegister(aarch64::kC));
  reil_emu.SetRegister(aarch64::kV, unicorn_emu.GetRegister(aarch64::kV));
  reil_emu.SetRegister(aarch64::kV0, unicorn_emu.GetRegister(aarch64::kV0));
  reil_emu.SetRegister(aarch64::kV1, unicorn_emu.GetRegister(aarch64::kV1));
  reil_emu.SetRegister(aarch64::kV2, unicorn_emu.GetRegister(aarch64::kV2));
  reil_emu.SetRegister(aarch64::kV3, unicorn_emu.GetRegister(aarch64::kV3));
  reil_emu.SetRegister(aarch64::kPc, Imm64(0x1000));

  reil::aarch64::Emulator reil_min_emu(kMinimalBranches |
                                       aarch64::kAdvancedSIMD);
  reil_min_emu.SetMemory(0x1000, bytes);
  reil_min_emu.SetRegister(aarch64::kX0, unicorn_emu.GetRegister(aarch64::kX0));
  reil_min_emu.SetRegister(aarch64::kX1, unicorn_emu.GetRegister(aarch64::kX1));
  reil_min_emu.SetRegister(aarch64::kX2, unicorn_emu.GetRegister(aarch64::kX2));
  reil_min_emu.SetRegister(aarch64::kX3, unicorn_emu.GetRegister(aarch64::kX3));
  reil_min_emu.SetRegister(aarch64::kN, unicorn_emu.GetRegister(aarch64::kN));
  reil_min_emu.SetRegister(aarch64::kZ, unicorn_emu.GetRegister(aarch64::kZ));
  reil_min_emu.SetRegister(aarch64::kC, unicorn_emu.GetRegister(aarch64::kC));
  reil_min_emu.SetRegister(aarch64::kV, unicorn_emu.GetRegister(aarch64::kV));
  reil_min_emu.SetRegister(aarch64::kV0, unicorn_emu.GetRegister(aarch64::kV0));
  reil_min_emu.SetRegister(aarch64::kV1, unicorn_emu.GetRegister(aarch64::kV1));
  reil_min_emu.SetRegister(aarch64::kV2, unicorn_emu.GetRegister(aarch64::kV2));
  reil_min_emu.SetRegister(aarch64::kV3, unicorn_emu.GetRegister(aarch64::kV3));
  reil_min_emu.SetRegister(aarch64::kPc, Imm64(0x1000));

  for (uint32_t j = 0; j < instruction_count; ++j) {
    if (can_fail) {
      if (!unicorn_emu.SingleStep()) {
        return;
      }
    } else {
      ASSERT_TRUE(unicorn_emu.SingleStep());
    }

    ASSERT_TRUE(reil_emu.SingleStep());
    ASSERT_TRUE(reil_min_emu.SingleStep());

    Check(reil_emu, unicorn_emu);
    Check(reil_min_emu, unicorn_emu);
  }
}

#define AARCH64_TEST(test_name, instruction_count, ...) \
  TEST(AArch64Translator, test_name) {                  \
    AArch64Test(instruction_count, __VA_ARGS__);        \
  }

#define AARCH64_RANDOM_TEST(test_name, base, mask) \
  TEST(AArch64Translator, Random##test_name) {     \
    std::vector<uint8_t> bytes(32);                \
    for (int i = 0; i < 8; ++i) {                  \
      uint32_t rndm = (uint32_t)prng();            \
      uint32_t insn = base | (mask & rndm);        \
      bytes[(i * 4) + 3] = (insn >> 24) & 0xff;    \
      bytes[(i * 4) + 2] = (insn >> 16) & 0xff;    \
      bytes[(i * 4) + 1] = (insn >> 8) & 0xff;     \
      bytes[(i * 4) + 0] = insn & 0xff;            \
    }                                              \
    AArch64Test(128, bytes, true);                 \
  }

AARCH64_TEST(Adr, 1,
             {
                 0x60, 0x00, 0x00, 0x10,  // adr  x0, c
             })

AARCH64_TEST(Adrp, 1,
             {
                 0x00, 0x00, 0x00, 0x90,  // adrp  x0, 0
             })

AARCH64_TEST(AddImmediate, 3,
             {
                 0x01, 0x48, 0x00, 0x91,  // add  x1, x0, #0x12
                 0x01, 0x48, 0x40, 0x91,  // add  x1, x0, #0x12, lsl #12
                 0x01, 0xfc, 0x3f, 0x11,  // add  w1, w0, #0xfff
             })

AARCH64_TEST(AddsImmediate, 3,
             {
                 0x01, 0x48, 0x00, 0xb1,  // adds  x1, x0, #0x12
                 0x01, 0x48, 0x40, 0xb1,  // adds  x1, x0, #0x12, lsl #12
                 0x01, 0xfc, 0x7f, 0x31,  // adds  w1, w0, #0xfff, lsl #12
             })

AARCH64_TEST(CmnImmediate, 2,
             {
                 0x1f, 0xf8, 0x3d, 0xb1,  // cmn  x0, #0xf7e
                 0x1f, 0xf8, 0x7d, 0x31,  // cmn  w0, #0xf7e, lsl #12
             })

AARCH64_TEST(CmpImmediate, 2,
             {
                 0x1f, 0xf8, 0x3d, 0xf1,  // cmp  x0, #0xf7e
                 0x1f, 0xf8, 0x7d, 0x71,  // cmp  w0, #0xf7e, lsl #12
             })

AARCH64_TEST(MovToFromSp, 2,
             {
                 0x1f, 0x00, 0x00, 0x91,  // mov  sp, x0
                 0xe1, 0x03, 0x00, 0x11,  // mov  w1, wsp
             })

AARCH64_TEST(SubImmediate, 3,
             {
                 0x01, 0x48, 0x00, 0xd1,  // sub  x1, x0, #0x12
                 0x01, 0x48, 0x40, 0xd1,  // sub  x1, x0, #0x12, lsl #12
                 0x01, 0xfc, 0x3f, 0x51,  // sub  w1, w0, #0xfff
             })

AARCH64_TEST(SubsImmediate, 3,
             {
                 0x01, 0x48, 0x00, 0xf1,  // subs  x1, x0, #0x12
                 0x01, 0x48, 0x40, 0xf1,  // subs  x1, x0, #0x12, lsl #12
                 0x01, 0xfc, 0x7f, 0x71,  // subs  w1, w0, #0xfff, lsl #12
             })

AARCH64_TEST(AndImmediate, 3,
             {
                 0x01, 0xc4, 0x02, 0x92,  // and  x1, x0, #0xc0c0c0c0c0c0c0c0
                 0x01, 0x9c, 0x08, 0x92,  // and  x1, x0, #0xff00ff00ff00ff00
                 0x01, 0x24, 0x00, 0x12,  // and  w1, w0, #0x3ff
             })

AARCH64_TEST(OrrImmediate, 3,
             {
                 0x01, 0xc4, 0x02, 0xb2,  // orr  x1, x0, #0xc0c0c0c0c0c0c0c0
                 0x01, 0x9c, 0x08, 0xb2,  // orr  x1, x0, #0xff00ff00ff00ff00
                 0x01, 0x24, 0x00, 0x32,  // orr  w1, w0, #0x3ff
             })

AARCH64_TEST(EorImmediate, 3,
             {
                 0x01, 0xc4, 0x02, 0xd2,  // eor  x1, x0, #0xc0c0c0c0c0c0c0c0
                 0x01, 0x9c, 0x08, 0xd2,  // eor  x1, x0, #0xff00ff00ff00ff00
                 0x01, 0x24, 0x00, 0x52,  // eor  w1, w0, #0x3ff
             })

AARCH64_TEST(AndsImmediate, 3,
             {
                 0x01, 0xc4, 0x02, 0xf2,  // ands  x1, x0, #0xc0c0c0c0c0c0c0c0
                 0x01, 0x9c, 0x08, 0xf2,  // ands  x1, x0, #0xff00ff00ff00ff00
                 0x01, 0x24, 0x00, 0x72,  // ands  w1, w0, #0x3ff
             })

AARCH64_TEST(TstImmediate, 2,
             {
                 0x1f, 0x28, 0x40, 0xf2,  // tst  x0, #0x7ff
                 0x1f, 0x24, 0x00, 0x72,  // tst  w0, #0x3ff
             })

AARCH64_TEST(Movn, 3,
             {
                 0xe0, 0xff, 0x9f, 0x92,  // mov  x0, #0xffffffffffff0000
                 0xe0, 0xff, 0xdf, 0x92,  // mov  x0, #0xffff0000ffffffff
                 0xe0, 0x1f, 0xa0, 0x12,  // mov  w0, #0xff00ffff
             })

AARCH64_TEST(Movz, 3,
             {
                 0x80, 0x46, 0x82, 0xd2,  // mov  x0, #0x1234
                 0xa0, 0x79, 0xd5, 0xd2,  // mov  x0, #0xabcd00000000
                 0x40, 0x02, 0xa0, 0x52,  // mov  w0, #0x120000
             })

AARCH64_TEST(Movk, 3,
             {
                 0x80, 0x46, 0xe2, 0xf2,  // movk  x0, #0x1234, lsl #48
                 0xa0, 0x79, 0xd5, 0xf2,  // movk  x0, #0xabcd, lsl #32
                 0x40, 0x02, 0xa0, 0x72,  // movk  w0, #0x12, lsl #16
             })

AARCH64_TEST(AsrImmediate, 2,
             {
                 0x01, 0xfc, 0x4b, 0x93,  // asr  x1, x0, #11
                 0x01, 0x7c, 0x06, 0x13,  // asr  w1, w0, #6
             })

AARCH64_TEST(Sbfiz, 2,
             {
                 0x01, 0x18, 0x4a, 0x93,  // sbfiz  x1, x0, #54, #7
                 0x01, 0x04, 0x13, 0x13,  // sbfiz  w1, w0, #13, #2
             })

AARCH64_TEST(Sbfx, 2,
             {
                 0x01, 0xb0, 0x4b, 0x93,  // sbfx  x1, x0, #11, #34
                 0x01, 0x08, 0x01, 0x13,  // sbfx  w1, w0, #1, #2
             })

AARCH64_TEST(Sxtb, 1,
             {
                 0x01, 0x1c, 0x00, 0x13,  // sxtb  w1, w0
             })

AARCH64_TEST(Sxth, 1,
             {
                 0x01, 0x3c, 0x00, 0x13,  // sxth  w1, w0
             })

AARCH64_TEST(Bfi, 3,
             {
                 0x01, 0x40, 0x79, 0xb3,  // bfi  x1, x0, #7, #17
                 0x01, 0x00, 0x42, 0xb3,  // bfi  x1, x0, #62, #1
                 0x01, 0x24, 0x0c, 0x33,  // bfi  w1, w0, #20, #10
             })

AARCH64_TEST(Bfxil, 3,
             {
                 0x01, 0x5c, 0x47, 0xb3,  // bfxil  x1, x0, #7, #17
                 0x01, 0xf8, 0x7e, 0xb3,  // bfxil  x1, x0, #62, #1
                 0x01, 0x74, 0x14, 0x33,  // bfxil  w1, w0, #20, #10
             })

AARCH64_TEST(LslImmediate, 2,
             {
                 0x01, 0xb8, 0x6f, 0xd3,  // lsl  x1, x0, #17
                 0x01, 0x70, 0x1d, 0x53,  // lsl  w1, w0, #3
             })

AARCH64_TEST(LsrImmediate, 2,
             {
                 0x01, 0xfc, 0x4b, 0xd3,  // lsr  x1, x0, #11
                 0x01, 0x7c, 0x06, 0x53,  // lsr  w1, w0, #6
             })

AARCH64_TEST(Ubfiz, 2,
             {
                 0x01, 0x18, 0x4a, 0xd3,  // ubfiz  x1, x0, #54, #7
                 0x01, 0x04, 0x13, 0x53,  // ubfiz  w1, w0, #13, #2
             })

AARCH64_TEST(Ubfx, 2,
             {
                 0x01, 0xb0, 0x4b, 0xd3,  // ubfx  x1, x0, #11, #34
                 0x01, 0x08, 0x01, 0x53,  // ubfx  w1, w0, #1, #2
             })

AARCH64_TEST(Uxtb, 1,
             {
                 0x01, 0x1c, 0x00, 0x53,  // uxtb  w1, w0
             })

AARCH64_TEST(Uxth, 1,
             {
                 0x01, 0x3c, 0x00, 0x53,  // uxth  w1, w0
             })

AARCH64_TEST(RorImmediate, 2,
             {
                 0x01, 0x9c, 0xc0, 0x93,  // ror  x1, x0, #39
                 0x01, 0x18, 0x80, 0x13,  // ror  w1, w0, #6
             })

AARCH64_TEST(Extract, 2,
             {
                 0x22, 0xf4, 0xc0, 0x93,  // extr  x2, x1, x0, #61
                 0x22, 0x44, 0x80, 0x13,  // extr  w2, w1, w0, #17
             })

AARCH64_TEST(CompareAndBranch, 7,
             {
                 0x00, 0x00, 0x80, 0xd2,  // mov  x0, #0x0
                 0x81, 0x46, 0x82, 0xd2,  // mov  x1, #0x1234
                 0xc0, 0x00, 0x00, 0xb5,  // cbnz  x0, 20 <not_taken>
                 0x40, 0x00, 0x00, 0xb4,  // cbz  x0, 14 <taken_0>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x61, 0x00, 0x00, 0x34,  // cbz  w1, 20 <not_taken>
                 0x01, 0x00, 0x00, 0x35,  // cbnz  w1, 0 <taken_1>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
             })

AARCH64_TEST(ConditionalBranch, 17,
             {
                 0x00, 0x00, 0x80, 0xd2,  // mov  x0, #0x0
                 0x1f, 0x00, 0x00, 0xf1,  // cmp  x0, #0x0
                 0x40, 0x00, 0x00, 0x54,  // b.eq  10 <taken_0>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x81, 0x02, 0x00, 0x54,  // b.ne  60 <not_taken>
                 0x42, 0x00, 0x00, 0x54,  // b.cs  1c <taken_1>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x23, 0x02, 0x00, 0x54,  // b.cc  60 <not_taken>
                 0x45, 0x00, 0x00, 0x54,  // b.pl  28 <taken_2>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0xc4, 0x01, 0x00, 0x54,  // b.mi  60 <not_taken>
                 0x47, 0x00, 0x00, 0x54,  // b.vc  34 <taken_3>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x66, 0x01, 0x00, 0x54,  // b.vs  60 <not_taken>
                 0x49, 0x00, 0x00, 0x54,  // b.ls  40 <taken_4>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x08, 0x01, 0x00, 0x54,  // b.hi  60 <not_taken>
                 0x4a, 0x00, 0x00, 0x54,  // b.ge  4c <taken_5>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0xab, 0x00, 0x00, 0x54,  // b.lt  60 <not_taken>
                 0x4d, 0x00, 0x00, 0x54,  // b.le  58 <taken_6>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x4e, 0x00, 0x00, 0x54,  // b.al  60 <not_taken>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
             })

AARCH64_TEST(TestAndBranchIfZero, 2,
             {
                 0x40, 0x00, 0x88, 0x36,  // tbz  w0, #17, 8 <taken>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
             })

AARCH64_TEST(TestAndBranchIfNonzero, 2,
             {
                 0x40, 0x00, 0xf8, 0xb7,  // tbnz  x0, #63, 8 <taken>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
             })

AARCH64_TEST(BranchRegister, 2,
             {
                 0xa0, 0x00, 0x00, 0x10,  // adr  x0, 14 <forward>
                 0x00, 0x00, 0x1f, 0xd6,  // br  x0
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
             })

AARCH64_TEST(BranchAndLinkRegister, 2,
             {
                 0xa0, 0x00, 0x00, 0x10,  // adr  x0, 14 <forward>
                 0x00, 0x00, 0x3f, 0xd6,  // blr  x0
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
             })

AARCH64_TEST(BranchImmediateForward, 2,
             {
                 0x04, 0x00, 0x00, 0x14,  // b  10 <forward>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
             })

AARCH64_TEST(BranchImmediateBackward, 5,
             {
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0xfd, 0xff, 0xff, 0x17,  // b  0 <backward>
             })

AARCH64_TEST(BranchAndLinkImmediateForward, 2,
             {
                 0x04, 0x00, 0x00, 0x94,  // bl  10 <forward>
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
             })

AARCH64_TEST(BranchAndLinkImmediateBackward, 5,
             {
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0xfd, 0xff, 0xff, 0x97,  // bl  0 <backward>
             })

AARCH64_TEST(Casb, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x41, 0x7c, 0xa0, 0x08,  // casb  w0, w1, [x2]
                 0x40, 0x00, 0x00, 0xb9,  // str  w0, [x2]
                 0x41, 0x7c, 0xa0, 0x08,  // casb  w0, w1, [x2]
                 0x23, 0x23, 0x23, 0x23,
             })

AARCH64_TEST(Cash, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x41, 0x7c, 0xa0, 0x08,  // casb  w0, w1, [x2]
                 0x40, 0x00, 0x00, 0xb9,  // str  w0, [x2]
                 0x41, 0x7c, 0xa0, 0x08,  // casb  w0, w1, [x2]
                 0x23, 0x23, 0x23, 0x23,
             })

AARCH64_TEST(Casw, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x41, 0x7c, 0xa0, 0x08,  // casb  w0, w1, [x2]
                 0x40, 0x00, 0x00, 0xb9,  // str  w0, [x2]
                 0x41, 0x7c, 0xa0, 0x08,  // casb  w0, w1, [x2]
                 0x23, 0x23, 0x23, 0x23,
             })

AARCH64_TEST(Cas, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x41, 0x7c, 0xa0, 0x08,  // casb  w0, w1, [x2]
                 0x40, 0x00, 0x00, 0xb9,  // str  w0, [x2]
                 0x41, 0x7c, 0xa0, 0x08,  // casb  w0, w1, [x2]
                 0x23, 0x23, 0x23, 0x23,
             })

AARCH64_TEST(Stxrb, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x7c, 0x5f, 0x08,  // ldxrb  w0, [x2]
                 0x41, 0x7c, 0x00, 0x08,  // stxrb  w0, w1, [x2]
                 0x40, 0x7c, 0x5f, 0x08,  // ldxrb  w0, [x2]
             })

AARCH64_TEST(Stlxrb, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0xfc, 0x5f, 0x08,  // ldaxrb  w0, [x2]
                 0x41, 0xfc, 0x00, 0x08,  // stlxrb  w0, w1, [x2]
                 0x40, 0xfc, 0x5f, 0x08,  // ldaxrb  w0, [x2]
             })

AARCH64_TEST(Stlrb, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0xfc, 0xdf, 0x08,  // ldarb  w0, [x2]
                 0x41, 0xfc, 0x9f, 0x08,  // stlrb  w1, [x2]
                 0x40, 0xfc, 0xdf, 0x08,  // ldarb  w0, [x2]
             })

AARCH64_TEST(Stxrh, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x7c, 0x5f, 0x48,  // ldxrh  w0, [x2]
                 0x41, 0x7c, 0x00, 0x48,  // stxrh  w0, w1, [x2]
                 0x40, 0x7c, 0x5f, 0x48,  // ldxrh  w0, [x2]
             })

AARCH64_TEST(Stlxrh, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0xfc, 0x5f, 0x48,  // ldaxrh  w0, [x2]
                 0x41, 0xfc, 0x00, 0x48,  // stlxrh  w0, w1, [x2]
                 0x40, 0xfc, 0x5f, 0x48,  // ldaxrh  w0, [x2]
             })

AARCH64_TEST(Stlrh, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0xfc, 0xdf, 0x48,  // ldarh  w0, [x2]
                 0x41, 0xfc, 0x9f, 0x48,  // stlrh  w1, [x2]
                 0x40, 0xfc, 0xdf, 0x48,  // ldarh  w0, [x2]
             })

AARCH64_TEST(Stxrw, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x7c, 0x5f, 0x88,  // ldxr  w0, [x2]
                 0x41, 0x7c, 0x00, 0x88,  // stxr  w0, w1, [x2]
                 0x40, 0x7c, 0x5f, 0x88,  // ldxr  w0, [x2]
             })

AARCH64_TEST(Stxpw, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x00, 0x7f, 0x88,  // ldxp  w0, w0, [x2]
                 0x41, 0x0c, 0x20, 0x88,  // stxp  w0, w1, w3, [x2]
                 0x40, 0x04, 0x7f, 0x88,  // ldxp  w0, w1, [x2]
             })

AARCH64_TEST(Stlxpw, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x80, 0x7f, 0x88,  // ldaxp  w0, w0, [x2]
                 0x41, 0x8c, 0x20, 0x88,  // stlxp  w0, w1, w3, [x2]
                 0x40, 0x84, 0x7f, 0x88,  // ldaxp  w0, w1, [x2]
             })

AARCH64_TEST(Stlxrw, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0xfc, 0x5f, 0x88,  // ldaxr  w0, [x2]
                 0x41, 0xfc, 0x00, 0x88,  // stlxr  w0, w1, [x2]
                 0x40, 0xfc, 0x5f, 0x88,  // ldaxr  w0, [x2]
             })

AARCH64_TEST(Stlrw, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0xfc, 0xdf, 0x88,  // ldar  w0, [x2]
                 0x41, 0xfc, 0x9f, 0x88,  // stlr  w1, [x2]
                 0x40, 0xfc, 0xdf, 0x88,  // ldar  w0, [x2]
             })

AARCH64_TEST(Stxrx, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x7c, 0x5f, 0xc8,  // ldxr  x0, [x2]
                 0x41, 0x7c, 0x00, 0xc8,  // stxr  w0, x1, [x2]
                 0x40, 0x7c, 0x5f, 0xc8,  // ldxr  x0, [x2]
             })

AARCH64_TEST(Stxpx, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x00, 0x7f, 0xc8,  // ldxp  x0, x0, [x2]
                 0x41, 0x0c, 0x20, 0xc8,  // stxp  w0, x1, x3, [x2]
                 0x40, 0x04, 0x7f, 0xc8,  // ldxp  x0, x1, [x2]
             })

AARCH64_TEST(Stlxpx, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x80, 0x7f, 0xc8,  // ldaxp  x0, x0, [x2]
                 0x41, 0x8c, 0x20, 0xc8,  // stlxp  w0, x1, x3, [x2]
                 0x40, 0x84, 0x7f, 0xc8,  // ldaxp  x0, x1, [x2]
             })

AARCH64_TEST(Stlxrx, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0xfc, 0x5f, 0xc8,  // ldaxr  x0, [x2]
                 0x41, 0xfc, 0x00, 0xc8,  // stlxr  w0, x1, [x2]
                 0x40, 0xfc, 0x5f, 0xc8,  // ldaxr  x0, [x2]
             })

AARCH64_TEST(Stlrx, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0xfc, 0xdf, 0xc8,  // ldar  x0, [x2]
                 0x41, 0xfc, 0x9f, 0xc8,  // stlr  x1, [x2]
                 0x40, 0xfc, 0xdf, 0xc8,  // ldar  x0, [x2]
             })

AARCH64_TEST(LdrwLiteral, 1,
             {
                 0x20, 0x00, 0x00, 0x18,  // ldr  w0, 4 <data>
                 0x10, 0x11, 0x12, 0x13,  // junk
                 0x14, 0x15, 0x16, 0x17,  // junk
             })

AARCH64_TEST(LdrxLiteral, 1,
             {
                 0x20, 0x00, 0x00, 0x58,  // ldr  x0, 4 <data>
                 0x10, 0x11, 0x12, 0x13,  // junk
                 0x14, 0x15, 0x16, 0x17,  // junk
             })

AARCH64_TEST(LdrswLiteralPositive, 1,
             {
                 0x20, 0x00, 0x00, 0x98,  // ldrsw  x0, 4 <data>
                 0x10, 0x11, 0x12, 0x13,  // junk
                 0x14, 0x15, 0x16, 0x17,  // junk
             })

AARCH64_TEST(LdrswLiteralNegative, 1,
             {
                 0x20, 0x00, 0x00, 0x98,  // ldrsw  x0, 4 <data>
                 0x10, 0x11, 0x12, 0x83,  // junk
                 0x14, 0x15, 0x16, 0x17,  // junk
             })

/*
AARCH64_TEST(LdrsLiteral, 1,
           {
               0x40, 0x00, 0x00, 0x1c,  // ldr  s0, 8 <data>
               0x1f, 0x20, 0x03, 0xd5,  // nop
           })

AARCH64_TEST(LdrdLiteral, 1,
           {
               0x40, 0x00, 0x00, 0x5c,  // ldr  d0, 8 <data>
               0x1f, 0x20, 0x03, 0xd5,  // nop
           })

AARCH64_TEST(LdrqLiteral, 1,
           {
               0x40, 0x00, 0x00, 0x9c,  // ldr  q0, 8 <data>
               0x1f, 0x20, 0x03, 0xd5,  // nop
           })
*/

AARCH64_TEST(Ldstnpw, 3,
             {
                 0x62, 0x00, 0x00, 0x10,  // adr  x2, c <data>
                 0x40, 0x04, 0x02, 0x28,  // stnp  w0, w1, [x2,#16]
                 0x41, 0x00, 0x42, 0x28,  // ldnp  w1, w0, [x2,#16]
             })

AARCH64_TEST(Ldpsw, 3,
             {
                 0x62, 0x00, 0x00, 0x10,  // adr  x2, c <data>
                 0x40, 0x04, 0x02, 0x29,  // stp  w0, w1, [x2,#16]
                 0x40, 0x04, 0x42, 0x69,  // ldpsw  x1, x0, [x2,#16]
             })

AARCH64_TEST(Ldstnpx, 3,
             {
                 0x62, 0x00, 0x00, 0x10,  // adr  x2, c <data>
                 0x40, 0x04, 0x01, 0xa8,  // stnp  x0, x1, [x2,#16]
                 0x41, 0x00, 0x41, 0xa8,  // ldnp  x1, x0, [x2,#16]
             })

AARCH64_TEST(LdstpwPreindex, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x04, 0x81, 0x29,  // stp  w0, w1, [x2,#8]!
                 0x42, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x41, 0x00, 0xc1, 0x29,  // ldp  w1, w0, [x2,#8]!
             })

AARCH64_TEST(LdstpwOffset, 3,
             {
                 0x62, 0x00, 0x00, 0x10,  // adr  x2, c <data>
                 0x40, 0x04, 0x00, 0x29,  // stp  w0, w1, [x2]
                 0x41, 0x80, 0x7f, 0x29,  // ldp  w1, w0, [x2,#-4]
             })

AARCH64_TEST(LdstpwPostindex, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x04, 0x82, 0x28,  // stp  w0, w1, [x2],#16
                 0x42, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x41, 0x00, 0xc2, 0x28,  // ldp  w1, w0, [x2],#16
             })

AARCH64_TEST(LdstpxPreindex, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x04, 0x81, 0xa9,  // stp  x0, x1, [x2,#16]!
                 0x42, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x41, 0x00, 0xc1, 0xa9,  // ldp  x1, x0, [x2,#16]!
             })

AARCH64_TEST(LdstpxOffset, 3,
             {
                 0x62, 0x00, 0x00, 0x10,  // adr  x2, c <data>
                 0x40, 0x84, 0x00, 0xa9,  // stp  x0, x1, [x2,#8]
                 0x41, 0x00, 0x41, 0xa9,  // ldp  x1, x0, [x2,#16]
             })

AARCH64_TEST(LdstpxPostindex, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x84, 0x80, 0xa8,  // stp  x0, x1, [x2],#8
                 0x42, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x41, 0x80, 0xc0, 0xa8,  // ldp  x1, x0, [x2],#8
             })

/*
AARCH64_TEST(Ldstnps, 3,
           {
               0x60, 0x00, 0x00, 0x10,  // adr  x0, c <data>
               0x00, 0x84, 0x02, 0x2c,  // stnp s0, s1, [x0, #20]
               0x01, 0x80, 0x42, 0x2c,  // ldnp s1, s0, [x0, #20]
           })

AARCH64_TEST(Ldstnpd, 3,
           {
               0x60, 0x00, 0x00, 0x10,  // adr  x0, c <data>
               0x00, 0x84, 0x01, 0x6c,  // stnp d0, d1, [x0, #24]
               0x01, 0x80, 0x41, 0x6c,  // ldnp d1, d0, [x0, #24]
           })

AARCH64_TEST(Ldstnpq, 3,
           {
               0x60, 0x00, 0x00, 0x10,  // adr  x0, c <data>
               0x00, 0x04, 0x01, 0xac,  // stnp q0, q1, [x0, #32]
               0x01, 0x00, 0x41, 0xac,  // ldnp q1, q0, [x0, #32]
           })

AARCH64_TEST(LdstpsPreindex, 4,
           {
               0x80, 0x00, 0x00, 0x10,  // adr  x0, 10 <data>
               0x00, 0x04, 0x81, 0x2d,  // stp  s0, s1, [x0, #8]!
               0x42, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
               0x01, 0x00, 0xc1, 0x2d,  // ldp  s1, s0, [x0, #8]!
           })

AARCH64_TEST(LdstpsOffset, 3,
           {
               0x62, 0x00, 0x00, 0x10,  // adr  x2, c <data>
               0x40, 0x04, 0x00, 0x29,  // stp  w0, w1, [x2]
               0x41, 0x80, 0x7f, 0x29,  // ldp  w1, w0, [x2, #-4]
           })

AARCH64_TEST(LdstpsPostindex, 4,
           {
               0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
               0x40, 0x04, 0x82, 0x28,  // stp  w0, w1, [x2], #16
               0x42, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
               0x41, 0x00, 0xc2, 0x28,  // ldp  w1, w0, [x2], #16
           })

AARCH64_TEST(LdstpdPreindex, 4,
           {
               0x80, 0x00, 0x00, 0x10,  // adr  x0, 10 <data>
               0x00, 0x84, 0x80, 0x6d,  // stp  d0, d1, [x0, #8]!
               0x42, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
               0x01, 0x80, 0xc0, 0x6d,  // ldp  d1, d0, [x0, #8]!
           })

AARCH64_TEST(LdstpdOffset, 4,
           {
               0x1f, 0x20, 0x03, 0xd5,  // nop
               0xe0, 0x00, 0x00, 0x10,  // adr  x0, 1c <data>
               0x00, 0x04, 0x00, 0x6d,  // stp  d0, d1, [x0]
               0x01, 0x80, 0x7f, 0x6d,  // ldp  d1, d0, [x0, #-8]
           })

AARCH64_TEST(LdstpdPostindex, 4,
           {
               0x80, 0x00, 0x00, 0x10,  // adr  x0, 10 <data>
               0x00, 0x04, 0x81, 0x6c,  // stp  d0, d1, [x0], #16
               0x40, 0x00, 0x00, 0x10,  // adr  x0, 10 <data>
               0x01, 0x00, 0xc1, 0x6c,  // ldp  d1, d0, [x0], #16
           })

AARCH64_TEST(LdstpqPreindex, 4,
           {
               0x80, 0x00, 0x00, 0x10,  // adr  x0, 10 <data>
               0x00, 0x04, 0x82, 0xad,  // stp  q0, q1, [x0, #64]!
               0x40, 0x00, 0x00, 0x10,  // adr  x0, 10 <data>
               0x01, 0x00, 0xc2, 0xad,  // ldp  q1, q0, [x0, #64]!
           })

AARCH64_TEST(LdstpqOffset, 4,
           {
               0x1f, 0x20, 0x03, 0xd5,  // nop
               0xe0, 0x00, 0x00, 0x10,  // adr  x0, 1c <data>
               0x00, 0x04, 0x00, 0xad,  // stp  q0, q1, [x0]
               0x01, 0x00, 0x7f, 0xad,  // ldp  q1, q0, [x0, #-32]
           })

AARCH64_TEST(LdstpqPostindex, 4,
           {
               0x80, 0x00, 0x00, 0x10,  // adr  x0, 10 <data>
               0x00, 0x84, 0x80, 0xac,  // stp  q0, q1, [x0], #16
               0x40, 0x00, 0x00, 0x10,  // adr  x0, 10 <data>
               0x01, 0x80, 0xc0, 0xac,  // ldp  q1, q0, [x0], #16
           })
*/

AARCH64_TEST(LdstrbUnscaledImmediatePostindex, 6,
             {
                 0xc2, 0x00, 0x00, 0x10,  // adr  x2, 18 <data>
                 0x40, 0x14, 0x00, 0x38,  // strb  w0, [x2],#1
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 18 <data>
                 0x41, 0x14, 0x40, 0x38,  // ldrb  w1, [x2],#1
                 0x42, 0x00, 0x00, 0x10,  // adr  x2, 18 <data>
                 0x41, 0x14, 0xc0, 0x38,  // ldrsb  w1, [x2],#1
             })

AARCH64_TEST(LdstrhUnscaledImmediatePostindex, 6,
             {
                 0xc2, 0x00, 0x00, 0x10,  // adr  x2, 18 <data>
                 0x40, 0x74, 0x01, 0x78,  // strh  w0, [x2],#23
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 18 <data>
                 0x41, 0x84, 0x41, 0x78,  // ldrh  w1, [x2],#24
                 0x42, 0x00, 0x00, 0x10,  // adr  x2, 18 <data>
                 0x41, 0x74, 0xc1, 0x78,  // ldrsh  w1, [x2],#23
             })

AARCH64_TEST(LdstrwUnscaledImmediatePostindex, 6,
             {
                 0x42, 0x02, 0x00, 0x10,  // adr  x2, 48 <data>
                 0x40, 0x44, 0x1f, 0xb8,  // str  w0, [x2],#-12
                 0x02, 0x02, 0x00, 0x10,  // adr  x2, 48 <data>
                 0x41, 0x44, 0x5f, 0xb8,  // ldr  w1, [x2],#-12
                 0xc2, 0x01, 0x00, 0x10,  // adr  x2, 48 <data>
                 0x41, 0x44, 0x9f, 0xb8,  // ldrsw  x1, [x2],#-12
             })

AARCH64_TEST(LdstrxUnscaledImmediatePostindex, 4,
             {
                 0x82, 0x01, 0x00, 0x10,  // adr  x2, 30 <data>
                 0x40, 0xf4, 0x1f, 0xf8,  // str  x0, [x2],#-1
                 0x42, 0x01, 0x00, 0x10,  // adr  x2, 30 <data>
                 0x41, 0xd4, 0x5f, 0xf8,  // ldr  x1, [x2],#-3
             })

AARCH64_TEST(LdstrbUnscaledImmediatePreindex, 8,
             {
                 0xc2, 0x01, 0x00, 0x10,  // adr  x2, 38 <data>
                 0x40, 0xfc, 0x1f, 0x38,  // strb  w0, [x2,#-1]!
                 0x82, 0x01, 0x00, 0x10,  // adr  x2, 38 <data>
                 0x41, 0xfc, 0x5f, 0x38,  // ldrb  w1, [x2,#-1]!
                 0x42, 0x01, 0x00, 0x10,  // adr  x2, 38 <data>
                 0x41, 0xfc, 0xdf, 0x38,  // ldrsb  w1, [x2,#-1]!
                 0x02, 0x01, 0x00, 0x10,  // adr  x2, 38 <data>
                 0x41, 0xfc, 0x9f, 0x38,  // ldrsb  x1, [x2,#-1]!
             })

AARCH64_TEST(LdstrhUnscaledImmediatePreindex, 8,
             {
                 0xc2, 0x01, 0x00, 0x10,  // adr  x2, 38 <data>
                 0x40, 0xfc, 0x1f, 0x78,  // strh  w0, [x2,#-1]!
                 0x82, 0x01, 0x00, 0x10,  // adr  x2, 38 <data>
                 0x41, 0xfc, 0x5f, 0x78,  // ldrh  w1, [x2,#-1]!
                 0x42, 0x01, 0x00, 0x10,  // adr  x2, 38 <data>
                 0x41, 0xfc, 0xdf, 0x78,  // ldrsh  w1, [x2,#-1]!
                 0x02, 0x01, 0x00, 0x10,  // adr  x2, 38 <data>
                 0x41, 0xfc, 0x9f, 0x78,  // ldrsh  x1, [x2,#-1]!
             })

AARCH64_TEST(LdstrwUnscaledImmediatePreindex, 6,
             {
                 0x82, 0x01, 0x00, 0x10,  // adr  x2, 30 <data>
                 0x40, 0x4c, 0x1f, 0xb8,  // str  w0, [x2,#-12]!
                 0x42, 0x01, 0x00, 0x10,  // adr  x2, 30 <data>
                 0x41, 0x6c, 0x5f, 0xb8,  // ldr  w1, [x2,#-10]!
                 0x02, 0x01, 0x00, 0x10,  // adr  x2, 30 <data>
                 0x41, 0x4c, 0x9f, 0xb8,  // ldrsw  x1, [x2,#-12]!
             })

AARCH64_TEST(LdstrxUnscaledImmediatePreindex, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x7c, 0x00, 0xf8,  // str  x0, [x2,#7]!
                 0x42, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x41, 0x9c, 0x40, 0xf8,  // ldr  x1, [x2,#9]!
             })

AARCH64_TEST(LdsturbUnscaledImmediate, 5,
             {
                 0xa2, 0x00, 0x00, 0x10,  // adr  x2, 14 <data>
                 0x40, 0x00, 0x00, 0x38,  // sturb  w0, [x2]
                 0x41, 0x00, 0x40, 0x38,  // ldurb  w1, [x2]
                 0x41, 0x00, 0xc0, 0x38,  // ldursb  w1, [x2]
                 0x41, 0x00, 0x80, 0x38,  // ldursb  x1, [x2]
             })

AARCH64_TEST(LdsturhUnscaledImmediate, 5,
             {
                 0xa2, 0x00, 0x00, 0x10,  // adr  x2, 14 <data>
                 0x40, 0x10, 0x01, 0x78,  // strh  w0, [x2,#17]
                 0x41, 0x20, 0x41, 0x78,  // ldurh  w1, [x2,#18]
                 0x41, 0x10, 0xc1, 0x78,  // ldrsh  w1, [x2,#17]
                 0x41, 0x10, 0x81, 0x78,  // ldrsh  x1, [x2,#17]
             })

AARCH64_TEST(LdsturwUnscaledImmediate, 4,
             {
                 0x82, 0x00, 0x00, 0x10,  // adr  x2, 10 <data>
                 0x40, 0x10, 0x01, 0xb8,  // str  w0, [x2,#17]
                 0x41, 0x30, 0x41, 0xb8,  // ldr  w1, [x2,#19]
                 0x41, 0x10, 0x81, 0xb8,  // ldrsw  x1, [x2,#17]
             })

AARCH64_TEST(LdsturxUnscaledImmediate, 3,
             {
                 0xa2, 0x00, 0x00, 0x10,  // adr  x2, 14 <data>
                 0x40, 0x80, 0x1f, 0xf8,  // str  x0, [x2,#-8]
                 0x41, 0xa0, 0x5f, 0xf8,  // ldr  x1, [x2,#-6]
                 0x1f, 0x20, 0x03, 0xd5,  // nop
                 0x1f, 0x20, 0x03, 0xd5,  // nop
             })

AARCH64_TEST(LdsttrbUnscaledImmediate, 5,
             {
                 0xa2, 0x00, 0x00, 0x10,  // adr  x2, 14 <data>
                 0x40, 0x08, 0x00, 0x38,  // sttrb  w0, [x2]
                 0x41, 0x08, 0x40, 0x38,  // ldtrb  w1, [x2]
                 0x41, 0x08, 0xc0, 0x38,  // ldtrsb  w1, [x2]
                 0x41, 0x08, 0x80, 0x38,  // ldtrsb  x1, [x2]
             })

AARCH64_TEST(LdsttrhUnscaledImmediate, 5,
             {
                 0xc2, 0x01, 0x00, 0x10,  // adr  x2, 38 <data>
                 0x40, 0xd8, 0x1e, 0x78,  // sttrh  w0, [x2,#-19]
                 0x41, 0xe8, 0x5e, 0x78,  // ldtrh  w1, [x2,#-18]
                 0x41, 0xd8, 0xde, 0x78,  // ldtrsh  w1, [x2,#-19]
                 0x41, 0xd8, 0x9e, 0x78,  // ldtrsh  x1, [x2,#-19]
             })

AARCH64_TEST(LdsttrwUnscaledImmediate, 4,
             {
                 0xa2, 0x01, 0x00, 0x10,  // adr  x2, 34 <data>
                 0x40, 0xd8, 0x1e, 0xb8,  // sttr  w0, [x2,#-19]
                 0x41, 0xe8, 0x5e, 0xb8,  // ldtr  w1, [x2,#-18]
                 0x41, 0xd8, 0x9e, 0xb8,  // ldtrsw  x1, [x2,#-19]
             })

AARCH64_TEST(LdsttrxUnscaledImmediate, 3,
             {
                 0x82, 0x01, 0x00, 0x10,  // adr  x2, 30 <data>
                 0x40, 0x18, 0x00, 0xf8,  // sttr  x0, [x2,#1]
                 0x41, 0x18, 0x40, 0xf8,  // ldtr  x1, [x2,#1]
             })

AARCH64_TEST(LdstrbUnscaledImmediate, 5,
             {
                 0x01, 0x00, 0x00, 0x10,  // adr  x1, 0 <data>
                 0x21, 0x00, 0x3c, 0xd1,  // sub  x1, x1, #0xf00
                 0x20, 0x00, 0x3c, 0x39,  // strb  w0, [x1,#3840]
                 0x20, 0x00, 0x7c, 0x39,  // ldrb  w0, [x1,#3840]
                 0x20, 0x00, 0xfc, 0x39,  // ldrsb  w0, [x1,#3840]
             })

AARCH64_TEST(LdstrhUnscaledImmediate, 5,
             {
                 0x01, 0x00, 0x00, 0x10,  // adr  x1, 0 <data>
                 0x21, 0x00, 0x3c, 0xd1,  // sub  x1, x1, #0xf00
                 0x20, 0x00, 0x1e, 0x79,  // strh  w0, [x1,#3840]
                 0x20, 0x00, 0x5e, 0x79,  // ldrh  w0, [x1,#3840]
                 0x20, 0x00, 0xde, 0x79,  // ldrsh  w0, [x1,#3840]
             })

AARCH64_TEST(LdstrwUnscaledImmediate, 5,
             {
                 0x01, 0x00, 0x00, 0x10,  // adr  x1, 0 <data>
                 0x21, 0x00, 0x3c, 0xd1,  // sub  x1, x1, #0xf00
                 0x20, 0x00, 0x0f, 0xb9,  // str  w0, [x1,#3840]
                 0x20, 0x00, 0x4f, 0xb9,  // ldr  w0, [x1,#3840]
                 0x20, 0x00, 0x8f, 0xb9,  // ldrsw  x0, [x1,#3840]
             })

AARCH64_TEST(LdstrxUnscaledImmediate, 4,
             {
                 0x01, 0x00, 0x00, 0x10,  // adr  x1, 0 <data>
                 0x21, 0x00, 0x3c, 0xd1,  // sub  x1, x1, #0xf00
                 0x20, 0x80, 0x07, 0xf9,  // str  x0, [x1,#3840]
                 0x20, 0x80, 0x47, 0xf9,  // ldr  x0, [x1,#3840]
             })

AARCH64_TEST(LdstrbRegisterOffset, 10,
             {
                 0x24, 0x00, 0x80, 0x52,  // mov  w4, #0x1
                 0x05, 0x00, 0x80, 0x12,  // mov  w5, #0xffffffff
                 0x06, 0x02, 0x80, 0x52,  // mov  w6, #0x10
                 0xa7, 0x01, 0x00, 0x10,  // adr  x7, 44 <data>
                 0xe0, 0x48, 0x24, 0x38,  // strb  w0, [x7,w4,uxtw]
                 0xe1, 0xc8, 0x25, 0x38,  // strb  w1, [x7,w5,sxtw]
                 0xe2, 0xe8, 0x26, 0x38,  // strb  w2, [x7,x6,sxtx]
                 0xe0, 0x48, 0x64, 0x38,  // ldrb  w0, [x7,w4,uxtw]
                 0xe1, 0xc8, 0x65, 0x38,  // ldrb  w1, [x7,w5,sxtw]
                 0xe2, 0xe8, 0x66, 0x38,  // ldrb  w2, [x7,x6,sxtx]
             })

// TODO: test the sxtx variants on hardware.
AARCH64_TEST(LdstrhRegisterOffset, 11,
             {
                 0x24, 0x00, 0x80, 0x52,  // mov  w4, #0x1
                 0x05, 0x01, 0x80, 0x52,  // mov  w5, #0x8
                 0x06, 0x02, 0x80, 0x52,  // mov  w6, #0x10
                 0x07, 0x00, 0x80, 0x12,  // mov  w7, #0xffffffff
                 0xa8, 0x01, 0x00, 0x10,  // adr  x8, 44 <data>
                 0x00, 0x49, 0x24, 0x78,  // strh  w0, [x8,w4,uxtw]
                 0x00, 0x79, 0x25, 0x78,  // strh  w0, [x8,x5,lsl #1]
                 0x01, 0xd9, 0x26, 0x78,  // strh  w1, [x8,w6,sxtw #1]
                 // 0x02, 0xe9, 0x27, 0x78, // strh  w2, [x8,x7,sxtx]
                 0x00, 0x49, 0x64, 0x78,  // ldrh  w0, [x8,w4,uxtw]
                 0x00, 0x79, 0x25, 0x78,  // strh  w0, [x8,x5,lsl #1]
                 0x01, 0xd9, 0x66, 0x78,  // ldrh  w1, [x8,w6,sxtw #1]
                 // 0x02, 0xe9, 0x67, 0x78, // ldrh  w2, [x8,x7,sxtx]
             })

AARCH64_TEST(LdstrwRegisterOffset, 11,
             {
                 0x24, 0x00, 0x80, 0x52,  // mov  w4, #0x1
                 0x05, 0x01, 0x80, 0x52,  // mov  w5, #0x8
                 0x06, 0x02, 0x80, 0x52,  // mov  w6, #0x10
                 0x07, 0x00, 0x80, 0x12,  // mov  w7, #0xffffffff
                 0xa8, 0x01, 0x00, 0x10,  // adr  x8, 44 <data>
                 0x00, 0x59, 0x24, 0xb8,  // str  w0, [x8,w4,uxtw #2]
                 0x00, 0x79, 0x25, 0xb8,  // str  w0, [x8,x5,lsl #2]
                 0x01, 0xc9, 0x26, 0xb8,  // str  w1, [x8,w6,sxtw]
                 // 0x02, 0xe9, 0x27, 0xb8, // str  w2, [x8,x7,sxtx]
                 0x00, 0x59, 0x64, 0xb8,  // ldr  w0, [x8,w4,uxtw #2]
                 0x00, 0x79, 0x65, 0xb8,  // ldr  w0, [x8,x5,lsl #2]
                 0x01, 0xc9, 0x66, 0xb8,  // ldr  w1, [x8,w6,sxtw]
                 // 0x02, 0xe9, 0x67, 0xb8, // ldr  w2, [x8,x7,sxtx]
             })

AARCH64_TEST(LdstrxRegisterOffset, 11,
             {
                 0x24, 0x00, 0x80, 0x52,  // mov  w4, #0x1
                 0x05, 0x01, 0x80, 0x52,  // mov  w5, #0x8
                 0x06, 0x02, 0x80, 0x52,  // mov  w6, #0x10
                 0x07, 0x00, 0x80, 0x12,  // mov  w7, #0xffffffff
                 0x28, 0x02, 0x00, 0x10,  // adr  x8, 54 <data>
                 0x00, 0x59, 0x24, 0xf8,  // str  x0, [x8,w4,uxtw #3]
                 0x00, 0x79, 0x25, 0xf8,  // str  x0, [x8,x5,lsl #3]
                 0x01, 0xc9, 0x26, 0xf8,  // str  x1, [x8,w6,sxtw]
                 // 0x02, 0xe9, 0x27, 0xf8, // str  x2, [x8,x7,sxtx]
                 0x00, 0x59, 0x64, 0xf8,  // ldr  x0, [x8,w4,uxtw #3]
                 0x00, 0x79, 0x65, 0xb8,  // ldr  w0, [x8,x5,lsl #2]
                 0x01, 0xc9, 0x66, 0xb8,  // ldr  w1, [x8,w6,sxtw]
                 // 0x02, 0xe9, 0x67, 0xf8, // ldr  x2, [x8,x7,sxtx]
             })

AARCH64_TEST(Adc, 2,
             {
                 0x22, 0x00, 0x00, 0x9a,  // adc  x2, x1, x0
                 0x22, 0x00, 0x00, 0x1a,  // adc  w2, w1, w0
             })

AARCH64_TEST(Adcs, 2,
             {
                 0x22, 0x00, 0x00, 0xba,  // adcs  x2, x1, x0
                 0x22, 0x00, 0x00, 0x3a,  // adcs  w2, w1, w0
             })

AARCH64_TEST(Sbc, 2,
             {
                 0x22, 0x00, 0x00, 0xda,  // sbc  x2, x1, x0
                 0x22, 0x00, 0x00, 0x5a,  // sbc  w2, w1, w0
             })

AARCH64_TEST(Sbcs, 2,
             {
                 0x22, 0x00, 0x00, 0xfa,  // sbcs  x2, x1, x0
                 0x22, 0x00, 0x00, 0x7a,  // sbcs  w2, w1, w0
             })

AARCH64_TEST(CcmpRegister, 14,
             {
                 0x69, 0x00, 0x42, 0xfa,  // ccmp  x3, x2, #0x9, eq
                 0x68, 0x10, 0x42, 0x7a,  // ccmp  w3, w2, #0x8, ne
                 0x6b, 0x20, 0x42, 0xfa,  // ccmp  x3, x2, #0xb, cs
                 0x6a, 0x30, 0x42, 0x7a,  // ccmp  w3, w2, #0xa, cc
                 0x6f, 0x40, 0x42, 0xfa,  // ccmp  x3, x2, #0xf, mi
                 0x6e, 0x50, 0x42, 0x7a,  // ccmp  w3, w2, #0xe, pl
                 0x66, 0x60, 0x42, 0xfa,  // ccmp  x3, x2, #0x6, vs
                 0x6d, 0x70, 0x42, 0x7a,  // ccmp  w3, w2, #0xd, vc
                 0x6c, 0x80, 0x42, 0xfa,  // ccmp  x3, x2, #0xc, hi
                 0x65, 0x90, 0x42, 0x7a,  // ccmp  w3, w2, #0x5, ls
                 0x60, 0xa0, 0x42, 0xfa,  // ccmp  x3, x2, #0x0, ge
                 0x61, 0xb0, 0x42, 0x7a,  // ccmp  w3, w2, #0x1, lt
                 0x63, 0xc0, 0x42, 0xfa,  // ccmp  x3, x2, #0x3, gt
                 0x67, 0xd0, 0x42, 0x7a,  // ccmp  w3, w2, #0x7, le
             })

AARCH64_TEST(CcmnRegister, 14,
             {
                 0x69, 0x00, 0x42, 0xba,  // ccmn  x3, x2, #0x9, eq
                 0x68, 0x10, 0x42, 0x3a,  // ccmn  w3, w2, #0x8, ne
                 0x6b, 0x20, 0x42, 0xba,  // ccmn  x3, x2, #0xb, cs
                 0x6a, 0x30, 0x42, 0x3a,  // ccmn  w3, w2, #0xa, cc
                 0x6f, 0x40, 0x42, 0xba,  // ccmn  x3, x2, #0xf, mi
                 0x6e, 0x50, 0x42, 0x3a,  // ccmn  w3, w2, #0xe, pl
                 0x66, 0x60, 0x42, 0xba,  // ccmn  x3, x2, #0x6, vs
                 0x6d, 0x70, 0x42, 0x3a,  // ccmn  w3, w2, #0xd, vc
                 0x6c, 0x80, 0x42, 0xba,  // ccmn  x3, x2, #0xc, hi
                 0x65, 0x90, 0x42, 0x3a,  // ccmn  w3, w2, #0x5, ls
                 0x60, 0xa0, 0x42, 0xba,  // ccmn  x3, x2, #0x0, ge
                 0x61, 0xb0, 0x42, 0x3a,  // ccmn  w3, w2, #0x1, lt
                 0x63, 0xc0, 0x42, 0xba,  // ccmn  x3, x2, #0x3, gt
                 0x67, 0xd0, 0x42, 0x3a,  // ccmn  w3, w2, #0x7, le
             })

AARCH64_TEST(CcmpImmediate, 14,
             {
                 0x09, 0x08, 0x5f, 0xfa,  // ccmp  x0, #0x1f, #0x9, eq
                 0x08, 0x18, 0x5e, 0x7a,  // ccmp  w0, #0x1e, #0x8, ne
                 0x0b, 0x28, 0x5d, 0xfa,  // ccmp  x0, #0x1d, #0xb, cs
                 0x0a, 0x38, 0x5c, 0x7a,  // ccmp  w0, #0x1c, #0xa, cc
                 0x0f, 0x48, 0x5b, 0xfa,  // ccmp  x0, #0x1b, #0xf, mi
                 0x0e, 0x58, 0x5a, 0x7a,  // ccmp  w0, #0x1a, #0xe, pl
                 0x06, 0x68, 0x59, 0xfa,  // ccmp  x0, #0x19, #0x6, vs
                 0x0d, 0x78, 0x57, 0x7a,  // ccmp  w0, #0x17, #0xd, vc
                 0x0c, 0x88, 0x55, 0xfa,  // ccmp  x0, #0x15, #0xc, hi
                 0x05, 0x98, 0x54, 0x7a,  // ccmp  w0, #0x14, #0x5, ls
                 0x00, 0xa8, 0x53, 0xfa,  // ccmp  x0, #0x13, #0x0, ge
                 0x01, 0xb8, 0x52, 0x7a,  // ccmp  w0, #0x12, #0x1, lt
                 0x03, 0xc8, 0x41, 0xfa,  // ccmp  x0, #0x1, #0x3, gt
                 0x07, 0xd8, 0x40, 0x7a,  // ccmp  w0, #0x0, #0x7, le
             })

AARCH64_TEST(CcmnImmediate, 14,
             {
                 0x09, 0x08, 0x5f, 0xba,  // ccmn  x0, #0x1f, #0x9, eq
                 0x08, 0x18, 0x5e, 0x3a,  // ccmn  w0, #0x1e, #0x8, ne
                 0x0b, 0x28, 0x5d, 0xba,  // ccmn  x0, #0x1d, #0xb, cs
                 0x0a, 0x38, 0x5c, 0x3a,  // ccmn  w0, #0x1c, #0xa, cc
                 0x0f, 0x48, 0x5b, 0xba,  // ccmn  x0, #0x1b, #0xf, mi
                 0x0e, 0x58, 0x5a, 0x3a,  // ccmn  w0, #0x1a, #0xe, pl
                 0x06, 0x68, 0x59, 0xba,  // ccmn  x0, #0x19, #0x6, vs
                 0x0d, 0x78, 0x57, 0x3a,  // ccmn  w0, #0x17, #0xd, vc
                 0x0c, 0x88, 0x55, 0xba,  // ccmn  x0, #0x15, #0xc, hi
                 0x05, 0x98, 0x54, 0x3a,  // ccmn  w0, #0x14, #0x5, ls
                 0x00, 0xa8, 0x53, 0xba,  // ccmn  x0, #0x13, #0x0, ge
                 0x01, 0xb8, 0x52, 0x3a,  // ccmn  w0, #0x12, #0x1, lt
                 0x03, 0xc8, 0x41, 0xba,  // ccmn  x0, #0x1, #0x3, gt
                 0x07, 0xd8, 0x40, 0x3a,  // ccmn  w0, #0x0, #0x7, le
             })

AARCH64_TEST(Csel, 15,
             {
                 0x22, 0x00, 0x80, 0x9a,  // csel  x2, x1, x0, eq
                 0x22, 0x10, 0x80, 0x1a,  // csel  w2, w1, w0, ne
                 0x22, 0x20, 0x80, 0x9a,  // csel  x2, x1, x0, cs
                 0x22, 0x30, 0x80, 0x1a,  // csel  w2, w1, w0, cc
                 0x22, 0x40, 0x80, 0x9a,  // csel  x2, x1, x0, mi
                 0x22, 0x50, 0x80, 0x1a,  // csel  w2, w1, w0, pl
                 0x22, 0x60, 0x80, 0x9a,  // csel  x2, x1, x0, vs
                 0x22, 0x70, 0x80, 0x1a,  // csel  w2, w1, w0, vc
                 0x22, 0x80, 0x80, 0x9a,  // csel  x2, x1, x0, hi
                 0x22, 0x90, 0x80, 0x1a,  // csel  w2, w1, w0, ls
                 0x22, 0xa0, 0x80, 0x9a,  // csel  x2, x1, x0, ge
                 0x22, 0xb0, 0x80, 0x1a,  // csel  w2, w1, w0, lt
                 0x22, 0xc0, 0x80, 0x9a,  // csel  x2, x1, x0, gt
                 0x22, 0xd0, 0x80, 0x1a,  // csel  w2, w1, w0, le
                 0x22, 0xe0, 0x80, 0x9a,  // csel  x2, x1, x0, al
             })

AARCH64_TEST(Csinc, 15,
             {
                 0x22, 0x04, 0x80, 0x9a,  // csinc  x2, x1, x0, eq
                 0x22, 0x14, 0x80, 0x1a,  // csinc  w2, w1, w0, ne
                 0x22, 0x24, 0x80, 0x9a,  // csinc  x2, x1, x0, cs
                 0x22, 0x34, 0x80, 0x1a,  // csinc  w2, w1, w0, cc
                 0x22, 0x44, 0x80, 0x9a,  // csinc  x2, x1, x0, mi
                 0x22, 0x54, 0x80, 0x1a,  // csinc  w2, w1, w0, pl
                 0x22, 0x64, 0x80, 0x9a,  // csinc  x2, x1, x0, vs
                 0x22, 0x74, 0x80, 0x1a,  // csinc  w2, w1, w0, vc
                 0x22, 0x84, 0x80, 0x9a,  // csinc  x2, x1, x0, hi
                 0x22, 0x94, 0x80, 0x1a,  // csinc  w2, w1, w0, ls
                 0x22, 0xa4, 0x80, 0x9a,  // csinc  x2, x1, x0, ge
                 0x22, 0xb4, 0x80, 0x1a,  // csinc  w2, w1, w0, lt
                 0x22, 0xc4, 0x80, 0x9a,  // csinc  x2, x1, x0, gt
                 0x22, 0xd4, 0x80, 0x1a,  // csinc  w2, w1, w0, le
                 0x22, 0xe4, 0x80, 0x9a,  // csinc  x2, x1, x0, al
             })

AARCH64_TEST(Csinv, 15,
             {
                 0x22, 0x00, 0x80, 0xda,  // csinv  x2, x1, x0, eq
                 0x22, 0x10, 0x80, 0x5a,  // csinv  w2, w1, w0, ne
                 0x22, 0x20, 0x80, 0xda,  // csinv  x2, x1, x0, cs
                 0x22, 0x30, 0x80, 0x5a,  // csinv  w2, w1, w0, cc
                 0x22, 0x40, 0x80, 0xda,  // csinv  x2, x1, x0, mi
                 0x22, 0x50, 0x80, 0x5a,  // csinv  w2, w1, w0, pl
                 0x22, 0x60, 0x80, 0xda,  // csinv  x2, x1, x0, vs
                 0x22, 0x70, 0x80, 0x5a,  // csinv  w2, w1, w0, vc
                 0x22, 0x80, 0x80, 0xda,  // csinv  x2, x1, x0, hi
                 0x22, 0x90, 0x80, 0x5a,  // csinv  w2, w1, w0, ls
                 0x22, 0xa0, 0x80, 0xda,  // csinv  x2, x1, x0, ge
                 0x22, 0xb0, 0x80, 0x5a,  // csinv  w2, w1, w0, lt
                 0x22, 0xc0, 0x80, 0xda,  // csinv  x2, x1, x0, gt
                 0x22, 0xd0, 0x80, 0x5a,  // csinv  w2, w1, w0, le
                 0x22, 0xe0, 0x80, 0xda,  // csinv  x2, x1, x0, al
             })

AARCH64_TEST(Csneg, 15,
             {
                 0x22, 0x04, 0x80, 0xda,  // csneg  x2, x1, x0, eq
                 0x22, 0x14, 0x80, 0x5a,  // csneg  w2, w1, w0, ne
                 0x22, 0x24, 0x80, 0xda,  // csneg  x2, x1, x0, cs
                 0x22, 0x34, 0x80, 0x5a,  // csneg  w2, w1, w0, cc
                 0x22, 0x44, 0x80, 0xda,  // csneg  x2, x1, x0, mi
                 0x22, 0x54, 0x80, 0x5a,  // csneg  w2, w1, w0, pl
                 0x22, 0x64, 0x80, 0xda,  // csneg  x2, x1, x0, vs
                 0x22, 0x74, 0x80, 0x5a,  // csneg  w2, w1, w0, vc
                 0x22, 0x84, 0x80, 0xda,  // csneg  x2, x1, x0, hi
                 0x22, 0x94, 0x80, 0x5a,  // csneg  w2, w1, w0, ls
                 0x22, 0xa4, 0x80, 0xda,  // csneg  x2, x1, x0, ge
                 0x22, 0xb4, 0x80, 0x5a,  // csneg  w2, w1, w0, lt
                 0x22, 0xc4, 0x80, 0xda,  // csneg  x2, x1, x0, gt
                 0x22, 0xd4, 0x80, 0x5a,  // csneg  w2, w1, w0, le
                 0x22, 0xe4, 0x80, 0xda,  // csneg  x2, x1, x0, al
             })

AARCH64_TEST(Cset, 14,
             {
                 0xe2, 0x17, 0x9f, 0x9a,  // cset  x2, eq
                 0xe2, 0x07, 0x9f, 0x1a,  // cset  w2, ne
                 0xe2, 0x37, 0x9f, 0x9a,  // cset  x2, cs
                 0xe2, 0x27, 0x9f, 0x1a,  // cset  w2, cc
                 0xe2, 0x57, 0x9f, 0x9a,  // cset  x2, mi
                 0xe2, 0x47, 0x9f, 0x1a,  // cset  w2, pl
                 0xe2, 0x77, 0x9f, 0x9a,  // cset  x2, vs
                 0xe2, 0x67, 0x9f, 0x1a,  // cset  w2, vc
                 0xe2, 0x97, 0x9f, 0x9a,  // cset  x2, hi
                 0xe2, 0x87, 0x9f, 0x1a,  // cset  w2, ls
                 0xe2, 0xb7, 0x9f, 0x9a,  // cset  x2, ge
                 0xe2, 0xa7, 0x9f, 0x1a,  // cset  w2, lt
                 0xe2, 0xd7, 0x9f, 0x9a,  // cset  x2, gt
                 0xe2, 0xc7, 0x9f, 0x1a,  // cset  w2, le
             })

AARCH64_TEST(Csetm, 14,
             {
                 0xe2, 0x13, 0x9f, 0xda,  // csetm  x2, eq
                 0xe2, 0x03, 0x9f, 0x5a,  // csetm  w2, ne
                 0xe2, 0x33, 0x9f, 0xda,  // csetm  x2, cs
                 0xe2, 0x23, 0x9f, 0x5a,  // csetm  w2, cc
                 0xe2, 0x53, 0x9f, 0xda,  // csetm  x2, mi
                 0xe2, 0x43, 0x9f, 0x5a,  // csetm  w2, pl
                 0xe2, 0x73, 0x9f, 0xda,  // csetm  x2, vs
                 0xe2, 0x63, 0x9f, 0x5a,  // csetm  w2, vc
                 0xe2, 0x93, 0x9f, 0xda,  // csetm  x2, hi
                 0xe2, 0x83, 0x9f, 0x5a,  // csetm  w2, ls
                 0xe2, 0xb3, 0x9f, 0xda,  // csetm  x2, ge
                 0xe2, 0xa3, 0x9f, 0x5a,  // csetm  w2, lt
                 0xe2, 0xd3, 0x9f, 0xda,  // csetm  x2, gt
                 0xe2, 0xc3, 0x9f, 0x5a,  // csetm  w2, le
             })

AARCH64_TEST(Cinc, 14,
             {
                 0x01, 0x14, 0x80, 0x9a,  // cinc  x1, x0, eq
                 0x01, 0x04, 0x80, 0x1a,  // cinc  w1, w0, ne
                 0x01, 0x34, 0x80, 0x9a,  // cinc  x1, x0, cs
                 0x01, 0x24, 0x80, 0x1a,  // cinc  w1, w0, cc
                 0x01, 0x54, 0x80, 0x9a,  // cinc  x1, x0, mi
                 0x01, 0x44, 0x80, 0x1a,  // cinc  w1, w0, pl
                 0x01, 0x74, 0x80, 0x9a,  // cinc  x1, x0, vs
                 0x01, 0x64, 0x80, 0x1a,  // cinc  w1, w0, vc
                 0x01, 0x94, 0x80, 0x9a,  // cinc  x1, x0, hi
                 0x01, 0x84, 0x80, 0x1a,  // cinc  w1, w0, ls
                 0x01, 0xb4, 0x80, 0x9a,  // cinc  x1, x0, ge
                 0x01, 0xa4, 0x80, 0x1a,  // cinc  w1, w0, lt
                 0x01, 0xd4, 0x80, 0x9a,  // cinc  x1, x0, gt
                 0x01, 0xc4, 0x80, 0x1a,  // cinc  w1, w0, le
             })

AARCH64_TEST(Cinv, 14,
             {
                 0x01, 0x10, 0x80, 0xda,  // cinv  x1, x0, eq
                 0x01, 0x00, 0x80, 0x5a,  // cinv  w1, w0, ne
                 0x01, 0x30, 0x80, 0xda,  // cinv  x1, x0, cs
                 0x01, 0x20, 0x80, 0x5a,  // cinv  w1, w0, cc
                 0x01, 0x50, 0x80, 0xda,  // cinv  x1, x0, mi
                 0x01, 0x40, 0x80, 0x5a,  // cinv  w1, w0, pl
                 0x01, 0x70, 0x80, 0xda,  // cinv  x1, x0, vs
                 0x01, 0x60, 0x80, 0x5a,  // cinv  w1, w0, vc
                 0x01, 0x90, 0x80, 0xda,  // cinv  x1, x0, hi
                 0x01, 0x80, 0x80, 0x5a,  // cinv  w1, w0, ls
                 0x01, 0xb0, 0x80, 0xda,  // cinv  x1, x0, ge
                 0x01, 0xa0, 0x80, 0x5a,  // cinv  w1, w0, lt
                 0x01, 0xd0, 0x80, 0xda,  // cinv  x1, x0, gt
                 0x01, 0xc0, 0x80, 0x5a,  // cinv  w1, w0, le
             })

AARCH64_TEST(Cneg, 14,
             {
                 0x01, 0x14, 0x80, 0xda,  // cneg  x1, x0, eq
                 0x01, 0x04, 0x80, 0x5a,  // cneg  w1, w0, ne
                 0x01, 0x34, 0x80, 0xda,  // cneg  x1, x0, cs
                 0x01, 0x24, 0x80, 0x5a,  // cneg  w1, w0, cc
                 0x01, 0x54, 0x80, 0xda,  // cneg  x1, x0, mi
                 0x01, 0x44, 0x80, 0x5a,  // cneg  w1, w0, pl
                 0x01, 0x74, 0x80, 0xda,  // cneg  x1, x0, vs
                 0x01, 0x64, 0x80, 0x5a,  // cneg  w1, w0, vc
                 0x01, 0x94, 0x80, 0xda,  // cneg  x1, x0, hi
                 0x01, 0x84, 0x80, 0x5a,  // cneg  w1, w0, ls
                 0x01, 0xb4, 0x80, 0xda,  // cneg  x1, x0, ge
                 0x01, 0xa4, 0x80, 0x5a,  // cneg  w1, w0, lt
                 0x01, 0xd4, 0x80, 0xda,  // cneg  x1, x0, gt
                 0x01, 0xc4, 0x80, 0x5a,  // cneg  w1, w0, le
             })

AARCH64_TEST(Sdiv, 2,
             {
                 0x22, 0x0c, 0xc0, 0x9a,  // sdiv  x2, x1, x0
                 0x22, 0x0c, 0xc0, 0x1a,  // sdiv  w2, w1, w0
             })

AARCH64_TEST(Udiv, 2,
             {
                 0x22, 0x08, 0xc0, 0x9a,  // udiv  x2, x1, x0
                 0x22, 0x08, 0xc0, 0x1a,  // udiv  w2, w1, w0
             })

AARCH64_TEST(Lslv, 2,
             {
                 0x22, 0x20, 0xc0, 0x9a,  // lsl  x2, x1, x0
                 0x22, 0x20, 0xc0, 0x1a,  // lsl  w2, w1, w0
             })

AARCH64_TEST(Lsrv, 2,
             {
                 0x22, 0x24, 0xc0, 0x9a,  // lsr  x2, x1, x0
                 0x22, 0x24, 0xc0, 0x1a,  // lsr  w2, w1, w0
             })

AARCH64_TEST(Asrv, 2,
             {
                 0x22, 0x28, 0xc0, 0x9a,  // asr  x2, x1, x0
                 0x22, 0x28, 0xc0, 0x1a,  // asr  w2, w1, w0
             })

AARCH64_TEST(Rorv, 2,
             {
                 0x22, 0x2c, 0xc0, 0x9a,  // ror  x2, x1, x0
                 0x22, 0x2c, 0xc0, 0x1a,  // ror  w2, w1, w0
             })

AARCH64_TEST(Rbit, 2,
             {
                 0x01, 0x00, 0xc0, 0xda,  // rbit  x1, x0
                 0x01, 0x00, 0xc0, 0x5a,  // rbit  w1, w0
             })

AARCH64_TEST(Rev16, 2,
             {
                 0x01, 0x04, 0xc0, 0xda,  // rev16  x1, x0
                 0x01, 0x04, 0xc0, 0x5a,  // rev16  w1, w0
             })

AARCH64_TEST(Rev32, 1,
             {
                 0x01, 0x08, 0xc0, 0xda,  // rev32  x1, x0
             })

AARCH64_TEST(Rev, 2,
             {
                 0x01, 0x0c, 0xc0, 0xda,  // rev  x1, x0
                 0x01, 0x08, 0xc0, 0x5a,  // rev  w1, w0
             })

AARCH64_TEST(Clz, 5,
             {
                 0x01, 0x10, 0xc0, 0xda,  // clz  x1, x0
                 0x01, 0x10, 0xc0, 0x5a,  // clz  w1, w0
                 0xe0, 0x03, 0x1f, 0xaa,  // mov  x0, xzr
                 0x01, 0x10, 0xc0, 0xda,  // clz  x1, x0
                 0x01, 0x10, 0xc0, 0x5a,  // clz  w1, w0
             })

AARCH64_TEST(Cls, 2,
             {
                 0x01, 0x14, 0xc0, 0xda,  // cls  x1, x0
                 0x01, 0x14, 0xc0, 0x5a,  // cls  w1, w0
             })

AARCH64_TEST(Madd, 2,
             {
                 0x43, 0x00, 0x01, 0x9b,  // madd  x3, x2, x1, x0
                 0x43, 0x00, 0x01, 0x1b,  // madd  w3, w2, w1, w0
             })

AARCH64_TEST(Mul, 2,
             {
                 0x22, 0x7c, 0x00, 0x9b,  // mul  x2, x1, x0
                 0x22, 0x7c, 0x00, 0x1b,  // mul  w2, w1, w0
             })

AARCH64_TEST(Msub, 2,
             {
                 0x43, 0x80, 0x01, 0x9b,  // msub  x3, x2, x1, x0
                 0x43, 0x80, 0x01, 0x1b,  // msub  w3, w2, w1, w0
             })

AARCH64_TEST(Mneg, 2,
             {
                 0x22, 0xfc, 0x00, 0x9b,  // mneg  x2, x1, x0
                 0x22, 0xfc, 0x00, 0x1b,  // mneg  w2, w1, w0
             })

AARCH64_TEST(Smaddl, 1,
             {
                 0x43, 0x00, 0x21, 0x9b,  // smaddl  x3, w2, w1, x0
             })

AARCH64_TEST(Smull, 1,
             {
                 0x22, 0x7c, 0x20, 0x9b,  // smull  x2, w1, w0
             })

AARCH64_TEST(Smsubl, 1,
             {
                 0x43, 0x80, 0x21, 0x9b,  // smsubl  x3, w2, w1, x0
             })

AARCH64_TEST(Smnegl, 1,
             {
                 0x22, 0xfc, 0x20, 0x9b,  // smnegl  x2, w1, w0
             })

AARCH64_TEST(Smulh, 1,
             {
                 0x22, 0x7c, 0x40, 0x9b,  // smulh  x2, x1, x0
             })

AARCH64_TEST(Umaddl, 1,
             {
                 0x43, 0x00, 0xa1, 0x9b,  // umaddl  x3, w2, w1, x0
             })

AARCH64_TEST(Umull, 1,
             {
                 0x22, 0x7c, 0xa0, 0x9b,  // umull  x2, w1, w0
             })

AARCH64_TEST(Umsubl, 1,
             {
                 0x43, 0x80, 0xa1, 0x9b,  // umsubl  x3, w2, w1, x0
             })

AARCH64_TEST(Umnegl, 1,
             {
                 0x22, 0xfc, 0xa0, 0x9b,  // umnegl  x2, w1, w0
             })

AARCH64_TEST(AndShiftedRegister, 8,
             {
                 0x22, 0x44, 0x00, 0x0a,  // and  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x40, 0x0a,  // and  w2, w1, w0, lsr #9
                 0x22, 0x04, 0x80, 0x0a,  // and  w2, w1, w0, asr #1
                 0x22, 0x30, 0xc0, 0x0a,  // and  w2, w1, w0, ror #12
                 0x22, 0x44, 0x00, 0x8a,  // and  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x40, 0x8a,  // and  x2, x1, x0, lsr #9
                 0x22, 0x94, 0x80, 0x8a,  // and  x2, x1, x0, asr #37
                 0x22, 0xd4, 0xc0, 0x8a,  // and  x2, x1, x0, ror #53
             })

AARCH64_TEST(BicShiftedRegister, 8,
             {
                 0x22, 0x44, 0x20, 0x0a,  // bic  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x60, 0x0a,  // bic  w2, w1, w0, lsr #9
                 0x22, 0x04, 0xa0, 0x0a,  // bic  w2, w1, w0, asr #1
                 0x22, 0x30, 0xe0, 0x0a,  // bic  w2, w1, w0, ror #12
                 0x22, 0x44, 0x20, 0x8a,  // bic  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x60, 0x8a,  // bic  x2, x1, x0, lsr #9
                 0x22, 0x94, 0xa0, 0x8a,  // bic  x2, x1, x0, asr #37
                 0x22, 0xd4, 0xe0, 0x8a,  // bic  x2, x1, x0, ror #53
             })

AARCH64_TEST(OrrShiftedRegister, 8,
             {
                 0x22, 0x44, 0x00, 0x2a,  // orr  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x40, 0x2a,  // orr  w2, w1, w0, lsr #9
                 0x22, 0x04, 0x80, 0x2a,  // orr  w2, w1, w0, asr #1
                 0x22, 0x30, 0xc0, 0x2a,  // orr  w2, w1, w0, ror #12
                 0x22, 0x44, 0x00, 0xaa,  // orr  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x40, 0xaa,  // orr  x2, x1, x0, lsr #9
                 0x22, 0x94, 0x80, 0xaa,  // orr  x2, x1, x0, asr #37
                 0x22, 0xd4, 0xc0, 0xaa,  // orr  x2, x1, x0, ror #53
             })

AARCH64_TEST(MovShiftedRegister, 2,
             {
                 0xe1, 0x03, 0x00, 0xaa,  // mov  x1, x0
                 0xe1, 0x03, 0x00, 0x2a,  // mov  w1, w0
             })

AARCH64_TEST(OrnShiftedRegister, 8,
             {
                 0x22, 0x44, 0x20, 0x2a,  // orn  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x60, 0x2a,  // orn  w2, w1, w0, lsr #9
                 0x22, 0x04, 0xa0, 0x2a,  // orn  w2, w1, w0, asr #1
                 0x22, 0x30, 0xe0, 0x2a,  // orn  w2, w1, w0, ror #12
                 0x22, 0x44, 0x20, 0xaa,  // orn  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x60, 0xaa,  // orn  x2, x1, x0, lsr #9
                 0x22, 0x94, 0xa0, 0xaa,  // orn  x2, x1, x0, asr #37
                 0x22, 0xd4, 0xe0, 0xaa,  // orn  x2, x1, x0, ror #53
             })

AARCH64_TEST(MvnShiftedRegister, 2,
             {
                 0xe1, 0x03, 0x20, 0xaa,  // mvn  x1, x0
                 0xe1, 0x03, 0x20, 0x2a,  // mvn  w1, w0
             })

AARCH64_TEST(EorShiftedRegister, 8,
             {
                 0x22, 0x44, 0x00, 0x4a,  // eor  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x40, 0x4a,  // eor  w2, w1, w0, lsr #9
                 0x22, 0x04, 0x80, 0x4a,  // eor  w2, w1, w0, asr #1
                 0x22, 0x30, 0xc0, 0x4a,  // eor  w2, w1, w0, ror #12
                 0x22, 0x44, 0x00, 0xca,  // eor  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x40, 0xca,  // eor  x2, x1, x0, lsr #9
                 0x22, 0x94, 0x80, 0xca,  // eor  x2, x1, x0, asr #37
                 0x22, 0xd4, 0xc0, 0xca,  // eor  x2, x1, x0, ror #53
             })

AARCH64_TEST(EonShiftedRegister, 8,
             {
                 0x22, 0x44, 0x20, 0x4a,  // eon  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x60, 0x4a,  // eon  w2, w1, w0, lsr #9
                 0x22, 0x04, 0xa0, 0x4a,  // eon  w2, w1, w0, asr #1
                 0x22, 0x30, 0xe0, 0x4a,  // eon  w2, w1, w0, ror #12
                 0x22, 0x44, 0x20, 0xca,  // eon  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x60, 0xca,  // eon  x2, x1, x0, lsr #9
                 0x22, 0x94, 0xa0, 0xca,  // eon  x2, x1, x0, asr #37
                 0x22, 0xd4, 0xe0, 0xca,  // eon  x2, x1, x0, ror #53
             })

AARCH64_TEST(AndsShiftedRegister, 8,
             {
                 0x22, 0x44, 0x00, 0x6a,  // ands  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x40, 0x6a,  // ands  w2, w1, w0, lsr #9
                 0x22, 0x04, 0x80, 0x6a,  // ands  w2, w1, w0, asr #1
                 0x22, 0x30, 0xc0, 0x6a,  // ands  w2, w1, w0, ror #12
                 0x22, 0x44, 0x00, 0xea,  // ands  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x40, 0xea,  // ands  x2, x1, x0, lsr #9
                 0x22, 0x94, 0x80, 0xea,  // ands  x2, x1, x0, asr #37
                 0x22, 0xd4, 0xc0, 0xea,  // ands  x2, x1, x0, ror #53
             })

AARCH64_TEST(BicsShiftedRegister, 8,
             {
                 0x22, 0x44, 0x20, 0x6a,  // bics  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x60, 0x6a,  // bics  w2, w1, w0, lsr #9
                 0x22, 0x04, 0xa0, 0x6a,  // bics  w2, w1, w0, asr #1
                 0x22, 0x30, 0xe0, 0x6a,  // bics  w2, w1, w0, ror #12
                 0x22, 0x44, 0x20, 0xea,  // bics  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x60, 0xea,  // bics  x2, x1, x0, lsr #9
                 0x22, 0x94, 0xa0, 0xea,  // bics  x2, x1, x0, asr #37
                 0x22, 0xd4, 0xe0, 0xea,  // bics  x2, x1, x0, ror #53
             })

AARCH64_TEST(AddShiftedRegister, 6,
             {
                 0x22, 0x44, 0x00, 0x0b,  // add  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x40, 0x0b,  // add  w2, w1, w0, lsr #9
                 0x22, 0x04, 0x80, 0x0b,  // add  w2, w1, w0, asr #1
                 0x22, 0x44, 0x00, 0x8b,  // add  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x40, 0x8b,  // add  x2, x1, x0, lsr #9
                 0x22, 0x94, 0x80, 0x8b,  // add  x2, x1, x0, asr #37
             })

AARCH64_TEST(AddsShiftedRegister, 6,
             {
                 0x22, 0x44, 0x00, 0x2b,  // adds  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x40, 0x2b,  // adds  w2, w1, w0, lsr #9
                 0x22, 0x04, 0x80, 0x2b,  // adds  w2, w1, w0, asr #1
                 0x22, 0x44, 0x00, 0xab,  // adds  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x40, 0xab,  // adds  x2, x1, x0, lsr #9
                 0x22, 0x94, 0x80, 0xab,  // adds  x2, x1, x0, asr #37
             })

AARCH64_TEST(CmnShiftedRegister, 6,
             {
                 0x3f, 0x44, 0x00, 0x2b,  // cmn  w1, w0, lsl #17
                 0x3f, 0x24, 0x40, 0x2b,  // cmn  w1, w0, lsr #9
                 0x3f, 0x04, 0x80, 0x2b,  // cmn  w1, w0, asr #1
                 0x3f, 0x44, 0x00, 0xab,  // cmn  x1, x0, lsl #17
                 0x3f, 0x24, 0x40, 0xab,  // cmn  x1, x0, lsr #9
                 0x3f, 0x94, 0x80, 0xab,  // cmn  x1, x0, asr #37
             })

AARCH64_TEST(SubShiftedRegister, 6,
             {
                 0x22, 0x44, 0x00, 0x4b,  // sub  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x40, 0x4b,  // sub  w2, w1, w0, lsr #9
                 0x22, 0x04, 0x80, 0x4b,  // sub  w2, w1, w0, asr #1
                 0x22, 0x44, 0x00, 0xcb,  // sub  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x40, 0xcb,  // sub  x2, x1, x0, lsr #9
                 0x22, 0x94, 0x80, 0xcb,  // sub  x2, x1, x0, asr #37
             })

AARCH64_TEST(NegShiftedRegister, 6,
             {
                 0xe1, 0x47, 0x00, 0x4b,  // neg  w1, w0, lsl #17
                 0xe1, 0x27, 0x40, 0x4b,  // neg  w1, w0, lsr #9
                 0xe1, 0x07, 0x80, 0x4b,  // neg  w1, w0, asr #1
                 0xe1, 0x47, 0x00, 0xcb,  // neg  x1, x0, lsl #17
                 0xe1, 0x27, 0x40, 0xcb,  // neg  x1, x0, lsr #9
                 0xe1, 0x97, 0x80, 0xcb,  // neg  x1, x0, asr #37
             })

AARCH64_TEST(SubsShiftedRegister, 6,
             {
                 0x22, 0x44, 0x00, 0x6b,  // subs  w2, w1, w0, lsl #17
                 0x22, 0x24, 0x40, 0x6b,  // subs  w2, w1, w0, lsr #9
                 0x22, 0x04, 0x80, 0x6b,  // subs  w2, w1, w0, asr #1
                 0x22, 0x44, 0x00, 0xeb,  // subs  x2, x1, x0, lsl #17
                 0x22, 0x24, 0x40, 0xeb,  // subs  x2, x1, x0, lsr #9
                 0x22, 0x94, 0x80, 0xeb,  // subs  x2, x1, x0, asr #37
             })

AARCH64_TEST(CmpShiftedRegister, 6,
             {
                 0x3f, 0x44, 0x00, 0x6b,  // cmp  w1, w0, lsl #17
                 0x3f, 0x24, 0x40, 0x6b,  // cmp  w1, w0, lsr #9
                 0x3f, 0x04, 0x80, 0x6b,  // cmp  w1, w0, asr #1
                 0x3f, 0x44, 0x00, 0xeb,  // cmp  x1, x0, lsl #17
                 0x3f, 0x24, 0x40, 0xeb,  // cmp  x1, x0, lsr #9
                 0x3f, 0x94, 0x80, 0xeb,  // cmp  x1, x0, asr #37
             })

AARCH64_TEST(NegsShiftedRegister, 6,
             {
                 0xe1, 0x47, 0x00, 0x6b,  // negs  w1, w0, lsl #17
                 0xe1, 0x27, 0x40, 0x6b,  // negs  w1, w0, lsr #9
                 0xe1, 0x07, 0x80, 0x6b,  // negs  w1, w0, asr #1
                 0xe1, 0x47, 0x00, 0xeb,  // negs  x1, x0, lsl #17
                 0xe1, 0x27, 0x40, 0xeb,  // negs  x1, x0, lsr #9
                 0xe1, 0x97, 0x80, 0xeb,  // negs  x1, x0, asr #37
             });

AARCH64_TEST(AddExtendedRegister, 16,
             {
                 0x22, 0x04, 0x20, 0x0b,  // add  w2, w1, w0, uxtb #1
                 0x22, 0x28, 0x20, 0x0b,  // add  w2, w1, w0, uxth #2
                 0x22, 0x4c, 0x20, 0x0b,  // add  w2, w1, w0, uxtw #3
                 0x22, 0x70, 0x20, 0x0b,  // add  w2, w1, w0, uxtx #4
                 0x22, 0x84, 0x20, 0x0b,  // add  w2, w1, w0, sxtb #1
                 0x22, 0xa8, 0x20, 0x0b,  // add  w2, w1, w0, sxth #2
                 0x22, 0xcc, 0x20, 0x0b,  // add  w2, w1, w0, sxtw #3
                 0x22, 0xec, 0x20, 0x0b,  // add  w2, w1, w0, sxtx #3
                 0x22, 0x04, 0x20, 0x8b,  // add  x2, x1, w0, uxtb #1
                 0x22, 0x28, 0x20, 0x8b,  // add  x2, x1, w0, uxth #2
                 0x22, 0x4c, 0x20, 0x8b,  // add  x2, x1, w0, uxtw #3
                 0x22, 0x70, 0x20, 0x8b,  // add  x2, x1, x0, uxtx #4
                 0x22, 0x84, 0x20, 0x8b,  // add  x2, x1, w0, sxtb #1
                 0x22, 0xa8, 0x20, 0x8b,  // add  x2, x1, w0, sxth #2
                 0x22, 0xcc, 0x20, 0x8b,  // add  x2, x1, w0, sxtw #3
                 0x22, 0xec, 0x20, 0x8b,  // add  x2, x1, x0, sxtx #3
             })

AARCH64_TEST(AddsExtendedRegister, 16,
             {
                 0x22, 0x04, 0x20, 0x2b,  // adds  w2, w1, w0, uxtb #1
                 0x22, 0x28, 0x20, 0x2b,  // adds  w2, w1, w0, uxth #2
                 0x22, 0x4c, 0x20, 0x2b,  // adds  w2, w1, w0, uxtw #3
                 0x22, 0x70, 0x20, 0x2b,  // adds  w2, w1, w0, uxtx #4
                 0x22, 0x84, 0x20, 0x2b,  // adds  w2, w1, w0, sxtb #1
                 0x22, 0xa8, 0x20, 0x2b,  // adds  w2, w1, w0, sxth #2
                 0x22, 0xcc, 0x20, 0x2b,  // adds  w2, w1, w0, sxtw #3
                 0x22, 0xec, 0x20, 0x2b,  // adds  w2, w1, w0, sxtx #3
                 0x22, 0x04, 0x20, 0xab,  // adds  x2, x1, w0, uxtb #1
                 0x22, 0x28, 0x20, 0xab,  // adds  x2, x1, w0, uxth #2
                 0x22, 0x4c, 0x20, 0xab,  // adds  x2, x1, w0, uxtw #3
                 0x22, 0x70, 0x20, 0xab,  // adds  x2, x1, x0, uxtx #4
                 0x22, 0x84, 0x20, 0xab,  // adds  x2, x1, w0, sxtb #1
                 0x22, 0xa8, 0x20, 0xab,  // adds  x2, x1, w0, sxth #2
                 0x22, 0xcc, 0x20, 0xab,  // adds  x2, x1, w0, sxtw #3
                 0x22, 0xec, 0x20, 0xab,  // adds  x2, x1, x0, sxtx #3
             })

AARCH64_TEST(CmnExtendedRegister, 16,
             {
                 0x3f, 0x04, 0x20, 0x2b,  // cmn  w1, w0, uxtb #1
                 0x3f, 0x28, 0x20, 0x2b,  // cmn  w1, w0, uxth #2
                 0x3f, 0x4c, 0x20, 0x2b,  // cmn  w1, w0, uxtw #3
                 0x3f, 0x70, 0x20, 0x2b,  // cmn  w1, w0, uxtx #4
                 0x3f, 0x84, 0x20, 0x2b,  // cmn  w1, w0, sxtb #1
                 0x3f, 0xa8, 0x20, 0x2b,  // cmn  w1, w0, sxth #2
                 0x3f, 0xcc, 0x20, 0x2b,  // cmn  w1, w0, sxtw #3
                 0x3f, 0xec, 0x20, 0x2b,  // cmn  w1, w0, sxtx #3
                 0x3f, 0x04, 0x20, 0xab,  // cmn  x1, w0, uxtb #1
                 0x3f, 0x28, 0x20, 0xab,  // cmn  x1, w0, uxth #2
                 0x3f, 0x4c, 0x20, 0xab,  // cmn  x1, w0, uxtw #3
                 0x3f, 0x70, 0x20, 0xab,  // cmn  x1, x0, uxtx #4
                 0x3f, 0x84, 0x20, 0xab,  // cmn  x1, w0, sxtb #1
                 0x3f, 0xa8, 0x20, 0xab,  // cmn  x1, w0, sxth #2
                 0x3f, 0xcc, 0x20, 0xab,  // cmn  x1, w0, sxtw #3
                 0x3f, 0xec, 0x20, 0xab,  // cmn  x1, x0, sxtx #3
             })

AARCH64_TEST(SubExtendedRegister, 6,
             {
                 0x22, 0x04, 0x20, 0x4b,  // sub  w2, w1, w0, uxtb #1
                 0x22, 0x28, 0x20, 0x4b,  // sub  w2, w1, w0, uxth #2
                 0x22, 0x4c, 0x20, 0x4b,  // sub  w2, w1, w0, uxtw #3
                 0x22, 0x70, 0x20, 0x4b,  // sub  w2, w1, w0, uxtx #4
                 0x22, 0x84, 0x20, 0x4b,  // sub  w2, w1, w0, sxtb #1
                 0x22, 0xa8, 0x20, 0x4b,  // sub  w2, w1, w0, sxth #2
                 0x22, 0xcc, 0x20, 0x4b,  // sub  w2, w1, w0, sxtw #3
                 0x22, 0xec, 0x20, 0x4b,  // sub  w2, w1, w0, sxtx #3
                 0x22, 0x04, 0x20, 0xcb,  // sub  x2, x1, w0, uxtb #1
                 0x22, 0x28, 0x20, 0xcb,  // sub  x2, x1, w0, uxth #2
                 0x22, 0x4c, 0x20, 0xcb,  // sub  x2, x1, w0, uxtw #3
                 0x22, 0x70, 0x20, 0xcb,  // sub  x2, x1, x0, uxtx #4
                 0x22, 0x84, 0x20, 0xcb,  // sub  x2, x1, w0, sxtb #1
                 0x22, 0xa8, 0x20, 0xcb,  // sub  x2, x1, w0, sxth #2
                 0x22, 0xcc, 0x20, 0xcb,  // sub  x2, x1, w0, sxtw #3
                 0x22, 0xec, 0x20, 0xcb,  // sub  x2, x1, x0, sxtx #3
             })

AARCH64_TEST(SubsExtendedRegister, 16,
             {
                 0x22, 0x04, 0x20, 0x6b,  // subs  w2, w1, w0, uxtb #1
                 0x22, 0x28, 0x20, 0x6b,  // subs  w2, w1, w0, uxth #2
                 0x22, 0x4c, 0x20, 0x6b,  // subs  w2, w1, w0, uxtw #3
                 0x22, 0x70, 0x20, 0x6b,  // subs  w2, w1, w0, uxtx #4
                 0x22, 0x84, 0x20, 0x6b,  // subs  w2, w1, w0, sxtb #1
                 0x22, 0xa8, 0x20, 0x6b,  // subs  w2, w1, w0, sxth #2
                 0x22, 0xcc, 0x20, 0x6b,  // subs  w2, w1, w0, sxtw #3
                 0x22, 0xec, 0x20, 0x6b,  // subs  w2, w1, w0, sxtx #3
                 0x22, 0x04, 0x20, 0xeb,  // subs  x2, x1, w0, uxtb #1
                 0x22, 0x28, 0x20, 0xeb,  // subs  x2, x1, w0, uxth #2
                 0x22, 0x4c, 0x20, 0xeb,  // subs  x2, x1, w0, uxtw #3
                 0x22, 0x70, 0x20, 0xeb,  // subs  x2, x1, x0, uxtx #4
                 0x22, 0x84, 0x20, 0xeb,  // subs  x2, x1, w0, sxtb #1
                 0x22, 0xa8, 0x20, 0xeb,  // subs  x2, x1, w0, sxth #2
                 0x22, 0xcc, 0x20, 0xeb,  // subs  x2, x1, w0, sxtw #3
                 0x22, 0xec, 0x20, 0xeb,  // subs  x2, x1, x0, sxtx #3
             })

AARCH64_TEST(CmpExtendedRegister, 16,
             {
                 0x3f, 0x04, 0x20, 0x6b,  // cmp  w1, w0, uxtb #1
                 0x3f, 0x28, 0x20, 0x6b,  // cmp  w1, w0, uxth #2
                 0x3f, 0x4c, 0x20, 0x6b,  // cmp  w1, w0, uxtw #3
                 0x3f, 0x70, 0x20, 0x6b,  // cmp  w1, w0, uxtx #4
                 0x3f, 0x84, 0x20, 0x6b,  // cmp  w1, w0, sxtb #1
                 0x3f, 0xa8, 0x20, 0x6b,  // cmp  w1, w0, sxth #2
                 0x3f, 0xcc, 0x20, 0x6b,  // cmp  w1, w0, sxtw #3
                 0x3f, 0xec, 0x20, 0x6b,  // cmp  w1, w0, sxtx #3
                 0x3f, 0x04, 0x20, 0xeb,  // cmp  x1, w0, uxtb #1
                 0x3f, 0x28, 0x20, 0xeb,  // cmp  x1, w0, uxth #2
                 0x3f, 0x4c, 0x20, 0xeb,  // cmp  x1, w0, uxtw #3
                 0x3f, 0x70, 0x20, 0xeb,  // cmp  x1, x0, uxtx #4
                 0x3f, 0x84, 0x20, 0xeb,  // cmp  x1, w0, sxtb #1
                 0x3f, 0xa8, 0x20, 0xeb,  // cmp  x1, w0, sxth #2
                 0x3f, 0xcc, 0x20, 0xeb,  // cmp  x1, w0, sxtw #3
                 0x3f, 0xec, 0x20, 0xeb,  // cmp  x1, x0, sxtx #3
             })

AARCH64_RANDOM_TEST(AndImmediate, 0b00010010000000000000000000000000,
                    0b10000000011111111111110000000000)

AARCH64_RANDOM_TEST(OrrImmediate, 0b00110010000000000000000000000000,
                    0b10000000011111111111110000000000)

AARCH64_RANDOM_TEST(EorImmediate, 0b01010010000000000000000000000000,
                    0b10000000011111111111110000000000)

AARCH64_RANDOM_TEST(AndsImmediate, 0b01110010000000000000000000000000,
                    0b10000000011111111111110000000000)

AARCH64_RANDOM_TEST(Movn, 0b00010010100000000000000000000000,
                    0b10000000011111111111111111100000)

AARCH64_RANDOM_TEST(Movz, 0b01010010100000000000000000000000,
                    0b10000000011111111111111111100000)

AARCH64_RANDOM_TEST(Movk, 0b01110010100000000000000000000000,
                    0b10000000011111111111111111100000)

AARCH64_RANDOM_TEST(Casp, 0b00001000001000000111110000000000,
                    0b01000000010111111000001111111111)
}  // namespace test
}  // namespace reil

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
