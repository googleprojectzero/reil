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

#ifndef REIL_ANALYSIS_CONSTANTS_ANALYSIS_H_

#include "analysis/session.h"
#include "flow_graph/flow_graph.h"
#include "memory_image/memory_image.h"

namespace reil {
namespace analysis {
class ConstantsState {
 public:
  virtual ~ConstantsState() {}

  virtual std::shared_ptr<Immediate> GetRegister(uint8_t index) const = 0;
  virtual std::shared_ptr<Immediate> GetTemporary(uint16_t index) const = 0;
  virtual std::shared_ptr<Immediate> GetOperand(
      const Operand& operand) const = 0;

  virtual void SetRegister(uint8_t index, std::shared_ptr<Immediate> value) = 0;
  virtual void SetRegister(uint8_t index, Immediate&& value) = 0;
  virtual void SetRegister(uint8_t index, const Immediate& value) = 0;

  virtual void SetTemporary(uint16_t index,
                            std::shared_ptr<Immediate> value) = 0;
  virtual void SetTemporary(uint16_t index, Immediate&& value) = 0;
  virtual void SetTemporary(uint16_t index, const Immediate& value) = 0;

  virtual void SetOperand(const Operand& operand,
                          std::shared_ptr<Immediate> value) = 0;
  virtual void SetOperand(const Operand& operand, Immediate&& value) = 0;
  virtual void SetOperand(const Operand& operand, const Immediate& value) = 0;
};

class ConstantsAnalysis {
 public:
  virtual std::unique_ptr<ConstantsState> At(const Node& node) = 0;
  virtual std::unique_ptr<ConstantsState> On(const Edge& edge) = 0;
  virtual void Update() = 0;
};

std::unique_ptr<ConstantsAnalysis> LocalConstantsAnalysis(Session& session,
                                                          uint64_t address);

std::unique_ptr<ConstantsAnalysis> GlobalConstantsAnalysis(Session& session);
}  // namespace analysis
}  // namespace reil

#define REIL_ANALYSIS_CONSTANTS_ANALYSIS_H_
#endif  // REIL_ANALYSIS_CONSTANTS_ANALYSIS_H_
