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

#ifndef REIL_ANALYSIS_MAP_STORAGE_H_

#include <map>

#include "analysis/value/operations.h"

namespace reil {
namespace analysis {
template <typename T, typename TOps>
class MapStorage {
  std::map<uint16_t, T> values_;

 public:
  typedef typename std::map<uint16_t, T>::const_iterator const_iterator;
  typedef typename std::map<uint16_t, T>::iterator iterator;

  uint16_t Count() const { return static_cast<uint16_t>(values_.size()); }

  T Get(uint16_t index) const {
    T value;
    auto value_iter = values_.find(index);
    if (value_iter != values_.end()) {
      value = value_iter->second;
    }
    return value;
  }

  void Set(uint16_t index, const T& value) {
    auto value_iter = values_.find(index);
    if (value_iter != values_.end()) {
      value_iter->second = value;
    } else {
      values_.emplace(std::make_pair(index, value));
    }
  }

  void Set(uint16_t index, T&& value) {
    auto value_iter = values_.find(index);
    if (value_iter != values_.end()) {
      value_iter->second = std::move(value);
    } else {
      values_.emplace(std::make_pair(index, std::move(value)));
    }
  }

  void Clear(uint16_t index) {
    auto value_iter = values_.find(index);
    if (value_iter != values_.end()) {
      values_.erase(value_iter);
    }
  }

  void Clear() { values_.clear(); }

  void Merge(const MapStorage<T, TOps>& other) {
    auto iter = values_.begin();
    auto other_iter = other.values_.begin();

    while (other_iter != other.values_.end()) {
      if (iter == values_.end()) {
        break;
      }

      while (iter->first < other_iter->first) {
        iter++;
      }

      if (iter->first == other_iter->first) {
        iter->second = TOps::Merge(iter->second, other_iter->second);
        iter++;
      } else {
        values_.emplace(*other_iter);
      }

      other_iter++;
    }

    while (other_iter != other.values_.end()) {
      values_.emplace(*other_iter++);
    }
  }

  const_iterator begin() const { return values_.begin(); }

  iterator begin() { return values_.begin(); }

  const_iterator end() const { return values_.end(); }

  iterator end() { return values_.end(); }

  template <typename S, typename SOps>
  friend bool operator==(const MapStorage<S, SOps>& lhs, const MapStorage<S, SOps>& rhs);
};

template <typename T, typename TOps>
bool operator==(const MapStorage<T, TOps>& lhs, const MapStorage<T, TOps>& rhs) {
  return lhs.values_ == rhs.values_;
}

template <typename T, typename TOps>
bool operator!=(const MapStorage<T, TOps>& lhs, const MapStorage<T, TOps>& rhs) {
  return !(lhs.values_ == rhs.values_);
}
}  // namespace analysis
}  // namespace reil

#define REIL_ANALYSIS_MAP_STORAGE_H_
#endif  // REIL_ANALYSIS_MAP_STORAGE_H_
