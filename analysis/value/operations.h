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

#ifndef REIL_ANALYSIS_VALUE_OPERATIONS_H_

#include "absl/meta/type_traits.h"
#include "absl/types/variant.h"

namespace reil {
namespace analysis {
template <size_t i, typename... Ts>
struct args_impl;

template <size_t i, typename T, typename... Ts>
struct args_impl<i, T (*)(Ts...)> {
  typedef typename std::tuple_element<i, std::tuple<Ts...>>::type type;
};

template <size_t i, typename T>
using arg_t = typename args_impl<i, T>::type;

template <typename F>
using result_of_t = typename std::result_of<F>::type;

template <typename T>
using remove_const_reference_t =
    typename absl::remove_const_t<typename absl::remove_reference_t<T>>;

template <typename T, T F>
struct binary_operation_impl {
  static constexpr T function = F;
  using lhs_type = remove_const_reference_t<arg_t<0, T>>;
  using rhs_type = remove_const_reference_t<arg_t<1, T>>;
};

#define binary_operation(f) binary_operation_impl<decltype(&f), f>

template <typename T>
T BinaryOperation(const T& lhs, const T& rhs) {
  return T();
}

template <typename T, typename F, typename... Fs>
T BinaryOperation(const T& lhs, const T& rhs) {
  T result;
  using lhs_type = typename F::lhs_type;
  using rhs_type = typename F::rhs_type;
  if (absl::holds_alternative<lhs_type>(lhs) &&
      absl::holds_alternative<rhs_type>(rhs)) {
    result = F::function(absl::get<lhs_type>(lhs), absl::get<rhs_type>(rhs));
  } else {
    result = BinaryOperation<T, Fs...>(lhs, rhs);
  }
  return result;
}

template <typename T, T F>
struct resize_operation_impl {
  static constexpr T function = F;
  using value_type = remove_const_reference_t<arg_t<0, T>>;
};

#define resize_operation(f) resize_operation_impl<decltype(&f), f>

template <typename T>
T ResizeOperation(const T& value, uint16_t size) {
  return value;
}

template <typename T, typename F, typename... Fs>
T ResizeOperation(const T& value, uint16_t size) {
  T result;
  using value_type = typename F::value_type;
  if (absl::holds_alternative<value_type>(value)) {
    result = F::function(absl::get<value_type>(value), size);
  } else {
    result = ResizeOperation<T, Fs...>(value, size);
  }
  return result;
}

template <typename T, T F>
struct unary_operation_impl {
  static constexpr T function = F;
  using value_type = remove_const_reference_t<arg_t<0, T>>;
};

#define unary_operation(f) unary_operation_impl<decltype(&f), f>

template <typename S, typename T>
S UnaryOperation(const T& value) {
  S result = {};
  return result;
}

template <typename S, typename T, typename F, typename... Fs>
S UnaryOperation(const T& value) {
  S result = {};
  using value_type = typename F::value_type;
  if (absl::holds_alternative<value_type>(value)) {
    result = F::function(absl::get<value_type>(value));
  } else {
    result = UnaryOperation<S, T, Fs...>(value);
  }
  return result;
}

template <class T, T(Merge_)(const T&, const T&) = BinaryOperation<T>,
          T(Add_)(const T&, const T&) = BinaryOperation<T>,
          T(Subtract_)(const T&, const T&) = BinaryOperation<T>,
          T(Multiply_)(const T&, const T&) = BinaryOperation<T>,
          T(Divide_)(const T&, const T&) = BinaryOperation<T>,
          T(Modulo_)(const T&, const T&) = BinaryOperation<T>,
          T(And_)(const T&, const T&) = BinaryOperation<T>,
          T(Or_)(const T&, const T&) = BinaryOperation<T>,
          T(Xor_)(const T&, const T&) = BinaryOperation<T>,
          T(Equal_)(const T&, const T&) = BinaryOperation<T>,
          T(NotEqual_)(const T&, const T&) = BinaryOperation<T>,
          T(LeftShift_)(const T&, const T&) = BinaryOperation<T>,
          T(RightShift_)(const T&, const T&) = BinaryOperation<T>,
          T(SignedRightShift_)(const T&, const T&) = BinaryOperation<T>,
          T(ZeroExtend_)(const T&, uint16_t) = ResizeOperation<T>,
          T(SignExtend_)(const T&, uint16_t) = ResizeOperation<T>,
          T(Extract_)(const T&, uint16_t) = ResizeOperation<T>,
          std::string(Print_)(const T&) = UnaryOperation<std::string, T>,
          uint16_t(Size_)(const T&) = UnaryOperation<uint16_t, T>>
struct Operations {
  static constexpr T (*Merge)(const T&, const T&) = Merge_;
  static constexpr T (*Add)(const T&, const T&) = Add_;
  static constexpr T (*Subtract)(const T&, const T&) = Subtract_;
  static constexpr T (*Multiply)(const T&, const T&) = Multiply_;
  static constexpr T (*Divide)(const T&, const T&) = Divide_;
  static constexpr T (*Modulo)(const T&, const T&) = Modulo_;
  static constexpr T (*And)(const T&, const T&) = And_;
  static constexpr T (*Or)(const T&, const T&) = Or_;
  static constexpr T (*Xor)(const T&, const T&) = Xor_;
  static constexpr T (*Equal)(const T&, const T&) = Equal_;
  static constexpr T (*NotEqual)(const T&, const T&) = NotEqual_;
  static constexpr T (*LeftShift)(const T&, const T&) = LeftShift_;
  static constexpr T (*RightShift)(const T&, const T&) = RightShift_;
  static constexpr T (*SignedRightShift)(const T&,
                                         const T&) = SignedRightShift_;
  static constexpr T (*ZeroExtend)(const T&, uint16_t) = ZeroExtend_;
  static constexpr T (*SignExtend)(const T&, uint16_t) = SignExtend_;
  static constexpr T (*Extract)(const T&, uint16_t) = Extract_;
  static constexpr std::string (*Print)(const T&) = Print_;
  static constexpr uint16_t (*Size)(const T&) = Size_;

  template <T(NewMerge)(const T&, const T&)>
  using SetMerge =
      Operations<T, NewMerge, Add_, Subtract_, Multiply_, Divide_, Modulo_,
                 And_, Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewAdd)(const T&, const T&)>
  using SetAdd =
      Operations<T, Merge_, NewAdd, Subtract_, Multiply_, Divide_, Modulo_,
                 And_, Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewSubtract)(const T&, const T&)>
  using SetSubtract =
      Operations<T, Merge_, Add_, NewSubtract, Multiply_, Divide_, Modulo_,
                 And_, Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewMultiply)(const T&, const T&)>
  using SetMultiply =
      Operations<T, Merge_, Add_, Subtract_, NewMultiply, Divide_, Modulo_,
                 And_, Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewDivide)(const T&, const T&)>
  using SetDivide =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, NewDivide, Modulo_,
                 And_, Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewModulo)(const T&, const T&)>
  using SetModulo =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, NewModulo,
                 And_, Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewAnd)(const T&, const T&)>
  using SetAnd =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_,
                 NewAnd, Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewOr)(const T&, const T&)>
  using SetOr =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 NewOr, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewXor)(const T&, const T&)>
  using SetXor =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 Or_, NewXor, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewEqual)(const T&, const T&)>
  using SetEqual =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 Or_, Xor_, NewEqual, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewNotEqual)(const T&, const T&)>
  using SetNotEqual =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 Or_, Xor_, Equal_, NewNotEqual, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewLeftShift)(const T&, const T&)>
  using SetLeftShift =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 Or_, Xor_, Equal_, NotEqual_, NewLeftShift, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewRightShift)(const T&, const T&)>
  using SetRightShift =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 Or_, Xor_, Equal_, NotEqual_, LeftShift_, NewRightShift,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewSignedRightShift)(const T&, const T&)>
  using SetSignedRightShift =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 NewSignedRightShift, ZeroExtend_, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewZeroExtend)(const T&, uint16_t)>
  using SetZeroExtend =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, NewZeroExtend, SignExtend_, Extract_, Print_, Size_>;

  template <T(NewSignExtend)(const T&, uint16_t)>
  using SetSignExtend =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, NewSignExtend, Extract_, Print_, Size_>;

  template <T(NewExtract)(const T&, uint16_t)>
  using SetExtract =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, NewExtract, Print_, Size_>;

  template <std::string(NewPrint)(const T&)>
  using SetPrint =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, NewPrint, Size_>;

  template <uint16_t(NewSize)(const T&)>
  using SetSize =
      Operations<T, Merge_, Add_, Subtract_, Multiply_, Divide_, Modulo_, And_,
                 Or_, Xor_, Equal_, NotEqual_, LeftShift_, RightShift_,
                 SignedRightShift_, ZeroExtend_, SignExtend_, Extract_, Print_, NewSize>;
};
}  // namespace analysis
}  // namespace reil

#define REIL_ANALYSIS_VALUE_OPERATIONS_H_
#endif  // REIL_ANALYSIS_VALUE_OPERATIONS_H_