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

#ifndef REIL_ANALYSIS_EDGE_INDEPENDENT_ANALYSIS_H_

#include <iostream>

#include "flow_graph/flow_graph.h"
#include "reil/reil.h"

namespace reil {
namespace analysis {
class EdgeIndependentAnalysis {
 protected:
  bool valid_ = true;

  virtual void TransformAdd(const Instruction& ri);
  virtual void TransformAnd(const Instruction& ri);
  virtual void TransformBisz(const Instruction& ri);
  virtual void TransformBsh(const Instruction& ri);
  virtual void TransformDiv(const Instruction& ri);
  virtual void TransformJcc(const Instruction& ri);
  virtual void TransformLdm(const Instruction& ri);
  virtual void TransformMod(const Instruction& ri);
  virtual void TransformMul(const Instruction& ri);
  virtual void TransformNop(const Instruction& ri);
  virtual void TransformOr(const Instruction& ri);
  virtual void TransformStm(const Instruction& ri);
  virtual void TransformStr(const Instruction& ri);
  virtual void TransformSub(const Instruction& ri);
  virtual void TransformUndef(const Instruction& ri);
  virtual void TransformUnkn(const Instruction& ri);
  virtual void TransformXor(const Instruction& ri);
  virtual void TransformEqu(const Instruction& ri);
  virtual void TransformLshl(const Instruction& ri);
  virtual void TransformLshr(const Instruction& ri);
  virtual void TransformAshr(const Instruction& ri);
  virtual void TransformSex(const Instruction& ri);
  virtual void TransformSys(const Instruction& ri);
  virtual void TransformIte(const Instruction& ri);

  virtual void OnNativeInstructionEnd();

 public:
  virtual ~EdgeIndependentAnalysis();

  virtual bool Valid() const;
  virtual void Invalidate();
  virtual void Transform(const Edge& edge, const Instruction& ri);
  virtual std::string Print() const;
};

std::ostream& operator<<(std::ostream& stream,
                         const EdgeIndependentAnalysis& analysis);
}  // namespace analysis
}  // namespace reil

#define REIL_ANALYSIS_EDGE_INDEPENDENT_ANALYSIS_H_
#endif  // REIL_ANALYSIS_EDGE_INDEPENDENT_ANALYSIS_H_