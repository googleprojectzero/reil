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

#include "control_flow_graph/instruction_graph.h"

#include "absl/memory/memory.h"
#include "glog/logging.h"
#include "reil/aarch64.h"

namespace reil {
InstructionGraph::InstructionGraph(std::shared_ptr<MemoryImage> memory_image,
                                   size_t cache_size)
    : memory_image_(memory_image), ring_cache_(cache_size) {}

InstructionGraph::~InstructionGraph() {}

uint64_t InstructionGraph::NextNativeInstruction(uint64_t address) {
  auto ni = NativeInstruction(address);
  return ni->address + ni->size;
}

std::shared_ptr<reil::NativeInstruction> InstructionGraph::NativeInstruction(
    uint64_t address) {
  std::shared_ptr<reil::NativeInstruction> ni = nullptr;

  auto cache_iter = cache_.find(address);
  if (cache_iter != cache_.end()) {
    ni = cache_iter->second.lock();
  }

  if (ni == nullptr) {
    if (memory_image_->executable(address)) {
      const uint8_t* data;
      uint64_t size;
      std::tie(data, size) = memory_image_->Read(address);
      ni = std::make_shared<reil::NativeInstruction>(
          NativeInstructionImpl(address, data, size));
      ring_cache_[cache_index_++ % ring_cache_.size()] = ni;
      cache_[address] = ni;
    } else {
      DCHECK(false);
    }
  }

  return ni;
}

Node InstructionGraph::NextInstruction(Node node) {
  auto ni = NativeInstruction(node.address);
  if (ni) {
    if (node.offset + 1 >= ni->reil.size()) {
      node = Node(ni->address + ni->size, 0);
    } else {
      node = Node(node.address, node.offset + 1);
    }
  }
  return node;
}

reil::Instruction InstructionGraph::Instruction(Node node) {
  std::shared_ptr<reil::NativeInstruction> ni = NativeInstruction(node.address);
  DCHECK(node.offset < ni->reil.size());
  return ni->reil[node.offset];
}

void InstructionGraph::Add(const ControlFlowGraph& other) {
  for (auto& in_iter : other.incoming_edges()) {
    for (auto& edge : in_iter.second) {
      if (edge.source == 0) {
        AddEdge(edge);
      }
    }

    auto out_iter = other.outgoing_edges().lower_bound(in_iter.first);
    if (out_iter == other.outgoing_edges().end()) {
      // this basic block isn't contained in other
      continue;
    }

    Node bb_start = in_iter.first;
    Node bb_end = Node(NextNativeInstruction(out_iter->first.address));

    VLOG(1) << "basic_block " << bb_start << " - " << bb_end;

    Node next_node = 0;
    for (Node node = bb_start; node <= bb_end; node = next_node) {
      next_node = NextInstruction(node);

      auto ri = Instruction(node);
      if (ri.opcode == reil::Opcode::Jcc) {
        if (other.outgoing_edges().count(node.address) == 0) {
          continue;
        }

        std::set<Edge> edges = other.outgoing_edges().at(node.address);
        bool flow = true;

        DCHECK(ri.input1.index() == kImmediate);
        Immediate hint = absl::get<Immediate>(ri.input1);
        if (hint == kJump) {
          if (ri.input0.index() == kImmediate &&
              (bool)absl::get<Immediate>(ri.input0)) {
            flow = false;
          }

          if (ri.output.index() == kOffset) {
            Offset offset = absl::get<Offset>(ri.output);
            AddEdge(Edge(node, Node(node.address, offset.offset), kReilJump));
          } else {
            for (auto edge : edges) {
              if (edge.kind == kNativeJump) {
                edge.source = node;
                AddEdge(edge);
              } else {
                DCHECK(edge.kind == kNativeFlow);
              }
            }
          }
        } else if (hint == kCall) {
          for (auto edge : edges) {
            if (edge.kind == kNativeCall) {
              edge.source = node;
              AddEdge(edge);
            } else {
              DCHECK(edge.kind == kNativeFlow);
            }
          }
        } else if (hint == kReturn) {
          flow = false;
          for (auto edge : edges) {
            if (edge.kind == kNativeReturn) {
              edge.source = node;
              AddEdge(edge);
            } else {
              DCHECK(false);
            }
          }
        }

        if (flow) {
          if (node.address != next_node.address) {
            AddEdge(node, next_node, kNativeFlow);
          } else {
            AddEdge(node, next_node, kReilFlow);
          }
        }
      } else {
        if (node.address != next_node.address) {
          AddEdge(node, next_node, kNativeFlow);
        } else {
          AddEdge(node, next_node, kReilFlow);
        }
      }
    }
  }
}

class AArch64InstructionGraphImpl : public InstructionGraph {
  reil::NativeInstruction NativeInstructionImpl(uint64_t address,
                                                const uint8_t* bytes,
                                                size_t bytes_len) override;

 public:
  AArch64InstructionGraphImpl(std::shared_ptr<MemoryImage> memory_image,
                              size_t cache_size);
};

AArch64InstructionGraphImpl::AArch64InstructionGraphImpl(
    std::shared_ptr<MemoryImage> memory_image, size_t cache_size)
    : InstructionGraph(memory_image, cache_size) {}

reil::NativeInstruction AArch64InstructionGraphImpl::NativeInstructionImpl(
    uint64_t address, const uint8_t* bytes, size_t bytes_len) {
  return reil::aarch64::TranslateInstruction(address, bytes, bytes_len);
}

std::unique_ptr<InstructionGraph> InstructionGraph::Create(
    std::shared_ptr<MemoryImage> memory_image, size_t cache_size) {
  if (memory_image->architecture_name() == "aarch64") {
    return absl::make_unique<AArch64InstructionGraphImpl>(memory_image,
                                                          cache_size);
  } else {
    LOG(FATAL) << "Unsupported memory image architecture: "
               << memory_image->architecture_name();
  }
  return nullptr;
}

std::unique_ptr<InstructionGraph> InstructionGraph::Load(
    std::shared_ptr<MemoryImage> memory_image, std::string path, size_t cache_size) {
  auto result = Create(memory_image, cache_size);
  if (result) {
    std::unique_ptr<ControlFlowGraph> cfg = ControlFlowGraph::Load(path);
    if (cfg) {
      result->Add(*cfg);
    }
  }
  return result;
}
}  // namespace reil
