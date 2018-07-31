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

#include <random>

#include "gtest/gtest.h"

#include "reil/immediate.h"

namespace reil {

std::mt19937_64 prng;

TEST(Immediate, Add) {
  for (int i = 0; i < 128; ++i) {
    uint32_t a = prng();
    uint32_t b = prng();
    uint64_t c = (uint64_t)a + b;

    Immediate A = Immediate(32, a);
    Immediate B = Immediate(32, b);
    Immediate C = A + B;

    EXPECT_EQ(c, static_cast<uint64_t>(C));
  }
}

TEST(Immediate, Subtract) {
  for (int i = 0; i < 128; ++i) {
    uint32_t a = prng();
    uint32_t b = prng();
    uint64_t c = (uint64_t)a - b;

    Immediate A = Immediate(32, a);
    Immediate B = Immediate(32, b);
    Immediate C = A - B;

    EXPECT_EQ(c, static_cast<uint64_t>(C));
  }
}

TEST(Immediate, Multiply) {
  for (int i = 0; i < 0x100; ++i) {
    uint32_t a = prng();
    uint32_t b = prng();
    uint64_t c = (uint64_t)a * b;

    Immediate A = Immediate(32, a);
    Immediate B = Immediate(32, b);
    Immediate C = A * B;

    EXPECT_EQ(c, static_cast<uint64_t>(C))
        << A << " * " << B << " == " << C << " [" << Immediate(64, c) << "]";
  }
}

TEST(Immediate, Divide) {
  for (int i = 0; i < 128; ++i) {
    uint32_t a = prng();
    uint64_t b = prng() % 0xffff;
    uint64_t c = (uint64_t)a / b;

    Immediate A = Immediate(32, a);
    Immediate B = Immediate(32, b);
    Immediate C = A / B;

    EXPECT_EQ(c, static_cast<uint64_t>(C));
  }
}

TEST(Immediate, Modulo) {
  for (int i = 0; i < 128; ++i) {
    uint32_t a = prng();
    uint64_t b = prng() % 0xffffffff;
    uint64_t c = (uint64_t)a % b;

    Immediate A = Immediate(32, a);
    Immediate B = Immediate(32, b);
    Immediate C = A % B;

    EXPECT_EQ(c, static_cast<uint64_t>(C));
  }
}

TEST(Immediate, And) {
  for (int i = 0; i < 128; ++i) {
    uint64_t a = prng();
    uint64_t b = prng();
    uint64_t c = a & b;

    Immediate A = Immediate(64, a);
    Immediate B = Immediate(64, b);
    Immediate C = A & B;

    EXPECT_EQ(c, static_cast<uint64_t>(C));
  }
}

TEST(Immediate, Or) {
  for (int i = 0; i < 128; ++i) {
    uint64_t a = prng();
    uint64_t b = prng();
    uint64_t c = a | b;

    Immediate A = Immediate(64, a);
    Immediate B = Immediate(64, b);
    Immediate C = A | B;

    EXPECT_EQ(c, static_cast<uint64_t>(C));
  }
}

TEST(Immediate, Xor) {
  for (int i = 0; i < 128; ++i) {
    uint64_t a = prng();
    uint64_t b = prng();
    uint64_t c = a ^ b;

    Immediate A = Immediate(64, a);
    Immediate B = Immediate(64, b);
    Immediate C = A ^ B;

    EXPECT_EQ(c, static_cast<uint64_t>(C));
  }
}

TEST(Immediate, LogicalShiftLeft) {
  for (int i = 0; i < 128; ++i) {
    uint64_t a = prng();
    uint64_t b = prng() % 64;
    uint64_t c = (uint64_t)a << b;

    Immediate A = Immediate(64, a);
    Immediate B = Immediate(64, b);
    Immediate C = A << B;

    EXPECT_EQ(c, static_cast<uint64_t>(C));
  }
}

TEST(Immediate, LogicalShiftRight) {
  for (int i = 0; i < 128; ++i) {
    uint64_t a = prng();
    uint64_t b = prng() % 64;
    uint64_t c = (uint64_t)a >> b;

    Immediate A = Immediate(64, a);
    Immediate B = Immediate(64, b);
    Immediate C = A >> B;

    EXPECT_EQ(c, static_cast<uint64_t>(C));
  }
}

}  // namespace reil

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
