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

#include "control_flow_graph/control_flow_graph.h"
#include "memory_image/memory_image.h"

#include "disassembler/first_pass_disassembler.h"
#include "disassembler/function_scan.h"

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " memory_image_proto"
              << "[output_directory]" << std::endl;
    return -1;
  }

  std::shared_ptr<reil::MemoryImage> memory_image =
      reil::MemoryImage::Load(argv[1]);
  CHECK(memory_image);
  std::set<uint64_t> heuristic_functions =
      reil::disassembler::FunctionScan(*memory_image);
  reil::disassembler::FirstPassDisassembler first_pass(memory_image);
  for (auto heuristic_function : heuristic_functions) {
    first_pass.QueueFunction(heuristic_function);
  }

  std::map<uint64_t, std::shared_ptr<reil::ControlFlowGraph>> functions =
      first_pass.AnalyseAllFunctions();

  if (argc >= 3) {
    for (auto& function_iter : functions) {
      std::stringstream path_stream;
      path_stream << argv[2] << "/" << std::hex << function_iter.first
                  << ".cfg";
      function_iter.second->Save(path_stream.str());
    }
  }
}
