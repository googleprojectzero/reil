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

#include "control_flow_graph/control_flow_graph.h"
#include "glog/logging.h"
#include "gtest/gtest.h"

namespace reil {
namespace test {
TEST(NodeTest, Equal) {
  Node a(0, 0);
  Node b(0, 1);
  Node c(1, 0);
  Node d(1, 1);

  EXPECT_TRUE(a == a);
  EXPECT_FALSE(a == b);
  EXPECT_FALSE(a == c);
  EXPECT_FALSE(a == d);

  EXPECT_FALSE(b == a);
  EXPECT_TRUE(b == b);
  EXPECT_FALSE(b == c);
  EXPECT_FALSE(b == d);

  EXPECT_FALSE(c == a);
  EXPECT_FALSE(c == b);
  EXPECT_TRUE(c == c);
  EXPECT_FALSE(c == d);

  EXPECT_FALSE(d == a);
  EXPECT_FALSE(d == b);
  EXPECT_FALSE(d == c);
  EXPECT_TRUE(d == d);
}

TEST(NodeTest, LessThan) {
  Node a(0, 0);
  Node b(0, 1);
  Node c(1, 0);
  Node d(1, 1);

  EXPECT_FALSE(a < a);
  EXPECT_TRUE(a < b);
  EXPECT_TRUE(a < c);
  EXPECT_TRUE(a < d);

  EXPECT_FALSE(b < a);
  EXPECT_FALSE(b < b);
  EXPECT_TRUE(b < c);
  EXPECT_TRUE(b < d);

  EXPECT_FALSE(c < a);
  EXPECT_FALSE(c < b);
  EXPECT_FALSE(c < c);
  EXPECT_TRUE(c < d);

  EXPECT_FALSE(d < a);
  EXPECT_FALSE(d < b);
  EXPECT_FALSE(d < c);
  EXPECT_FALSE(d < d);
}

TEST(NodeTest, LessThanOrEqual) {
  Node a(0, 0);
  Node b(0, 1);
  Node c(1, 0);
  Node d(1, 1);

  EXPECT_TRUE(a <= a);
  EXPECT_TRUE(a <= b);
  EXPECT_TRUE(a <= c);
  EXPECT_TRUE(a <= d);

  EXPECT_FALSE(b <= a);
  EXPECT_TRUE(b <= b);
  EXPECT_TRUE(b <= c);
  EXPECT_TRUE(b <= d);

  EXPECT_FALSE(c <= a);
  EXPECT_FALSE(c <= b);
  EXPECT_TRUE(c <= c);
  EXPECT_TRUE(c <= d);

  EXPECT_FALSE(d <= a);
  EXPECT_FALSE(d <= b);
  EXPECT_FALSE(d <= c);
  EXPECT_TRUE(d <= d);
}

TEST(EdgeTest, Equal) {
  Node a(0, 0);
  Node b(0, 1);

  Edge aaf(a, a, kReilFlow);
  Edge aab(a, a, kReilJump);
  Edge abf(a, b, kReilFlow);
  Edge abb(a, b, kReilJump);
  Edge ba(b, a, kReilFlow);
  Edge bb(b, b, kReilFlow);

  EXPECT_TRUE(aaf == aaf);
  EXPECT_FALSE(aaf == aab);
  EXPECT_FALSE(aaf == abf);
  EXPECT_FALSE(aaf == abb);
  EXPECT_FALSE(aaf == ba);
  EXPECT_FALSE(aaf == bb);

  EXPECT_FALSE(aab == aaf);
  EXPECT_TRUE(aab == aab);
  EXPECT_FALSE(aab == abf);
  EXPECT_FALSE(aab == abb);
  EXPECT_FALSE(aab == ba);
  EXPECT_FALSE(aab == bb);

  EXPECT_FALSE(abf == aaf);
  EXPECT_FALSE(abf == aab);
  EXPECT_TRUE(abf == abf);
  EXPECT_FALSE(abf == abb);
  EXPECT_FALSE(abf == ba);
  EXPECT_FALSE(abf == bb);

  EXPECT_FALSE(abb == aaf);
  EXPECT_FALSE(abb == aab);
  EXPECT_FALSE(abb == abf);
  EXPECT_TRUE(abb == abb);
  EXPECT_FALSE(abb == ba);
  EXPECT_FALSE(abb == bb);

  EXPECT_FALSE(ba == aaf);
  EXPECT_FALSE(ba == aab);
  EXPECT_FALSE(ba == abf);
  EXPECT_FALSE(ba == abb);
  EXPECT_TRUE(ba == ba);
  EXPECT_FALSE(ba == bb);

  EXPECT_FALSE(bb == aaf);
  EXPECT_FALSE(bb == aab);
  EXPECT_FALSE(bb == abf);
  EXPECT_FALSE(bb == abb);
  EXPECT_FALSE(bb == ba);
  EXPECT_TRUE(bb == bb);
}

TEST(EdgeTest, LessThan) {
  Node a(0, 0);
  Node b(0, 1);

  Edge aaf(a, a, kReilFlow);
  Edge aab(a, a, kReilJump);
  Edge abf(a, b, kReilFlow);
  Edge abb(a, b, kReilJump);
  Edge ba(b, a, kReilFlow);
  Edge bb(b, b, kReilFlow);

  EXPECT_FALSE(aaf < aaf);
  EXPECT_TRUE(aaf < aab);
  EXPECT_TRUE(aaf < abf);
  EXPECT_TRUE(aaf < abb);
  EXPECT_TRUE(aaf < ba);
  EXPECT_TRUE(aaf < bb);

  EXPECT_FALSE(aab < aaf);
  EXPECT_FALSE(aab < aab);
  EXPECT_TRUE(aab < abf);
  EXPECT_TRUE(aab < abb);
  EXPECT_TRUE(aab < ba);
  EXPECT_TRUE(aab < bb);

  EXPECT_FALSE(abf < aaf);
  EXPECT_FALSE(abf < aab);
  EXPECT_FALSE(abf < abf);
  EXPECT_TRUE(abf < abb);
  EXPECT_TRUE(abf < ba);
  EXPECT_TRUE(abf < bb);

  EXPECT_FALSE(abb < aaf);
  EXPECT_FALSE(abb < aab);
  EXPECT_FALSE(abb < abf);
  EXPECT_FALSE(abb < abb);
  EXPECT_TRUE(abb < ba);
  EXPECT_TRUE(abb < bb);

  EXPECT_FALSE(ba < aaf);
  EXPECT_FALSE(ba < aab);
  EXPECT_FALSE(ba < abf);
  EXPECT_FALSE(ba < abb);
  EXPECT_FALSE(ba < ba);
  EXPECT_TRUE(ba < bb);

  EXPECT_FALSE(bb < aaf);
  EXPECT_FALSE(bb < aab);
  EXPECT_FALSE(bb < abf);
  EXPECT_FALSE(bb < abb);
  EXPECT_FALSE(bb < ba);
  EXPECT_FALSE(bb < bb);
}

TEST(EdgeTest, LessThanOrEqual) {
  Node a(0, 0);
  Node b(0, 1);

  Edge aaf(a, a, kReilFlow);
  Edge aab(a, a, kReilJump);
  Edge abf(a, b, kReilFlow);
  Edge abb(a, b, kReilJump);
  Edge ba(b, a, kReilFlow);
  Edge bb(b, b, kReilFlow);

  EXPECT_TRUE(aaf <= aaf);
  EXPECT_TRUE(aaf <= aab);
  EXPECT_TRUE(aaf <= abf);
  EXPECT_TRUE(aaf <= abb);
  EXPECT_TRUE(aaf <= ba);
  EXPECT_TRUE(aaf <= bb);

  EXPECT_FALSE(aab <= aaf);
  EXPECT_TRUE(aab <= aab);
  EXPECT_TRUE(aab <= abf);
  EXPECT_TRUE(aab <= abb);
  EXPECT_TRUE(aab <= ba);
  EXPECT_TRUE(aab <= bb);

  EXPECT_FALSE(abf <= aaf);
  EXPECT_FALSE(abf <= aab);
  EXPECT_TRUE(abf <= abf);
  EXPECT_TRUE(abf <= abb);
  EXPECT_TRUE(abf <= ba);
  EXPECT_TRUE(abf <= bb);

  EXPECT_FALSE(abb <= aaf);
  EXPECT_FALSE(abb <= aab);
  EXPECT_FALSE(abb <= abf);
  EXPECT_TRUE(abb <= abb);
  EXPECT_TRUE(abb <= ba);
  EXPECT_TRUE(abb <= bb);

  EXPECT_FALSE(ba <= aaf);
  EXPECT_FALSE(ba <= aab);
  EXPECT_FALSE(ba <= abf);
  EXPECT_FALSE(ba <= abb);
  EXPECT_TRUE(ba <= ba);
  EXPECT_TRUE(ba <= bb);

  EXPECT_FALSE(bb <= aaf);
  EXPECT_FALSE(bb <= aab);
  EXPECT_FALSE(bb <= abf);
  EXPECT_FALSE(bb <= abb);
  EXPECT_FALSE(bb <= ba);
  EXPECT_TRUE(bb <= bb);
}
}  // namespace test
}  // namespace reil

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
