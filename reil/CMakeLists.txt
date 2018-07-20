# Copyright 2018 Google LLC
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     https://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.5.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(reil)

if (DEFINED ABSEIL_INCLUDE_DIR)
  include_directories(${ABSEIL_INCLUDE_DIR})
else()
  message(FATAL_ERROR "ABSEIL_INCLUDE_DIR was not supplied")
endif()

if (NOT DEFINED ABSEIL_LIBRARIES)
  message(FATAL_ERROR "ABSEIL_LIBRARIES was not supplied")
endif()

if (DEFINED GOOGLETEST_INCLUDE_DIR)
  include_directories(${GOOGLETEST_INCLUDE_DIR})
else()
  message(FATAL_ERROR "GOOGLETEST_INCLUDE_DIR was not supplied")
endif()

if (NOT DEFINED GOOGLETEST_LIBRARIES)
  message(FATAL_ERROR "GOOGLETEST_LIBRARIES was not supplied")
endif()

include_directories("${CMAKE_SOURCE_DIR}/../")

set(REIL_INCLUDE_FILES
       aarch64.h aarch64/decoder.h aarch64/translator.h aarch64/emulator.h
       emulator.h immediate.h interpreter.h reil.h translation.h)

set(REIL_SOURCE_FILES
       aarch64/decoder.cpp aarch64/printer.cpp aarch64/translator.cpp aarch64/emulator.cpp 
       emulator.cpp immediate.cpp interpreter.cpp reil.cpp translation.cpp)

add_library(reil
    ${REIL_INCLUDE_FILES}
    ${REIL_SOURCE_FILES})
target_compile_features(reil PUBLIC cxx_std_11)
target_link_libraries(reil ${ABSEIL_LIBRARIES})

add_executable(aarch64_test
    test/aarch64_test.cpp)
target_link_libraries(aarch64_test reil pthread ${GOOGLETEST_LIBRARIES})

add_executable(immediate_test
    immediate.cpp
    test/immediate_test.cpp)
target_link_libraries(immediate_test pthread ${GOOGLETEST_LIBRARIES})

add_executable(aarch64_fuzz
    test/aarch64_fuzz.cpp)
target_link_libraries(aarch64_fuzz reil)