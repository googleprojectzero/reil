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

workspace(name = "reil")

local_repository(name = 'reil_rules', path = '.')
load("@reil_rules//:repositories.bzl", "reil_repositories")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "com_google_protobuf",
  sha256 = "758249b537abba2f21ebc2d02555bf080917f0f2f88f4cbe2903e0e28c4187ed",
  strip_prefix = "protobuf-3.10.0",
  urls = ["https://github.com/google/protobuf/archive/v3.10.0.tar.gz"]
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")
protobuf_deps()

reil_repositories()
