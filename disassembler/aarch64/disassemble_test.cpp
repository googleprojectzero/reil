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
#include <iostream>

#include "disassembler/disassembler.h"
#include "glog/logging.h"
#include "gtest/gtest.h"

namespace reil {
namespace disassembler {
namespace aarch64 {
namespace test {
/*
0000000000400078 <_start>:
  400078: d10083ff  sub sp, sp, #0x20
  40007c: a9017bfd  stp x29, x30, [sp, #16]
  400080: 910043fd  add x29, sp, #0x10
  400084: 94000006  bl  40009c <function>
  400088: b4000040  cbz x0, 400090 <label>
  40008c: d503201f  nop

0000000000400090 <label>:
  400090: a9417bfd  ldp x29, x30, [sp, #16]
  400094: 910083ff  add sp, sp, #0x20
  400098: d65f03c0  ret

000000000040009c <function>:
  40009c: d65f03c0  ret
*/
TEST(DisassembleFunction, Simple) {
  auto memory_image = MemoryImage::Load("disassembler/test_data/simple.mem");
  ASSERT_NE(memory_image, nullptr);

  auto function = DisassembleFunction(*memory_image, 0x400078);
  EXPECT_EQ(function->outgoing_edges().size(), 5);

  auto edges = function->outgoing_edges(0x400084);
  EXPECT_EQ(edges.size(), 2);
  EXPECT_EQ(edges.count(NativeEdge(0x400084, 0x400088, NativeEdgeKind::kFlow)),
            1);
  EXPECT_EQ(edges.count(NativeEdge(0x400084, 0x40009c, NativeEdgeKind::kCall)),
            1);

  edges = function->outgoing_edges(0x400088);
  EXPECT_EQ(edges.size(), 2);
  EXPECT_EQ(edges.count(NativeEdge(0x400088, 0x40008c, NativeEdgeKind::kFlow)),
            1);
  EXPECT_EQ(edges.count(NativeEdge(0x400088, 0x400090, NativeEdgeKind::kJump)),
            1);

  edges = function->outgoing_edges(0x40008c);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x40008c, 0x400090, NativeEdgeKind::kFlow)),
            1);

  edges = function->outgoing_edges(0x400098);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x400098, 0, NativeEdgeKind::kReturn)), 1);

  function = DisassembleFunction(*memory_image, 0x40009c);
  EXPECT_EQ(function->outgoing_edges().size(), 2);

  edges = function->outgoing_edges(0x40009c);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x40009c, 0, NativeEdgeKind::kReturn)), 1);
}

/*
0000000000400078 <_start>:
  400078: d10083ff  sub sp, sp, #0x20
  40007c: a9017bfd  stp x29, x30, [sp, #16]
  400080: 910043fd  add x29, sp, #0x10

0000000000400084 <split>:
  400084: b4000000  cbz x0, 400084 <split>
  400088: d65f03c0  ret
*/
TEST(DisassembleFunction, BlockSplit) {
  auto memory_image =
      MemoryImage::Load("disassembler/test_data/block_split.mem");
  ASSERT_NE(memory_image, nullptr);

  auto function = DisassembleFunction(*memory_image, 0x400078);
  EXPECT_EQ(function->outgoing_edges().size(), 4);

  auto edges = function->outgoing_edges(0x400080);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x400080, 0x400084, NativeEdgeKind::kFlow)),
            1);

  edges = function->outgoing_edges(0x400084);
  EXPECT_EQ(edges.size(), 2);
  EXPECT_EQ(edges.count(NativeEdge(0x400084, 0x400088, NativeEdgeKind::kFlow)),
            1);
  EXPECT_EQ(edges.count(NativeEdge(0x400084, 0x400084, NativeEdgeKind::kJump)),
            1);

  edges = function->outgoing_edges(0x400088);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x400088, 0, NativeEdgeKind::kReturn)), 1);
}

/*
0000000000400078 <_start>:
  400078:       b4000040        cbz     x0, 400080 <return>
  40007c:       d65f03c0        ret

0000000000400080 <return>:
  400080:       d65f03c0        ret
*/
TEST(DisassembleFunction, BlockSplit2) {
  auto memory_image =
      MemoryImage::Load("disassembler/test_data/block_split_2.mem");
  ASSERT_NE(memory_image, nullptr);

  auto function = DisassembleFunction(*memory_image, 0x400078);
  EXPECT_EQ(function->outgoing_edges().size(), 4);

  auto edges = function->outgoing_edges(0x400078);
  EXPECT_EQ(edges.size(), 2);
  EXPECT_EQ(edges.count(NativeEdge(0x400078, 0x40007c, NativeEdgeKind::kFlow)),
            1);
  EXPECT_EQ(edges.count(NativeEdge(0x400078, 0x400080, NativeEdgeKind::kJump)),
            1);

  // this test is about making sure that we don't have stray flow edges on this
  // branch instruction

  edges = function->outgoing_edges(0x40007c);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x40007c, 0, NativeEdgeKind::kReturn)), 1);

  edges = function->outgoing_edges(0x400080);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x400080, 0, NativeEdgeKind::kReturn)), 1);
}

/*
0000000000400078 <_start>:
  400078:       b4000040        cbz     x0, 400080 <return>
  40007c:       14000001        b       400080 <return>

0000000000400080 <return>:
  400080:       d65f03c0        ret
*/
TEST(DisassembleFunction, BlockSplit3) {
  auto memory_image =
      MemoryImage::Load("disassembler/test_data/block_split_3.mem");
  ASSERT_NE(memory_image, nullptr);

  auto function = DisassembleFunction(*memory_image, 0x400078);
  EXPECT_EQ(function->outgoing_edges().size(), 4);

  auto edges = function->outgoing_edges(0x400078);
  EXPECT_EQ(edges.size(), 2);
  EXPECT_EQ(edges.count(NativeEdge(0x400078, 0x40007c, NativeEdgeKind::kFlow)),
            1);
  EXPECT_EQ(edges.count(NativeEdge(0x400078, 0x400080, NativeEdgeKind::kJump)),
            1);

  // this test is about making sure that we don't have stray flow edges on this
  // branch instruction

  edges = function->outgoing_edges(0x40007c);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x40007c, 0x400080, NativeEdgeKind::kJump)),
            1);

  edges = function->outgoing_edges(0x400080);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x400080, 0, NativeEdgeKind::kReturn)), 1);
}

/*
0000000000400078 <_start>:
  400078:       b4000040        cbz     x0, 400080 <return>
  40007c:       d4200000        brk     #0x0

0000000000400080 <return>:
  400080:       d65f03c0        ret
*/
TEST(DisassembleFunction, BlockSplit4) {
  auto memory_image =
      MemoryImage::Load("disassembler/test_data/block_split_4.mem");
  ASSERT_NE(memory_image, nullptr);

  auto function = DisassembleFunction(*memory_image, 0x400078);
  EXPECT_EQ(function->outgoing_edges().size(), 4);

  auto edges = function->outgoing_edges(0x400078);
  EXPECT_EQ(edges.size(), 2);
  EXPECT_EQ(edges.count(NativeEdge(0x400078, 0x40007c, NativeEdgeKind::kFlow)),
            1);
  EXPECT_EQ(edges.count(NativeEdge(0x400078, 0x400080, NativeEdgeKind::kJump)),
            1);

  // this test is about making sure that we don't have stray flow edges on this
  // break instruction

  edges = function->outgoing_edges(0x40007c);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x40007c, 0, NativeEdgeKind::kBreak)), 1);

  edges = function->outgoing_edges(0x400080);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(NativeEdge(0x400080, 0, NativeEdgeKind::kReturn)), 1);
}
}  // namespace test
}  // namespace aarch64
}  // namespace disassembler
}  // namespace reil

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
