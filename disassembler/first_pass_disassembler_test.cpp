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

#include "glog/logging.h"
#include "gtest/gtest.h"

#include "control_flow_graph/control_flow_graph.h"
#include "memory_image/memory_image.h"

#include "disassembler/first_pass_disassembler.h"

namespace reil {
namespace disassembler {
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
TEST(FirstPassDisassembler, Simple) {
  std::shared_ptr<MemoryImage> memory_image =
      MemoryImage::Load("disassembler/test_data/simple.memory_image");
  ASSERT_NE(memory_image, nullptr);

  std::map<uint64_t, std::shared_ptr<ControlFlowGraph>> functions;
  std::shared_ptr<ControlFlowGraph> function;
  std::set<Edge> edges;

  FirstPassDisassembler disassembler(memory_image);
  disassembler.QueueFunction(0x400078);
  functions = disassembler.AnalyseAllFunctions();

  function = functions[0x400078];
  EXPECT_EQ(function->outgoing_edges().size(), 4);

  edges = function->outgoing_edges(0x400084);
  EXPECT_EQ(edges.size(), 2);
  EXPECT_EQ(edges.count(Edge(0x400084, 0x400088, kNativeFlow)), 1);
  EXPECT_EQ(edges.count(Edge(0x400084, 0x40009c, kNativeCall)), 1);

  edges = function->outgoing_edges(0x400088);
  EXPECT_EQ(edges.size(), 2);
  EXPECT_EQ(edges.count(Edge(0x400088, 0x40008c, kNativeFlow)), 1);
  EXPECT_EQ(edges.count(Edge(0x400088, 0x400090, kNativeJump)), 1);

  edges = function->outgoing_edges(0x40008c);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(Edge(0x40008c, 0x400090, kNativeFlow)), 1);

  edges = function->outgoing_edges(0x400098);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(Edge(0x400098, 0, kNativeReturn)), 1);

  function = functions[0x40009c];
  EXPECT_EQ(function->outgoing_edges().size(), 1);

  edges = function->outgoing_edges(0x40009c);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(Edge(0x40009c, 0, kNativeReturn)), 1);
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
TEST(FirstPassDisassembler, BlockSplit) {
  std::shared_ptr<MemoryImage> memory_image =
      MemoryImage::Load("disassembler/test_data/block_split.memory_image");
  ASSERT_NE(memory_image, nullptr);

  std::map<uint64_t, std::shared_ptr<ControlFlowGraph>> functions;
  std::shared_ptr<ControlFlowGraph> function;
  std::set<Edge> edges;

  FirstPassDisassembler disassembler(memory_image);
  disassembler.QueueFunction(0x400078);
  functions = disassembler.AnalyseAllFunctions();

  function = functions[0x400078];
  EXPECT_EQ(function->outgoing_edges().size(), 3);

  edges = function->outgoing_edges(0x400080);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(Edge(0x400080, 0x400084, kNativeFlow)), 1);

  edges = function->outgoing_edges(0x400084);
  EXPECT_EQ(edges.size(), 2);
  EXPECT_EQ(edges.count(Edge(0x400084, 0x400088, kNativeFlow)), 1);
  EXPECT_EQ(edges.count(Edge(0x400084, 0x400084, kNativeJump)), 1);

  edges = function->outgoing_edges(0x400088);
  EXPECT_EQ(edges.size(), 1);
  EXPECT_EQ(edges.count(Edge(0x400088, 0, kNativeReturn)), 1);
}
}  // namespace test
}  // namespace disassembler
}  // namespace reil

int main(int argc, char **argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
