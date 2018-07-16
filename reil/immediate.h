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

#ifndef REIL_IMMEDIATE_H_

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

namespace reil {
class Immediate {
 private:
  uint16_t size_;
  std::vector<uint8_t> bytes_;

 public:
  Immediate();

  explicit Immediate(uint16_t size, uint64_t value = 0);
  Immediate(uint16_t size, const Immediate &value);
  Immediate(const std::vector<uint8_t> &bytes);
  Immediate(const uint8_t *bytes, size_t bytes_len);

  Immediate(const Immediate &value);

  uint16_t size() const;
  std::vector<uint8_t> bytes();

  static Immediate Mask(uint16_t size, uint16_t mask_size);
  static Immediate SignBit(uint16_t size, uint16_t mask_size);
  static Immediate CarryBit(uint16_t size, uint16_t mask_size);
  static Immediate SignedMin(uint16_t size, uint16_t mask_size);
  static Immediate SignedMax(uint16_t size, uint16_t mask_size);
  static Immediate UnsignedMax(uint16_t size, uint16_t mask_size);

  static Immediate Mask(uint16_t size);
  static Immediate SignBit(uint16_t size);
  static Immediate CarryBit(uint16_t size);
  static Immediate SignedMin(uint16_t size);
  static Immediate SignedMax(uint16_t size);
  static Immediate UnsignedMax(uint16_t size);

  Immediate Extract(uint16_t size, uint16_t offset = 0) const;
  Immediate ZeroExtend(uint16_t size) const;
  Immediate SignExtend(uint16_t size) const;

  explicit operator bool() const;
  explicit operator uint8_t() const;
  explicit operator uint16_t() const;
  explicit operator uint32_t() const;
  explicit operator uint64_t() const;

  bool operator==(const Immediate &other) const;
  bool operator<(const Immediate &other) const;
  bool operator<=(const Immediate &other) const;

  bool operator!=(const Immediate &other) const;
  bool operator>(const Immediate &other) const;
  bool operator>=(const Immediate &other) const;

  friend Immediate lattice_multiply(const Immediate &lhs, const Immediate &rhs);
  friend Immediate karatsuba_multiply(const Immediate &lhs,
                                      const Immediate &rhs);

  friend Immediate binary_long_divide(const Immediate &lhs,
                                      const Immediate &rhs);
  friend Immediate binary_long_modulus(const Immediate &lhs,
                                       const Immediate &rhs);

  friend Immediate operator+(const Immediate &lhs, const Immediate &rhs);
  friend Immediate operator-(const Immediate &lhs, const Immediate &rhs);
  friend Immediate operator*(const Immediate &lhs, const Immediate &rhs);
  friend Immediate operator/(const Immediate &lhs, const Immediate &rhs);
  friend Immediate operator%(const Immediate &lhs, const Immediate &rhs);

  Immediate &operator|=(const Immediate &rhs);
  Immediate &operator&=(const Immediate &rhs);
  Immediate &operator^=(const Immediate &rhs);

  Immediate &operator<<=(const Immediate &rhs);
  Immediate &operator>>=(const Immediate &rhs);

  Immediate &operator<<=(uint16_t rhs);
  Immediate &operator>>=(uint16_t rhs);

  friend Immediate operator&(Immediate lhs, const Immediate &rhs);
  friend Immediate operator|(Immediate lhs, const Immediate &rhs);
  friend Immediate operator^(Immediate lhs, const Immediate &rhs);

  friend Immediate operator<<(Immediate lhs, const Immediate &rhs);
  friend Immediate operator>>(Immediate lhs, const Immediate &rhs);

  friend Immediate operator<<(Immediate lhs, uint16_t rhs);
  friend Immediate operator>>(Immediate lhs, uint16_t rhs);

  friend std::ostream &operator<<(std::ostream &stream, const Immediate &imm);
};

}  // namespace reil

#define REIL_IMMEDIATE_H_
#endif  // REIL_IMMEDIATE_H_
