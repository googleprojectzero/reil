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

#include "analysis/edge_dependent_analysis.h"

#include "glog/logging.h"

namespace reil {
namespace analysis {
void EdgeDependentAnalysis::TransformAdd(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformAnd(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformBisz(const Edge& edge,
                                          const Instruction& ri) {}

void EdgeDependentAnalysis::TransformBsh(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformDiv(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformJcc(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformLdm(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformMod(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformMul(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformNop(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformOr(const Edge& edge,
                                        const Instruction& ri) {}

void EdgeDependentAnalysis::TransformStm(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformStr(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformSub(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformUndef(const Edge& edge,
                                           const Instruction& ri) {}

void EdgeDependentAnalysis::TransformUnkn(const Edge& edge,
                                          const Instruction& ri) {}

void EdgeDependentAnalysis::TransformXor(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformEqu(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformLshl(const Edge& edge,
                                          const Instruction& ri) {}

void EdgeDependentAnalysis::TransformLshr(const Edge& edge,
                                          const Instruction& ri) {}

void EdgeDependentAnalysis::TransformAshr(const Edge& edge,
                                          const Instruction& ri) {}

void EdgeDependentAnalysis::TransformSex(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformSys(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::TransformIte(const Edge& edge,
                                         const Instruction& ri) {}

void EdgeDependentAnalysis::OnNativeInstructionEnd() {}

EdgeDependentAnalysis::~EdgeDependentAnalysis() {}

bool EdgeDependentAnalysis::Valid() const { return valid_; }

void EdgeDependentAnalysis::Invalidate() { valid_ = false; }

void EdgeDependentAnalysis::Transform(const Edge& edge, const Instruction& ri) {
  switch (ri.opcode) {
    case Opcode::Add: {
      TransformAdd(edge, ri);
    } break;

    case Opcode::And: {
      TransformAnd(edge, ri);
    } break;

    case Opcode::Div: {
      TransformDiv(edge, ri);
    } break;

    case Opcode::Jcc: {
      TransformJcc(edge, ri);
    } break;

    case Opcode::Ldm: {
      TransformLdm(edge, ri);
    } break;

    case Opcode::Mod: {
      TransformMod(edge, ri);
    } break;

    case Opcode::Mul: {
      TransformMul(edge, ri);
    } break;

    case Opcode::Nop: {
    } break;

    case Opcode::Or: {
      TransformOr(edge, ri);
    } break;

    case Opcode::Stm: {
      TransformStm(edge, ri);
    } break;

    case Opcode::Str: {
      TransformStr(edge, ri);
    } break;

    case Opcode::Sub: {
      TransformSub(edge, ri);
    } break;

    case Opcode::Undef: {
      TransformUndef(edge, ri);
    } break;

    case Opcode::Unkn: {
      TransformUnkn(edge, ri);
    } break;

    case Opcode::Xor: {
      TransformXor(edge, ri);
    } break;

    case Opcode::Equ: {
      TransformEqu(edge, ri);
    } break;

    case Opcode::Lshl: {
      TransformLshl(edge, ri);
    } break;

    case Opcode::Lshr: {
      TransformLshr(edge, ri);
    } break;

    case Opcode::Ashr: {
      TransformAshr(edge, ri);
    } break;

    case Opcode::Sex: {
      TransformSex(edge, ri);
    } break;

    case Opcode::Sys: {
      TransformSys(edge, ri);
    } break;

    case Opcode::Ite: {
      TransformIte(edge, ri);
    } break;

    default:
      CHECK(false);
  }

  if (edge.kind >= EdgeKind::kNativeFlow) {
    this->OnNativeInstructionEnd();
  }
}

std::string EdgeDependentAnalysis::Print() const {
  return "EdgeDependentAnalysis";
}

std::ostream& operator<<(std::ostream& stream,
                         const EdgeDependentAnalysis& analysis) {
  return stream << analysis.Print();
}
}  // namespace analysis
}  // namespace reil