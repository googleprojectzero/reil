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

#include "glog/logging.h"
#include "gtest/gtest.h"

#include "disassembler/disassembler.h"
#include "memory_image/memory_image.h"

namespace reil {
namespace disassembler {
namespace test {
/*
0000000000400078 <_start>:
  400078:       10000060        adr     x0, 400084 <function>
  40007c:       d63f0000        blr     x0
  400080:       d65f03c0        ret

0000000000400084 <function>:
  400084:       d65f03c0        ret
*/
TEST(ResolveBranches, CallConstantRegister) {
  auto memory_image =
      MemoryImage::Load("disassembler/test_data/call_constant_register.mem");
  ASSERT_NE(memory_image, nullptr);

  auto function = DisassembleFunction(*memory_image, 0x400078);

  auto edges = function->outgoing_edges(0x40007c);
  EXPECT_EQ(edges.size(), 2);
  EXPECT_EQ(edges.count(NativeEdge(0x40007c, 0x400080, NativeEdgeKind::kFlow)),
            1);
  EXPECT_EQ(edges.count(NativeEdge(0x40007c, 0, NativeEdgeKind::kCall)), 1);

  ResolveBranches(*memory_image, *function);

  edges = function->outgoing_edges(0x40007c);
  EXPECT_EQ(edges.size(), 2);
  EXPECT_EQ(edges.count(NativeEdge(0x40007c, 0x400080, NativeEdgeKind::kFlow)),
            1);
  EXPECT_EQ(edges.count(NativeEdge(0x40007c, 0x400084, NativeEdgeKind::kCall)),
            1);
}

/*
0000000000400078 <_start>:
  400078:       10000040        adr     x0, 400080 <label>
  40007c:       d61f0000        br      x0

0000000000400080 <label>:
  400080:       d65f03c0        ret
*/
TEST(ResolveBranches, JumpConstantRegister) {
  auto memory_image =
      MemoryImage::Load("disassembler/test_data/jump_constant_register.mem");
  ASSERT_NE(memory_image, nullptr);

  auto function = DisassembleFunction(*memory_image, 0x400078);

  auto edges = function->outgoing_edges(0x40007c);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x40007c, 0, NativeEdgeKind::kJump)), 1);

  ResolveBranches(*memory_image, *function);

  edges = function->outgoing_edges(0x40007c);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x40007c, 0x400080, NativeEdgeKind::kJump)),
            1);

  edges = function->outgoing_edges(0x400080);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x400080, 0, NativeEdgeKind::kReturn)), 1);
}
}  // namespace test
}  // namespace disassembler
}  // namespace reil

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
