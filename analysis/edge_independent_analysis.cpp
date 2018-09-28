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

#include "analysis/edge_independent_analysis.h"

#include "glog/logging.h"

namespace reil {
namespace analysis {
void EdgeIndependentAnalysis::TransformAdd(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformAnd(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformBisz(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformBsh(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformDiv(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformJcc(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformLdm(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformMod(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformMul(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformNop(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformOr(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformStm(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformStr(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformSub(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformUndef(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformUnkn(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformXor(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformEqu(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformLshl(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformLshr(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformAshr(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformSex(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformSys(const Instruction& ri) {}

void EdgeIndependentAnalysis::TransformIte(const Instruction& ri) {}

void EdgeIndependentAnalysis::OnNativeInstructionEnd() {}

EdgeIndependentAnalysis::~EdgeIndependentAnalysis() {
}

bool EdgeIndependentAnalysis::Valid() const { return valid_; }

void EdgeIndependentAnalysis::Invalidate() { valid_ = false; }

void EdgeIndependentAnalysis::Transform(const Edge& edge,
                                        const Instruction& ri) {
  switch (ri.opcode) {
    case Opcode::Add: {
      TransformAdd(ri);
    } break;

    case Opcode::And: {
      TransformAnd(ri);
    } break;

    case Opcode::Div: {
      TransformDiv(ri);
    } break;

    case Opcode::Jcc: {
      TransformJcc(ri);
    } break;

    case Opcode::Ldm: {
      TransformLdm(ri);
    } break;

    case Opcode::Mod: {
      TransformMod(ri);
    } break;

    case Opcode::Mul: {
      TransformMul(ri);
    } break;

    case Opcode::Nop: {
    } break;

    case Opcode::Or: {
      TransformOr(ri);
    } break;

    case Opcode::Stm: {
      TransformStm(ri);
    } break;

    case Opcode::Str: {
      TransformStr(ri);
    } break;

    case Opcode::Sub: {
      TransformSub(ri);
    } break;

    case Opcode::Undef: {
      TransformUndef(ri);
    } break;

    case Opcode::Unkn: {
      TransformUnkn(ri);
    } break;

    case Opcode::Xor: {
      TransformXor(ri);
    } break;

    case Opcode::Equ: {
      TransformEqu(ri);
    } break;

    case Opcode::Lshl: {
      TransformLshl(ri);
    } break;

    case Opcode::Lshr: {
      TransformLshr(ri);
    } break;

    case Opcode::Ashr: {
      TransformAshr(ri);
    } break;

    case Opcode::Sex: {
      TransformSex(ri);
    } break;

    case Opcode::Sys: {
      TransformSys(ri);
    } break;

    case Opcode::Ite: {
      TransformIte(ri);
    } break;

    default:
      CHECK(false);
  }

  if (edge.kind >= EdgeKind::kNativeFlow) {
    this->OnNativeInstructionEnd();
  }
}

std::string EdgeIndependentAnalysis::Print() const {
  return "EdgeIndependentAnalysis";
}

std::ostream& operator<<(std::ostream& stream,
                         const EdgeIndependentAnalysis& analysis) {
  return stream << analysis.Print();
}
}  // namespace analysis
}  // namespace reil