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

#ifndef REIL_ANALYSIS_REGISTER_STATE_H_

#include "analysis/map_storage.h"
#include "reil/aarch64.h"
#include "reil/reil.h"

namespace reil {
namespace analysis {
template <typename T, typename TOps>
class RegisterState {
  using Storage = MapStorage<T, TOps>;
  Storage registers_;

 public:
  uint16_t RegisterCount() const { return registers_.Count(); }

  T GetRegister(uint16_t index) const { return registers_.Get(index); }

  T GetRegister(const Register& reg) const { return registers_.Get(reg.index); }

  void SetRegister(uint16_t index, const T& value) {
    registers_.Set(index, value);
  }

  void SetRegister(const Register& reg, const T& value) {
    registers_.Set(reg.index, value);
  }

  void SetRegister(uint16_t index, T&& value) {
    registers_.Set(index, std::move(value));
  }

  void SetRegister(const Register& reg, T&& value) {
    registers_.Set(reg.index, std::move(value));
  }

  void ClearRegister(uint16_t index) { registers_.Clear(index); }

  void ClearRegister(const Register& reg) { registers_.Clear(reg.index); }

  void MergeRegisters(const RegisterState<T, TOps>& other) {
    registers_.Merge(other.registers_);
  }

  bool CompareRegisters(const RegisterState<T, TOps>& other) const {
    return registers_ == other.registers_;
  }

  typename Storage::const_iterator RegistersBegin() const {
    return registers_.begin();
  }

  typename Storage::iterator RegistersBegin() {
    return registers_.begin();
  }

  typename Storage::const_iterator RegistersEnd() const {
    return registers_.end();
  }

  typename Storage::iterator RegistersEnd() { return registers_.end(); }
};

template <typename T, typename TOps>
class AArch64RegisterState : public RegisterState<T, TOps> {
 public:
  std::string RegisterName(uint16_t index) const {
    return reil::aarch64::RegisterName(index);
  }

  using RegisterState<T, TOps>::GetRegister;
  using RegisterState<T, TOps>::SetRegister;
  using RegisterState<T, TOps>::ClearRegister;

  T GetRegister(std::string name) {
    return this->RegisterState<T, TOps>::GetRegister(
        reil::aarch64::RegisterIndex(name));
  }

  void SetRegister(std::string name, const T& value) {
    this->RegisterState<T, TOps>::SetRegister(reil::aarch64::RegisterIndex(name),
                                        value);
  }

  void SetRegister(std::string name, T&& value) {
    this->RegisterState<T, TOps>::SetRegister(reil::aarch64::RegisterIndex(name),
                                        std::move(value));
  }

  void ClearRegister(std::string name) {
    this->RegisterState<T, TOps>::ClearRegister(reil::aarch64::RegisterIndex(name));
  }
};
}  // namespace analysis
}  // namespace reil

#define REIL_ANALYSIS_REGISTER_STATE_H_
#endif  // REIL_ANALYSIS_REGISTER_STATE_H_
