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
#include <mutex>
#include <sstream>
#include <thread>

#include "glog/logging.h"

#include "disassembler/disassembler.h"
#include "flow_graph/instruction_provider.h"
#include "flow_graph/native_flow_graph.h"

// TODO: this has horrendous lock contention.
std::set<uint64_t> queue;
std::mutex queue_mutex;

std::map<uint64_t, std::unique_ptr<reil::NativeFlowGraph>> functions;
std::mutex functions_mutex;

void disassembler_thread(const reil::MemoryImage& memory_image) {
  std::unique_lock<std::mutex> queue_lock(queue_mutex, std::defer_lock);
  std::unique_lock<std::mutex> functions_lock(functions_mutex, std::defer_lock);

  auto ip = reil::InstructionProvider::Create(memory_image);
  CHECK(ip);

  std::lock(queue_lock, functions_lock);
  while (!queue.empty()) {
    uint64_t address = *queue.begin();
    queue.erase(queue.begin());
    functions[address] = nullptr;

    LOG_EVERY_N(INFO, 1000) << "queue_size: " << queue.size();

    queue_lock.unlock();
    functions_lock.unlock();

    auto function =
        reil::disassembler::DisassembleFunction(memory_image, address);

    if (!function->resolved()) {
      reil::disassembler::ResolveBranches(memory_image, *ip, *function);
    }

    std::lock(queue_lock, functions_lock);
    for (auto edge_iter : function->outgoing_edges()) {
      for (auto edge : edge_iter.second) {
        if (edge.kind == reil::NativeEdgeKind::kCall && edge.target != 0 &&
            queue.count(edge.target) == 0 &&
            functions.count(edge.target) == 0) {
          queue.insert(edge.target);
        }
      }
    }

    auto functions_iter = functions.find(address);
    functions_iter->second = std::move(function);

    // we still hold locks here
  }
}

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  std::unique_lock<std::mutex> queue_lock(queue_mutex, std::defer_lock);
  std::unique_lock<std::mutex> functions_lock(functions_mutex, std::defer_lock);

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " memory_image_proto"
              << "[output_directory]" << std::endl;
    return -1;
  }

  auto memory_image = reil::MemoryImage::Load(argv[1]);
  CHECK(memory_image);

  queue_lock.lock();
  queue = reil::disassembler::FindFunctions(*memory_image);
  queue_lock.unlock();

  int resolved = 0;
  int thread_count = std::thread::hardware_concurrency() / 2;

  std::vector<std::thread> disassembler_threads;
  for (int i = 0; i < thread_count; ++i) {
    disassembler_threads.emplace_back(disassembler_thread, *memory_image);
  }

  for (int i = 0; i < thread_count; ++i) {
    disassembler_threads[i].join();
  }

  functions_lock.lock();
  if (argc >= 3) {
    for (auto& function_iter : functions) {
      resolved += function_iter.second->resolved();
      std::stringstream path_stream;
      path_stream << argv[2] << "/" << std::hex << function_iter.first
                  << ".cfg";
      function_iter.second->Save(path_stream.str());
    }
  } else {
    for (auto& function_iter : functions) {
      resolved += function_iter.second->resolved();
    }
  }
  functions_lock.unlock();

  LOG(INFO) << "total_resolved:  " << std::dec << resolved;
  LOG(INFO) << "total_functions: " << std::dec << functions.size();
}
