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

#ifndef REIL_ANALYSIS_TEMPORARY_STATE_H_

#include "analysis/map_storage.h"
#include "reil/reil.h"

namespace reil {
namespace analysis {
template <typename T, typename TOps>
class TemporaryState {
  using Storage = MapStorage<T, TOps>;
  Storage temporaries_;

 public:
  uint16_t TemporaryCount() const { return temporaries_.Count(); }

  T GetTemporary(uint16_t index) const { return temporaries_.Get(index); }

  T GetTemporary(const Temporary& tmp) const {
    return temporaries_.Get(tmp.index);
  }

  void SetTemporary(uint16_t index, const T& value) {
    temporaries_.Set(index, value);
  }

  void SetTemporary(const Temporary& tmp, const T& value) {
    temporaries_.Set(tmp.index, value);
  }

  void SetTemporary(uint16_t index, T&& value) {
    temporaries_.Set(index, std::move(value));
  }

  void SetTemporary(const Temporary& tmp, T&& value) {
    temporaries_.Set(tmp.index, std::move(value));
  }

  void ClearTemporary(uint16_t index) { temporaries_.Clear(index); }

  void ClearTemporary(const Temporary& tmp) { temporaries_.Clear(tmp.index); }

  void ClearTemporaries() { temporaries_.Clear(); }

  void MergeTemporaries(const TemporaryState<T, TOps>& other) {
    temporaries_.Merge(other.temporaries_);
  }

  bool CompareTemporaries(const TemporaryState<T, TOps>& other) const {
    return temporaries_ == other.temporaries_;
  }

  typename Storage::const_iterator TemporariesBegin() const {
    return temporaries_.begin();
  }

  typename Storage::iterator TemporariesBegin() { return temporaries_.begin(); }

  typename Storage::const_iterator TemporariesEnd() const {
    return temporaries_.end();
  }

  typename Storage::iterator TemporariesEnd() { return temporaries_.end(); }
};
}  // namespace analysis
}  // namespace reil

#define REIL_ANALYSIS_TEMPORARY_STATE_H_
#endif  // REIL_ANALYSIS_TEMPORARY_STATE_H_
