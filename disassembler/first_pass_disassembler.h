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

#ifndef REIL_FIRST_PASS_DISASSEMBLER_H_

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "control_flow_graph/control_flow_graph.h"
#include "memory_image/memory_image.h"

#include "reil/aarch64.h"
#include "reil/reil.h"

namespace reil {
namespace disassembler {
class FirstPassDisassembler {
 private:
  std::shared_ptr<MemoryImage> memory_image_;
  std::map<uint64_t, std::shared_ptr<ControlFlowGraph>> functions_;

  std::set<uint64_t> function_queue_;
  std::set<uint64_t> basic_block_queue_;

  std::shared_ptr<ControlFlowGraph> current_fn_;
  reil::aarch64::decoder::Instruction current_insn_;

  void DecodeInstruction(uint64_t address);

  bool QueueBasicBlock(uint64_t address, bool force = false);

  uint64_t PcRelativeOffset(const reil::aarch64::decoder::Operand& dst);

  bool AnalyseBranch(const reil::aarch64::decoder::Operand& dst);
  bool AnalyseMaybeBranch(const reil::aarch64::decoder::Operand& dsts);
  bool AnalyseCall(const reil::aarch64::decoder::Operand& dst);

  bool AnalyseOneInstruction(bool basic_block_start);
  bool AnalyseOneBasicBlock();
  bool AnalyseOneFunction();

 public:
  FirstPassDisassembler(std::shared_ptr<MemoryImage> memory_image);
  ~FirstPassDisassembler();

  bool QueueFunction(uint64_t address);
  std::map<uint64_t, std::shared_ptr<ControlFlowGraph>> AnalyseAllFunctions();
};
}  // namespace disassembler
}  // namespace reil

#define REIL_FIRST_PASS_DISASSEMBLER_H_
#endif  // REIL_FIRST_PASS_DISASSEMBLER_H_
