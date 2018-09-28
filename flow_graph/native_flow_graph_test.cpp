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

#include "flow_graph/native_flow_graph.h"

namespace reil {
namespace flow_graph {
namespace test {
TEST(NativeFlowGraph, CompleteBasicBlock) {
  NativeFlowGraph nfg;

  nfg.AddEdge(0, 0x1000, NativeEdgeKind::kCall);
  nfg.AddEdge(0x1010, 0, NativeEdgeKind::kReturn);

  EXPECT_EQ(nfg.BasicBlockStart(0x1008), 0x1000);
  EXPECT_EQ(nfg.BasicBlockEnd(0x1008), 0x1010);
}

TEST(NativeFlowGraph, IncompleteBasicBlock) {
  NativeFlowGraph nfg;

  nfg.AddEdge(0, 0x1000, NativeEdgeKind::kCall);

  EXPECT_EQ(nfg.BasicBlockStart(0x1008), 0);
  EXPECT_EQ(nfg.BasicBlockEnd(0x1008), 0);
}
}  // namespace test
}  // namespace disassembler
}  // namespace reil

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
