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

#include "glog/logging.h"
#include "gtest/gtest.h"

#include "memory_image/memory_image.h"

#include "disassembler/function_scan.h"

namespace reil {
namespace disassembler {
namespace test {
/*
0000000000400078 <_start>:
  400078: d10083ff  sub sp, sp, #0x20
  40007c: a9017bfd  stp x29, x30, [sp, #16]
  400080: 910043fd  add x29, sp, #0x10
  400084: 94000006  bl  40009c <function>
  400088: b4000040  cbz x0, 400090 <label>
  40008c: d503201f  nop

0000000000400090 <label>:
  400090: a9417bfd  ldp x29, x30, [sp, #16]
  400094: 910083ff  add sp, sp, #0x20
  400098: d65f03c0  ret

000000000040009c <function>:
  40009c: d65f03c0  ret
*/
TEST(FunctionScan, Simple) {
  auto memory_image =
      MemoryImage::Load("disassembler/test_data/simple.memory_image");
  ASSERT_NE(memory_image, nullptr);

  std::set<uint64_t> results = FunctionScan(*memory_image);
  EXPECT_EQ(results.size(), 1);
  EXPECT_NE(results.find(0x400078), results.end());
}

/*
0000000000400078 <_start>:
  400078: d10083ff  sub sp, sp, #0x20
  40007c: 6d0107e0  stp d0, d1, [sp, #16]
  400080: 910043fd  add x29, sp, #0x10
  400084: d65f03c0  ret

0000000000400088 <function1>:
  400088: d10083ff  sub sp, sp, #0x20
  40008c: 6d8107e0  stp d0, d1, [sp, #16]!
  400090: 910043fd  add x29, sp, #0x10
  400094: d65f03c0  ret

0000000000400098 <function2>:
  400098: 6d0107e0  stp d0, d1, [sp, #16]
  40009c: 910043fd  add x29, sp, #0x10
  4000a0: d65f03c0  ret

00000000004000a4 <function3>:
  4000a4: 6d8107e0  stp d0, d1, [sp, #16]!
  4000a8: 910043fd  add x29, sp, #0x10
  4000ac: d65f03c0  ret

00000000004000b0 <function4>:
  4000b0: d10083ff  sub sp, sp, #0x20
  4000b4: a9017bfd  stp x29, x30, [sp, #16]
  4000b8: 6d0107e0  stp d0, d1, [sp, #16]
  4000bc: a9017bfd  stp x29, x30, [sp, #16]
  4000c0: 910043fd  add x29, sp, #0x10
  4000c4: d65f03c0  ret
*/
TEST(FunctionScan, SimdStpPrologue) {
  auto memory_image = MemoryImage::Load(
      "disassembler/test_data/simd_stp_prologue.memory_image");
  ASSERT_NE(memory_image, nullptr);

  std::set<uint64_t> results = FunctionScan(*memory_image);
  EXPECT_EQ(results.size(), 5);
  EXPECT_NE(results.find(0x400078), results.end());
  EXPECT_NE(results.find(0x400088), results.end());
  EXPECT_NE(results.find(0x400098), results.end());
  EXPECT_NE(results.find(0x4000a4), results.end());
  EXPECT_NE(results.find(0x4000b0), results.end());
}
}  // namespace test
}  // namespace disassembler
}  // namespace reil

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
