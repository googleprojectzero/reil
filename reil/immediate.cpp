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

#include "immediate.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iomanip>

namespace reil {

Immediate::Immediate() : size_(0), bytes_(0) {}

Immediate::Immediate(uint16_t size, uint64_t value)
    : size_(size), bytes_(size / 8) {
  if (value) {
    memcpy(bytes_.data(), &value, std::min(bytes_.size(), sizeof(value)));
  }
}

Immediate::Immediate(uint16_t size, const Immediate &value)
    : size_(size), bytes_(size / 8) {
  memcpy(bytes_.data(), value.bytes_.data(),
         std::min(bytes_.size(), value.bytes_.size()));
}

Immediate::Immediate(const std::vector<uint8_t> &bytes)
    : Immediate(bytes.data(), bytes.size()) {}

Immediate::Immediate(const uint8_t *bytes, size_t bytes_len)
    : size_(bytes_len * 8), bytes_(bytes_len) {
  memcpy(bytes_.data(), bytes, bytes_len);
}

Immediate::Immediate(const Immediate &value)
    : size_(value.size_), bytes_(value.bytes_) {}

uint16_t Immediate::size() const { return size_; }

std::vector<uint8_t> Immediate::bytes() { return bytes_; }

Immediate Immediate::Extract(uint16_t size, uint16_t offset) const {
  assert(offset < size_ && size <= size_ - offset);

  if (offset) {
    Immediate tmp = *this >> offset;
    return Immediate(size, tmp);
  } else {
    return Immediate(size, *this);
  }
}

Immediate Immediate::ZeroExtend(uint16_t size) const {
  Immediate result(size, *this);
  return result;
}

Immediate Immediate::SignExtend(uint16_t size) const {
  Immediate result(size, *this);

  if (Immediate::SignBit(size_, size_) & *this) {
    result |= Immediate::Mask(size, size - size_) << size_;
  }

  return result;
}

Immediate::operator bool() const {
  for (uint16_t i = 0; i < bytes_.size(); ++i) {
    if (bytes_[i]) return true;
  }
  return false;
}

Immediate::operator uint8_t() const {
  uint8_t value = 0;
  memcpy(&value, bytes_.data(), std::min(bytes_.size(), sizeof(value)));
  return value;
}

Immediate::operator uint16_t() const {
  uint16_t value = 0;
  memcpy(&value, bytes_.data(), std::min(bytes_.size(), sizeof(value)));
  return value;
}

Immediate::operator uint32_t() const {
  uint32_t value = 0;
  memcpy(&value, bytes_.data(), std::min(bytes_.size(), sizeof(value)));
  return value;
}

Immediate::operator uint64_t() const {
  uint64_t value = 0;
  memcpy(&value, bytes_.data(), std::min(bytes_.size(), sizeof(value)));
  return value;
}

bool Immediate::operator==(const Immediate &other) const {
  assert(size_ == other.size_);

  return (memcmp(bytes_.data(), other.bytes_.data(), bytes_.size()) == 0);
}

bool Immediate::operator<(const Immediate &other) const {
  assert(size_ == other.size_);

  uint16_t byte_width = bytes_.size();
  for (uint16_t i = 1; i <= byte_width; ++i) {
    if (bytes_[byte_width - i] < other.bytes_[byte_width - i]) {
      return true;
    } else if (bytes_[byte_width - i] >= other.bytes_[byte_width - i]) {
      return false;
    }
  }

  return false;
}

bool Immediate::operator<=(const Immediate &other) const {
  assert(size_ == other.size_);

  uint16_t byte_width = bytes_.size();
  for (uint16_t i = 1; i <= byte_width; ++i) {
    if (bytes_[byte_width - i] < other.bytes_[byte_width - i]) {
      return true;
    } else if (bytes_[byte_width - i] > other.bytes_[byte_width - i]) {
      return false;
    }
  }

  return true;
}

bool Immediate::operator!=(const Immediate &other) const {
  return !(*this == other);
}

bool Immediate::operator>(const Immediate &other) const {
  return other < *this;
}

bool Immediate::operator>=(const Immediate &other) const {
  return other <= *this;
}

Immediate Immediate::Mask(uint16_t size, uint16_t mask_size) {
  Immediate mask(mask_size);
  memset(mask.bytes_.data(), 0xff, mask.bytes_.size());
  return mask.ZeroExtend(size);
}

Immediate Immediate::SignBit(uint16_t size, uint16_t mask_size) {
  Immediate sign_bit(size, 1);
  sign_bit <<= mask_size - 1;
  return sign_bit;
}

Immediate Immediate::CarryBit(uint16_t size, uint16_t mask_size) {
  Immediate carry_bit(size, 1);
  carry_bit <<= mask_size;
  return carry_bit;
}

Immediate Immediate::SignedMin(uint16_t size, uint16_t mask_size) {
  Immediate signed_min = Mask(size);
  signed_min <<= mask_size - 1;
  return signed_min;
}

Immediate Immediate::SignedMax(uint16_t size, uint16_t mask_size) {
  Immediate signed_max = Mask(size, mask_size);
  signed_max >>= 1;
  return signed_max;
}

Immediate Immediate::UnsignedMax(uint16_t size, uint16_t mask_size) {
  return Immediate::Mask(size, mask_size);
}

Immediate Immediate::Mask(uint16_t size) { return Immediate::Mask(size, size); }

Immediate Immediate::SignBit(uint16_t size) {
  return Immediate::SignBit(size, size);
}

Immediate Immediate::CarryBit(uint16_t size) {
  return Immediate::CarryBit(size * 2, size);
}

Immediate Immediate::SignedMin(uint16_t size) {
  return Immediate::SignedMin(size, size);
}

Immediate Immediate::SignedMax(uint16_t size) {
  return Immediate::SignedMax(size, size);
}

Immediate Immediate::UnsignedMax(uint16_t size) {
  return Immediate::UnsignedMax(size, size);
}

Immediate operator+(const Immediate &lhs, const Immediate &rhs) {
  assert(lhs.size_ == rhs.size_);

  Immediate result(lhs.size_ * 2);

  uint8_t i = 0;
  uint16_t tmp = 0;

  for (i = 0; i < lhs.bytes_.size(); ++i) {
    tmp += lhs.bytes_[i] + rhs.bytes_[i];
    result.bytes_[i] = (uint8_t)tmp;
    tmp >>= 8;
  }

  result.bytes_[i++] = tmp;
  for (; i < result.bytes_.size(); ++i) {
    result.bytes_[i] = 0;
  }

  return result;
}

Immediate operator-(const Immediate &lhs, const Immediate &rhs) {
  assert(lhs.size_ == rhs.size_);

  Immediate result(lhs.size_ * 2);

  uint8_t i = 0;
  int16_t tmp = 0;

  for (i = 0; i < lhs.bytes_.size(); ++i) {
    tmp += lhs.bytes_[i] - rhs.bytes_[i];
    result.bytes_[i] = (uint8_t)tmp;
    tmp >>= 8;
  }

  for (; i < result.bytes_.size(); ++i) {
    result.bytes_[i] = (uint8_t)tmp;
  }

  return result;
}

Immediate lattice_multiply(const Immediate &lhs, const Immediate &rhs) {
  // this is much faster than karatsuba for medium sized integers, since we
  // need only do a very small number of allocations.

  // we are limited in the size of computation we can perform by the size of
  // the temporary that we use. note that in the worst case, every element of
  // the carry lattice is 0xfe.

  // the sum for a diagonal is bounded by
  //    0xff * byte_width
  // and the sum for the corresponding carry diagonal is bounded by
  //    0xfe * (byte_width - 1)
  // and we require that this not overflow a 16-bit integer; we can compute a
  // loose upper bound for byte_width by
  //    0x10000 > (0xff + 0xfe) * byte_width
  //    byte_width < 0x80

  uint16_t byte_width = lhs.bytes_.size();

  Immediate result(lhs.size_ * 2);

  size_t lattice_size = lhs.bytes_.size() * rhs.bytes_.size();
  std::vector<uint8_t> lattice(lattice_size);
  std::vector<uint8_t> carry_lattice(lattice_size);

  uint16_t tmp = 0;
  uint16_t byte_value = 0;
  uint16_t byte_carry = 0;

  for (uint16_t j = 0; j < byte_width; ++j) {
    for (uint16_t i = 0; i < byte_width; ++i) {
      tmp = lhs.bytes_[i] * rhs.bytes_[j];
      lattice[(i * byte_width) + j] = (uint8_t)tmp;
      carry_lattice[(i * byte_width) + j] = (uint8_t)(tmp >> 8);
    }
  }

  for (uint16_t i = 0; i < byte_width * 2; ++i) {
    byte_value >>= 8;
    byte_value += byte_carry;
    byte_carry = 0;
    for (uint16_t x = 0; x < byte_width; ++x) {
      for (uint16_t y = 0; y < byte_width; ++y) {
        if (x + y == i) {
          byte_value += lattice[(x * byte_width) + y];
          byte_carry += carry_lattice[(x * byte_width) + y];
        }
      }
    }
    result.bytes_[i] = (uint8_t)byte_value;
  }

  return result;
}

Immediate karatsuba_multiply(const Immediate &lhs, const Immediate &rhs) {
  // variant of karatsuba algorithm for values too large for the lattice
  // algorithm to handle reasonably.
  //
  // where x = 2^m.x_1 + x_0
  //       y = 2^m.y_1 + y_0
  //
  // xy = 2^2m.x_1.y_1 + 2^m.(x_1.y_0 + x_0.y_1) + x_0.y_0
  //
  //    = 2^2m.x_1.y_1
  //      + 2^m.((x_1 - x_0)(y_1 - y_0) + x_1.y_1 + x_0.y_0)
  //      + x_0.y_0
  //
  //    = 2^2m.x_1.y_1 + 2^m.x_1.y_1
  //      + 2^m.(x_1 - x_0).(y_1 - y_0)
  //      + 2^m.x_0.y_0 + x_0.y_0

  uint16_t n = lhs.size_;
  uint16_t m = n / 2;

  auto x_0 = lhs.Extract(m);
  auto x_1 = lhs.Extract(m, m);

  auto y_0 = rhs.Extract(m);
  auto y_1 = rhs.Extract(m, m);

  auto x_0_y_0 = x_0 * y_0;
  auto x_1_y_1 = x_1 * y_1;

  bool negative = true;
  Immediate d_x, d_y;
  if (x_0 > x_1) {
    d_x = (x_0 - x_1).Extract(m);
  } else {
    d_x = (x_1 - x_0).Extract(m);
    negative = !negative;
  }

  if (y_0 > y_1) {
    d_y = (y_0 - y_1).Extract(m);
  } else {
    d_y = (y_1 - y_0).Extract(m);
    negative = !negative;
  }

  auto d_x_d_y = d_x * d_y;

  auto tmp1 = ((x_0_y_0.ZeroExtend(n * 2) << m) + x_0_y_0.ZeroExtend(n * 2))
                  .Extract(n * 2);
  auto tmp2 = d_x_d_y.ZeroExtend(n * 2) << m;
  auto tmp3 = ((x_1_y_1.ZeroExtend(n * 2) << (m * 2)) +
               (x_1_y_1.ZeroExtend(n * 2) << m));

  Immediate result;
  if (!negative) {
    result = ((tmp1 + tmp2) + tmp3).Extract(n * 2);
  } else {
    result = ((tmp1 - tmp2) + tmp3).Extract(n * 2);
  }

  return result;
}

Immediate operator*(const Immediate &lhs, const Immediate &rhs) {
  assert(lhs.size_ == rhs.size_);
  uint16_t byte_width = lhs.bytes_.size();

  if (byte_width <= 4) {
    uint64_t lhs64 = static_cast<uint64_t>(lhs);
    uint64_t rhs64 = static_cast<uint64_t>(rhs);
    return Immediate(64, lhs64 * rhs64);
  } else if (byte_width < 0x80) {
    return lattice_multiply(lhs, rhs);
  } else {
    return karatsuba_multiply(lhs, rhs);
  }
}

Immediate binary_long_divide(const Immediate &lhs, const Immediate &rhs) {
  Immediate quotient(lhs.size_);

  // binary long division
  Immediate dividend = lhs.ZeroExtend(lhs.size_ + 8);
  uint16_t dividend_width = dividend.bytes_.size();

  Immediate divisor = rhs.ZeroExtend(rhs.size_ + 8);
  uint16_t divisor_width = divisor.bytes_.size();

  // first we normalise, to make sure we only do the minimum necessary  work.

  uint16_t dividend_shift = 0;
  for (uint16_t i = 1; i <= dividend_width; ++i) {
    uint8_t dividend_byte = dividend.bytes_[dividend_width - i];
    if (dividend_byte != 0) {
      dividend_shift = 8 * (i - 1);
      while (!(dividend_byte & 0x80)) {
        dividend_byte <<= 1;
        ++dividend_shift;
      }
      break;
    }
  }

  uint16_t divisor_shift = 0;
  for (uint16_t i = 1; i <= divisor_width; ++i) {
    uint8_t divisor_byte = divisor.bytes_[divisor_width - i];
    if (divisor_byte != 0) {
      divisor_shift = 8 * (i - 1);
      while (!(divisor_byte & 0x80)) {
        divisor_byte <<= 1;
        ++divisor_shift;
      }
      break;
    }
  }

  std::cerr << dividend_shift << " " << divisor_shift << std::endl;

  uint16_t bit = divisor_shift - dividend_shift;
  divisor <<= bit;
  if (divisor >= dividend) {
    divisor >>= 1;
    bit -= 1;
  }

  // now we compute the quotient in max(bit) subtractions and shifts.

  while (bit) {
    std::cerr << bit << std::endl;
    std::cerr << quotient << std::endl;
    if (dividend >= divisor) {
      dividend = (dividend - divisor).Extract(lhs.size_ + 8);
      quotient.bytes_[bit / 8] |= 1 << (bit % 8);
    }

    bit -= 1;
    divisor >>= 1;
  }

  if (dividend >= divisor) {
    dividend = (dividend - divisor).Extract(lhs.size_ + 8);
    quotient.bytes_[0] |= 1;
  }

  return quotient;
}

Immediate binary_long_modulus(const Immediate &lhs, const Immediate &rhs) {
  // binary long division
  Immediate dividend = lhs.ZeroExtend(lhs.size_ + 8);
  uint16_t dividend_width = dividend.bytes_.size();

  Immediate divisor = rhs.ZeroExtend(rhs.size_ + 8);
  uint16_t divisor_width = divisor.bytes_.size();

  // first we normalise, to make sure we only do the minimum necessary  work.

  uint16_t dividend_shift = 0;
  for (uint16_t i = 1; i <= dividend_width; ++i) {
    uint8_t dividend_byte = dividend.bytes_[dividend_width - i];
    if (dividend_byte != 0) {
      dividend_shift = 8 * (i - 1);
      while (!(dividend_byte & 0x80)) {
        dividend_byte <<= 1;
        ++dividend_shift;
      }
      break;
    }
  }

  uint16_t divisor_shift = 0;
  for (uint16_t i = 1; i <= divisor_width; ++i) {
    uint8_t divisor_byte = divisor.bytes_[divisor_width - i];
    if (divisor_byte != 0) {
      divisor_shift = 8 * (i - 1);
      while (!(divisor_byte & 0x80)) {
        divisor_byte <<= 1;
        ++divisor_shift;
      }
      break;
    }
  }

  uint16_t bit = divisor_shift - dividend_shift;
  divisor <<= bit;
  if (divisor >= dividend) {
    divisor >>= 1;
    bit -= 1;
  }

  // now we compute the dividend in max(bit) subtractions and shifts.

  while (bit) {
    if (dividend >= divisor) {
      dividend = (dividend - divisor).Extract(lhs.size_ + 8);
    }

    bit -= 1;
    divisor >>= 1;
  }

  if (dividend >= divisor) {
    dividend = (dividend - divisor).Extract(lhs.size_ + 8);
  }

  return dividend;
}

Immediate operator/(const Immediate &lhs, const Immediate &rhs) {
  assert(lhs.size_ == rhs.size_);
  Immediate quotient(lhs.size_);
  uint16_t byte_width = lhs.bytes_.size();

  if (!rhs) {
    // TODO: raise divide by zero exception
    return quotient;
  }

  if (lhs == rhs) {
    // quotient is 1
    quotient.bytes_[0] = 1;
  } else if (byte_width == 1) {
    uint8_t lhs8 = static_cast<uint8_t>(lhs);
    uint8_t rhs8 = static_cast<uint8_t>(rhs);
    return Immediate(8, lhs8 / rhs8);
  } else if (byte_width == 2) {
    uint16_t lhs16 = static_cast<uint16_t>(lhs);
    uint16_t rhs16 = static_cast<uint16_t>(rhs);
    return Immediate(16, lhs16 / rhs16);
  } else if (byte_width == 4) {
    uint32_t lhs32 = static_cast<uint32_t>(lhs);
    uint32_t rhs32 = static_cast<uint32_t>(rhs);
    return Immediate(32, lhs32 / rhs32);
  } else if (byte_width == 8) {
    uint64_t lhs64 = static_cast<uint64_t>(lhs);
    uint64_t rhs64 = static_cast<uint64_t>(rhs);
    return Immediate(64, lhs64 / rhs64);
  } else if (rhs < lhs) {
    return binary_long_divide(lhs, rhs);
  } else {
    // do nothing, quotient is zero
  }

  return quotient;
}

Immediate operator%(const Immediate &lhs, const Immediate &rhs) {
  assert(lhs.size_ == rhs.size_);
  uint16_t byte_width = lhs.bytes_.size();

  if (!rhs) {
    // TODO: raise divide by zero exception
    return Immediate(lhs.size_);
  }

  if (lhs == rhs) {
    // quotient is 1
    return Immediate(lhs.size_);
  } else if (byte_width == 1) {
    uint8_t lhs8 = static_cast<uint8_t>(lhs);
    uint8_t rhs8 = static_cast<uint8_t>(rhs);
    return Immediate(8, lhs8 % rhs8);
  } else if (byte_width == 2) {
    uint16_t lhs16 = static_cast<uint16_t>(lhs);
    uint16_t rhs16 = static_cast<uint16_t>(rhs);
    return Immediate(16, lhs16 % rhs16);
  } else if (byte_width == 4) {
    uint32_t lhs32 = static_cast<uint32_t>(lhs);
    uint32_t rhs32 = static_cast<uint32_t>(rhs);
    return Immediate(32, lhs32 % rhs32);
  } else if (byte_width == 8) {
    uint64_t lhs64 = static_cast<uint64_t>(lhs);
    uint64_t rhs64 = static_cast<uint64_t>(rhs);
    return Immediate(64, lhs64 % rhs64);
  } else if (rhs < lhs) {
    return binary_long_modulus(lhs, rhs);
  } else {
    return lhs;
  }
}

Immediate operator&(Immediate lhs, const Immediate &rhs) {
  lhs &= rhs;
  return lhs;
}

Immediate operator|(Immediate lhs, const Immediate &rhs) {
  lhs |= rhs;
  return lhs;
}

Immediate operator^(Immediate lhs, const Immediate &rhs) {
  lhs ^= rhs;
  return lhs;
}

Immediate &Immediate::operator&=(const Immediate &rhs) {
  assert(size_ == rhs.size_);

  for (uint16_t i = 0; i < bytes_.size(); ++i) {
    bytes_[i] &= rhs.bytes_[i];
  }

  return *this;
}

Immediate &Immediate::operator|=(const Immediate &rhs) {
  assert(size_ == rhs.size_);

  for (uint16_t i = 0; i < bytes_.size(); ++i) {
    bytes_[i] |= rhs.bytes_[i];
  }

  return *this;
}

Immediate &Immediate::operator^=(const Immediate &rhs) {
  assert(size_ == rhs.size_);

  for (uint16_t i = 0; i < bytes_.size(); ++i) {
    bytes_[i] ^= rhs.bytes_[i];
  }

  return *this;
}

Immediate &Immediate::operator<<=(const Immediate &rhs) {
  *this <<= static_cast<uint64_t>(rhs);
  return *this;
}

Immediate &Immediate::operator>>=(const Immediate &rhs) {
  *this >>= static_cast<uint64_t>(rhs);
  return *this;
}

Immediate &Immediate::operator<<=(uint16_t rhs) {
  uint16_t byte_width = bytes_.size();
  uint16_t byte_shift = rhs / 8;
  uint8_t bit_shift = rhs % 8;

  if (byte_shift >= byte_width) {
    memset(bytes_.data(), 0, byte_width);
  } else if (bit_shift) {
    for (uint16_t i = byte_width - 1; i > byte_shift; --i) {
      bytes_[i] = (bytes_[i - byte_shift] << bit_shift) |
                  (bytes_[i - byte_shift - 1] >> (8 - bit_shift));
    }
    bytes_[byte_shift] = bytes_[0] << bit_shift;
    memset(bytes_.data(), 0, byte_shift);
  } else {
    memmove(bytes_.data() + byte_shift, bytes_.data(), byte_width - byte_shift);
    memset(bytes_.data(), 0, byte_shift);
  }

  return *this;
}

Immediate &Immediate::operator>>=(uint16_t rhs) {
  uint16_t byte_width = bytes_.size();
  uint16_t byte_shift = rhs / 8;
  uint8_t bit_shift = rhs % 8;

  if (byte_shift >= byte_width) {
    memset(bytes_.data(), 0, byte_width);
  } else if (bit_shift) {
    for (uint16_t i = byte_shift; i < byte_width - 1; ++i) {
      bytes_[i - byte_shift] =
          (bytes_[i] >> bit_shift) | (bytes_[i + 1] << (8 - bit_shift));
    }
    bytes_[byte_width - 1 - byte_shift] = bytes_[byte_width - 1] >> bit_shift;
    memset(bytes_.data() + (byte_width - byte_shift), 0, byte_shift);
  } else {
    memmove(bytes_.data(), bytes_.data() + byte_shift, byte_width - byte_shift);
    memset(bytes_.data() + (byte_width - byte_shift), 0, byte_shift);
  }

  return *this;
}

Immediate operator<<(Immediate lhs, const Immediate &rhs) {
  lhs <<= rhs;
  return lhs;
}

Immediate operator>>(Immediate lhs, const Immediate &rhs) {
  lhs >>= rhs;
  return lhs;
}

Immediate operator<<(Immediate lhs, uint16_t rhs) {
  lhs <<= rhs;
  return lhs;
}

Immediate operator>>(Immediate lhs, uint16_t rhs) {
  lhs >>= rhs;
  return lhs;
}

std::ostream &operator<<(std::ostream &stream, const Immediate &imm) {
  auto old_flags = stream.flags();
  auto old_precision = stream.precision();
  auto old_fill = stream.fill();

  stream << std::nouppercase << std::setfill('0') << std::hex;
  for (uint16_t i = 1; i <= imm.bytes_.size(); ++i) {
    stream << std::setw(2) << (uint16_t)imm.bytes_[imm.bytes_.size() - i];
  }

  stream.flags(old_flags);
  stream.precision(old_precision);
  stream.fill(old_fill);

  return stream;
}
}  // namespace reil
