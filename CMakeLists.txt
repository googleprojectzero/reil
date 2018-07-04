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

cmake_minimum_required(VERSION 3.3.0)

include(ExternalProject)

ExternalProject_Add(abseil
    GIT_REPOSITORY      "https://github.com/abseil/abseil-cpp"
    GIT_TAG             "master"
    UPDATE_DISCONNECTED 1
    INSTALL_COMMAND     ""
  )

ExternalProject_Get_Property(abseil SOURCE_DIR)
ExternalProject_Get_Property(abseil BINARY_DIR)
set(ABSEIL_INCLUDE_DIR "${SOURCE_DIR}")
set(ABSEIL_LIBRARIES "${BINARY_DIR}/absl/types/libabsl_variant.a")

ExternalProject_Add(googletest
    GIT_REPOSITORY      "https://github.com/google/googletest"
    GIT_TAG             "master"
    CMAKE_ARGS          "-DBUILD_GMOCK=OFF"
    CMAKE_ARGS          "-DBUILD_GTEST=ON"
    UPDATE_DISCONNECTED 1
    INSTALL_COMMAND     ""
  )

ExternalProject_Get_Property(googletest SOURCE_DIR)
ExternalProject_Get_Property(googletest BINARY_DIR)
set(GOOGLETEST_INCLUDE_DIR "${SOURCE_DIR}/googletest/include/")
set(GOOGLETEST_LIBRARIES "${BINARY_DIR}/googletest/libgtest.a")

ExternalProject_Add(unicorn
    GIT_REPOSITORY      "https://github.com/unicorn-engine/unicorn.git"
    GIT_TAG             "master"
    BUILD_IN_SOURCE     1
    UPDATE_DISCONNECTED 1
    CONFIGURE_COMMAND   ""
    INSTALL_COMMAND     ""
    BUILD_COMMAND       "./make.sh"
  )

ExternalProject_Get_Property(unicorn SOURCE_DIR)
set(UNICORN_INCLUDE_DIR "${SOURCE_DIR}/include")
set(UNICORN_LIBRARIES "${SOURCE_DIR}/libunicorn.a")

ExternalProject_Add(reil
    DEPENDS             abseil googletest unicorn
    CMAKE_ARGS          "-DABSEIL_INCLUDE_DIR=${ABSEIL_INCLUDE_DIR}"
    CMAKE_ARGS          "-DABSEIL_LIBRARIES=${ABSEIL_LIBRARIES}"
    CMAKE_ARGS          "-DGOOGLETEST_INCLUDE_DIR=${GOOGLETEST_INCLUDE_DIR}"
    CMAKE_ARGS          "-DGOOGLETEST_LIBRARIES=${GOOGLETEST_LIBRARIES}"
    CMAKE_ARGS          "-DUNICORN_INCLUDE_DIR=${UNICORN_INCLUDE_DIR}"
    CMAKE_ARGS          "-DUNICORN_LIBRARIES=${UNICORN_LIBRARIES}"
    SOURCE_DIR          "${CMAKE_SOURCE_DIR}/reil"
    INSTALL_COMMAND     ""
  )