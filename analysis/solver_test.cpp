
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

#include "absl/memory/memory.h"
#include "analysis/solver.h"
#include "control_flow_graph/control_flow_graph.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "memory_image/memory_image.h"

namespace reil {
namespace analysis {
namespace test {
typedef std::set<std::string> WrittenRegisters;

std::shared_ptr<WrittenRegisters> Merge(
    absl::Span<const std::shared_ptr<WrittenRegisters>> states) {
  std::shared_ptr<WrittenRegisters> output =
      std::make_shared<WrittenRegisters>();
  for (auto& state : states) {
    output->insert(state->cbegin(), state->cend());
  }
  return output;
}

std::shared_ptr<WrittenRegisters> Transform(
    const Edge& edge, const Instruction& ri,
    std::shared_ptr<WrittenRegisters> in_state) {
  std::shared_ptr<WrittenRegisters> out_state = in_state;
  switch (ri.opcode) {
    case Opcode::Add:
    case Opcode::And:
    case Opcode::Bisz:
    case Opcode::Bsh:
    case Opcode::Div:
    case Opcode::Ldm:
    case Opcode::Mod:
    case Opcode::Mul:
    case Opcode::Or:
    case Opcode::Str:
    case Opcode::Sub:
    case Opcode::Xor:
    case Opcode::Bisnz:
    case Opcode::Equ:
    case Opcode::Lshl:
    case Opcode::Lshr:
    case Opcode::Ashr:
    case Opcode::Sex:
    case Opcode::Ite: {
      if (ri.output.index() == kRegister) {
        Register reg = absl::get<Register>(ri.output);
        if (0 == out_state->count(reg.name)) {
          out_state = std::make_shared<WrittenRegisters>(*in_state);
          out_state->insert(reg.name);
        }
      }
    }

    default:
      break;
  }

  return out_state;
}

bool Compare(const WrittenRegisters& lhs, const WrittenRegisters& rhs) {
  if (rhs.size() < lhs.size()) {
    return false;
  }

  for (auto& l : lhs) {
    if (rhs.count(l) != 1) {
      return false;
    }
  }

  return true;
}

std::string Print(const WrittenRegisters& wrs) {
  std::stringstream stream;
  stream << "{";
  for (auto& wr : wrs) {
    stream << wr << ", ";
  }
  if (wrs.size() >= 1) {
    stream.seekp(-2, std::ios_base::cur);
  }
  stream << "}";
  return stream.str();
}

/*
0000000000400078 <_start>:
  400078:       d10083ff        sub     sp, sp, #0x20
  40007c:       a9017bfd        stp     x29, x30, [sp, #16]
  400080:       910043fd        add     x29, sp, #0x10
  400084:       d2802460        mov     x0, #0x123                      // #291
  400088:       91115801        add     x1, x0, #0x456
  40008c:       d65f03c0        ret
*/
TEST(Solver, LiteralArithmetic) {
  std::shared_ptr<MemoryImage> memory_image =
      MemoryImage::Load("analysis/test_data/literal_arithmetic.mem");
  ASSERT_NE(memory_image, nullptr);

  std::shared_ptr<ControlFlowGraph> function =
      ControlFlowGraph::Load("analysis/test_data/literal_arithmetic.cfg");
  ASSERT_NE(function, nullptr);

  std::shared_ptr<InstructionGraph> instruction_graph =
      InstructionGraph::Create(memory_image);
  instruction_graph->Add(*function.get());

  std::map<Edge, std::shared_ptr<WrittenRegisters>> edge_states;
  edge_states[Edge(0, 0x400078, kNativeCall)] =
      std::make_shared<WrittenRegisters>();
  ASSERT_EQ(Solve<WrittenRegisters>(instruction_graph, edge_states, Merge,
                                    Transform, Compare, Print),
            true);

  auto end_state = edge_states[Edge(0x40008c, 0, kNativeReturn)];
  ASSERT_NE(end_state, nullptr);
  EXPECT_EQ(end_state->size(), 4);
  EXPECT_EQ(end_state->count("x0"), 1);
  EXPECT_EQ(end_state->count("x1"), 1);
  EXPECT_EQ(end_state->count("x29"), 1);
  EXPECT_EQ(end_state->count("sp"), 1);
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
TEST(Solver, Loop) {
  std::shared_ptr<MemoryImage> memory_image =
      MemoryImage::Load("analysis/test_data/loop.mem");
  ASSERT_NE(memory_image, nullptr);

  std::shared_ptr<ControlFlowGraph> function =
      ControlFlowGraph::Load("analysis/test_data/loop.cfg");
  ASSERT_NE(function, nullptr);

  std::shared_ptr<InstructionGraph> instruction_graph =
      InstructionGraph::Create(memory_image);
  instruction_graph->Add(*function.get());

  std::map<Edge, std::shared_ptr<WrittenRegisters>> edge_states;
  edge_states[Edge(0, 0x400078, kNativeCall)] =
      std::make_shared<WrittenRegisters>();
  ASSERT_EQ(Solve<WrittenRegisters>(instruction_graph, edge_states, Merge,
                                    Transform, Compare, Print),
            true);

  auto end_state = edge_states[Edge(0x400098, 0, kNativeReturn)];
  ASSERT_NE(end_state, nullptr);
  EXPECT_EQ(end_state->size(), 8);
  EXPECT_EQ(end_state->count("x0"), 1);
  EXPECT_EQ(end_state->count("x29"), 1);
  EXPECT_EQ(end_state->count("x30"), 1);
  EXPECT_EQ(end_state->count("sp"), 1);
  EXPECT_EQ(end_state->count("n"), 1);
  EXPECT_EQ(end_state->count("c"), 1);
  EXPECT_EQ(end_state->count("z"), 1);
  EXPECT_EQ(end_state->count("v"), 1);
}
}  // namespace test
}  // namespace analysis
}  // namespace reil

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
