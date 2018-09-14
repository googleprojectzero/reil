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

"""External dependencies for reil rules."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def reil_repositories(
  omit_com_google_abseil=False,
  omit_com_github_gflags_gflags=False,
  omit_com_google_binexport=False,
  omit_com_google_glog=False,
  omit_com_google_googletest=False,
  omit_com_google_protobuf=False):

  if not omit_com_google_abseil:
    com_google_abseil()
  if not omit_com_github_gflags_gflags:
    com_github_gflags_gflags()
  if not omit_com_google_binexport:
    com_google_binexport()
  if not omit_com_google_glog:
    com_google_glog()
  if not omit_com_google_googletest:
    com_google_googletest()
  if not omit_com_google_protobuf:
    com_google_protobuf()

def com_google_abseil():
  http_archive(
    name = "com_google_abseil",
    sha256 = "21d6c16537dc4e6f368020402828671b71183a8b4be8a87b4f56f102a6b0708d",
    strip_prefix = "abseil-cpp-6c7e5ffc43decd92f7bdfc510ad8a245a20b6dea",
    urls = ["https://github.com/abseil/abseil-cpp/archive/6c7e5ffc43decd92f7bdfc510ad8a245a20b6dea.tar.gz"]
  )

def com_github_gflags_gflags():
  http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "bc13ca1509a06693b77780b5ed0bd7e581a176c94acfc4be3f1b78885e09d313",
    strip_prefix = "gflags-660603a3df1c400437260b51c55490a046a12e8a",
    urls = ["https://github.com/gflags/gflags/archive/660603a3df1c400437260b51c55490a046a12e8a.tar.gz"]
  )

def com_google_binexport():
  http_archive(
    name = "com_google_binexport",
    sha256 = "",
    strip_prefix = "binexport-68fdccf3966d762f5ed872ccc4e515d3016cb452",
    urls = ["https://github.com/google/binexport/archive/68fdccf3966d762f5ed872ccc4e515d3016cb452.tar.gz"],
    build_file = "binexport.BUILD.bazel"
  )

def com_google_glog():
  http_archive(
    name = "com_google_glog",
    sha256 = "2e0ac963a68216855ce4d1d8cc5e2226254e0e6bc4d70fd2fa0300c6fa0fefef",
    strip_prefix = "glog-8d7a107d68c127f3f494bb7807b796c8c5a97a82",
    urls = ["https://github.com/google/glog/archive/8d7a107d68c127f3f494bb7807b796c8c5a97a82.tar.gz"]
  )

def com_google_googletest():
  http_archive(
    name = "com_google_googletest",
    sha256 = "062d050c04f2baba9b9e07a7dd1083e608556e2516aa0a85d2ef79178121c2a7",
    strip_prefix = "googletest-b8e2562086ef326a4a88089f85b29ce7b6f9464b",
    urls = ["https://github.com/google/googletest/archive/b8e2562086ef326a4a88089f85b29ce7b6f9464b.tar.gz"]
  )

def com_google_protobuf():
  http_archive(
    name = "com_google_protobuf",
    sha256 = "cef7f1b5a7c5fba672bec2a319246e8feba471f04dcebfe362d55930ee7c1c30",
    strip_prefix = "protobuf-3.5.0",
    urls = ["https://github.com/google/protobuf/archive/v3.5.0.zip"]
  )
