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
#include <sstream>

#include "glog/logging.h"
#include "gtest/gtest.h"

#include "analysis/constants_analysis.h"
#include "analysis/session.h"
#include "flow_graph/flow_graph.h"
#include "memory_image/memory_image.h"
#include "reil/aarch64.h"

namespace reil {
namespace analysis {
namespace test {
/*
0000000000400078 <_start>:
  400078:       d10083ff        sub     sp, sp, #0x20
  40007c:       a9017bfd        stp     x29, x30, [sp, #16]
  400080:       910043fd        add     x29, sp, #0x10
  400084:       d2802460        mov     x0, #0x123                      // #291
  400088:       91115801        add     x1, x0, #0x456
  40008c:       d65f03c0        ret
*/
TEST(ConstantsAnalysis, LiteralArithmetic) {
  auto memory_image =
      MemoryImage::Load("analysis/test_data/literal_arithmetic.mem");
  ASSERT_NE(memory_image, nullptr);

  Session session(*memory_image);
  session.AddFlowGraph(FlowGraph::Load(
      *memory_image, "analysis/test_data/literal_arithmetic.cfg"));

  auto constants = LocalConstantsAnalysis(session, 0x400078);
  ASSERT_NE(constants, nullptr);

  auto at_return = constants->At(0x40008c);
  ASSERT_NE(at_return, nullptr);

  auto x0 = at_return->GetRegister(reil::aarch64::kX0);
  ASSERT_NE(x0, nullptr);
  EXPECT_EQ(static_cast<uint64_t>(*x0), 0x123);

  auto x1 = at_return->GetRegister(reil::aarch64::kX1);
  ASSERT_NE(x1, nullptr);
  EXPECT_EQ(static_cast<uint64_t>(*x1), 0x579);
}

/*
0000000000400078 <_start>:
  400078:       d10083ff        sub     sp, sp, #0x20
  40007c:       a9017bfd        stp     x29, x30, [sp, #16]
  400080:       910043fd        add     x29, sp, #0x10
  400084:       d2800200        mov     x0, #0x10                       // #16

0000000000400088 <loop>:
  400088:       f1000400        subs    x0, x0, #0x1
  40008c:       54ffffe1        b.ne    400088 <loop>  // b.any
  400090:       a9417bfd        ldp     x29, x30, [sp, #16]
  400094:       910083ff        add     sp, sp, #0x20
  400098:       d65f03c0        ret
*/
TEST(ConstantsAnalysis, Loop) {
  auto memory_image = MemoryImage::Load("analysis/test_data/loop.mem");
  ASSERT_NE(memory_image, nullptr);

  Session session(*memory_image);
  session.AddFlowGraph(
      FlowGraph::Load(*memory_image, "analysis/test_data/loop.cfg"));

  auto constants = LocalConstantsAnalysis(session, 0x400078);
  ASSERT_NE(constants, nullptr);

  auto at_return = constants->At(0x400098);
  ASSERT_NE(at_return, nullptr);

  // This test just validates that the solver correctly merges multiple x0
  // values to Top and is able to terminate the analysis.

  auto x0 = at_return->GetRegister(reil::aarch64::kX0);
  ASSERT_EQ(x0, nullptr);
}

/*
0000000000400078 <_start>:
  400078:       d10083ff        sub     sp, sp, #0x20
  40007c:       a9017bfd        stp     x29, x30, [sp, #16]
  400080:       910043fd        add     x29, sp, #0x10
  400084:       90000000        adrp    x0, 400000 <_start-0x78>
  400088:       91048c00        add     x0, x0, #0x123
  40008c:       9343fc00        asr     x0, x0, #3
  400090:       d65f03c0        ret
*/
TEST(Solver, AsrBug) {
  auto memory_image = MemoryImage::Load("analysis/test_data/asr_bug.mem");
  ASSERT_NE(memory_image, nullptr);

  Session session(*memory_image);
  session.AddFlowGraph(
      FlowGraph::Load(*memory_image, "analysis/test_data/asr_bug.cfg"));

  auto constants = LocalConstantsAnalysis(session, 0x400078);
  ASSERT_NE(constants, nullptr);

  auto at_return = constants->At(0x400090);
  ASSERT_NE(at_return, nullptr);

  // This test just validates the asr instruction is handled correctly, since
  // it is one of the more complex translations containing internal branches.

  auto x0 = at_return->GetRegister(reil::aarch64::kX0);
  ASSERT_NE(x0, nullptr);
  EXPECT_EQ(static_cast<uint64_t>(*x0), 0x80024);
}

/*
0000000000400078 <_start>:
  400078:       d10083ff        sub     sp, sp, #0x20
  40007c:       a9017bfd        stp     x29, x30, [sp, #16]
  400080:       910043fd        add     x29, sp, #0x10
  400084:       d2802460        mov     x0, #0x123                      // #291
  400088:       f1048c00        subs    x0, x0, #0x123
  40008c:       54000040        b.eq    400094 <done>  // b.none
  400090:       91048c00        add     x0, x0, #0x123

0000000000400094 <done>:
  400094:       d65f03c0        ret
*/
TEST(Solver, ControlFlowConstraint) {
  auto memory_image =
      MemoryImage::Load("analysis/test_data/control_flow_constraint.mem");
  ASSERT_NE(memory_image, nullptr);

  Session session(*memory_image);
  session.AddFlowGraph(FlowGraph::Load(
      *memory_image, "analysis/test_data/control_flow_constraint.cfg"));

  auto constants = LocalConstantsAnalysis(session, 0x400078);
  ASSERT_NE(constants, nullptr);

  auto at_return = constants->At(0x400094);
  ASSERT_NE(at_return, nullptr);

  // This test validates that the solver didn't propagate the values from the
  // branch which cannot be taken. If the instruction at 400090 was taken into
  // account, the final x0 value would have to be Top.

  auto x0 = at_return->GetRegister(reil::aarch64::kX0);
  ASSERT_NE(x0, nullptr);
  EXPECT_EQ(static_cast<uint64_t>(*x0), 0);
}
}  // namespace test
}  // namespace analysis
}  // namespace reil

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
