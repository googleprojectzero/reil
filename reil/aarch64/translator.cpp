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

#include "reil/aarch64/translator.h"

#include <cassert>
#include <cstring>
#include <map>
#include <sstream>

#include "reil/immediate.h"

namespace reil {
namespace aarch64 {

static Register X_[] = {
    Register(64, kX0, "x0"),   Register(64, kX1, "x1"),
    Register(64, kX2, "x2"),   Register(64, kX3, "x3"),
    Register(64, kX4, "x4"),   Register(64, kX5, "x5"),
    Register(64, kX6, "x6"),   Register(64, kX7, "x7"),
    Register(64, kX8, "x8"),   Register(64, kX9, "x9"),
    Register(64, kX10, "x10"), Register(64, kX11, "x11"),
    Register(64, kX12, "x12"), Register(64, kX13, "x13"),
    Register(64, kX14, "x14"), Register(64, kX15, "x15"),
    Register(64, kX16, "x16"), Register(64, kX17, "x17"),
    Register(64, kX18, "x18"), Register(64, kX19, "x19"),
    Register(64, kX20, "x20"), Register(64, kX21, "x21"),
    Register(64, kX22, "x22"), Register(64, kX23, "x23"),
    Register(64, kX24, "x24"), Register(64, kX25, "x25"),
    Register(64, kX26, "x26"), Register(64, kX27, "x27"),
    Register(64, kX28, "x28"), Register(64, kX29, "x29"),
    Register(64, kX30, "x30"),
};

static Register sp_(64, kSp, "sp");
static Register pc_(64, kPc, "pc");

static Register V_[] = {
    Register(128, kV0, "v0"),   Register(128, kV1, "v1"),
    Register(128, kV2, "v2"),   Register(128, kV3, "v3"),
    Register(128, kV4, "v4"),   Register(128, kV5, "v5"),
    Register(128, kV6, "v6"),   Register(128, kV7, "v7"),
    Register(128, kV8, "v8"),   Register(128, kV9, "v9"),
    Register(128, kV10, "v10"), Register(128, kV11, "v11"),
    Register(128, kV12, "v12"), Register(128, kV13, "v13"),
    Register(128, kV14, "v14"), Register(128, kV15, "v15"),
    Register(128, kV16, "v16"), Register(128, kV17, "v17"),
    Register(128, kV18, "v18"), Register(128, kV19, "v19"),
    Register(128, kV20, "v20"), Register(128, kV21, "v21"),
    Register(128, kV22, "v22"), Register(128, kV23, "v23"),
    Register(128, kV24, "v24"), Register(128, kV25, "v25"),
    Register(128, kV26, "v26"), Register(128, kV27, "v27"),
    Register(128, kV28, "v28"), Register(128, kV29, "v29"),
    Register(128, kV30, "v30"), Register(128, kV31, "v31"),
};

static Register n_(8, kN, "n");
static Register z_(8, kZ, "z");
static Register c_(8, kC, "c");
static Register v_(8, kV, "v");

class Translation : public reil::Translation {
 private:
  const decoder::Instruction& di_;

  Operand Register(decoder::Register::Name name);

  Operand GetOperand(const decoder::Operand& operand);
  Operand GetOperand(uint8_t index);
  void SetOperand(const decoder::Operand& operand, const Operand& value);
  void SetOperand(uint8_t index, const Operand& value);

  Operand ApplyExtend(const decoder::Extend& extend, const Operand& value);
  Operand ApplyExtend(uint8_t index, const Operand& value);
  Operand ApplyShift(const decoder::Shift& shift, const Operand& value);
  Operand ApplyShift(uint8_t index, const Operand& value);

  std::tuple<Operand, Operand, Operand, Operand, Operand> AddWithCarry(
      const Operand& lhs, const Operand& rhs, const Operand& carry);
  Operand CountLeadingSignBits(const Operand& value);
  Operand CountLeadingZeroBits(const Operand& value);
  Operand Pack(const std::vector<Operand>& values);
  std::vector<Operand> Unpack(const Operand& value, uint8_t count);

  Operand ConditionHolds();

  std::tuple<Operand, Operand> DecodeBitMasks(uint64_t immN, uint64_t imms,
                                              uint64_t immr, bool immediate);
  std::tuple<uint8_t, bool, bool> DecodeLoadStore(uint8_t opc);

  void TranslatePcRelativeAddressing();
  void TranslateAddSubtractImmediate();
  void TranslateLogicalImmediate();
  void TranslateMoveWideImmediate();
  void TranslateBitfield();
  void TranslateExtract();
  void TranslateConditionalBranch();
  void TranslateExceptionGeneration();
  void TranslateSystem();
  void TranslateBranchRegister();
  void TranslateBranchImmediate();
  void TranslateCompareAndBranch();
  void TranslateTestAndBranch();
  void TranslateLoadStoreExclusive();
  void TranslateLoadLiteral();
  void TranslateLoadStorePair();
  void TranslateLoadStore();
  void TranslateDataProcessingTwoSource();
  void TranslateDataProcessingOneSource();
  void TranslateLogicalShiftedRegister();
  void TranslateAddSubtractShiftedRegister();
  void TranslateAddSubtractExtendedRegister();
  void TranslateAddSubtractWithCarry();
  void TranslateConditionalCompare();
  void TranslateConditionalSelect();
  void TranslateDataProcessingThreeSource();

 public:
  explicit Translation(uint32_t flags, const decoder::Instruction& di);
  virtual ~Translation();

  bool Translate();
};

Translation::Translation(uint32_t flags, const decoder::Instruction& di)
    : reil::Translation(flags), di_(di) {}

Translation::~Translation() {}

Operand Translation::Register(decoder::Register::Name name) {
  if (decoder::Register::kX0 <= name && name <= decoder::Register::kX30) {
    return X_[name - decoder::Register::kX0];
  } else if (name == decoder::Register::kXzr) {
    return Imm64(0);
  } else if (name == decoder::Register::kSp) {
    return sp_;
  } else if (name == decoder::Register::kPc) {
    if (flags_ & kPositionIndependent) {
      return pc_;
    } else {
      return Imm64(di_.address);
    }
  } else {
    valid_ = false;
    return Imm64(0);
  }
}

Operand Translation::GetOperand(const decoder::Operand& operand) {
  Operand value;

  switch (operand.index()) {
    case decoder::kImmediate: {
      decoder::Immediate imm = absl::get<decoder::Immediate>(operand);
      value = Imm(imm.size, imm.value);
    } break;

    case decoder::kRegister: {
      decoder::Register reg = absl::get<decoder::Register>(operand);
      value = Register(reg.name);
      if (reg.size != Size(value)) {
        value = Str(value, Tmp(reg.size));
      }
    } break;

    case decoder::kImmediateOffset: {
      decoder::ImmediateOffset imm_off =
          absl::get<decoder::ImmediateOffset>(operand);
      auto base = GetOperand(imm_off.base);
      auto offset = ApplyShift(imm_off.shift, GetOperand(imm_off.offset));
      auto address = Str(Add(base, offset), Tmp(64));
      if (imm_off.post_index) {
        value = Ldm(base, Tmp(imm_off.size));
      } else {
        value = Ldm(address, Tmp(imm_off.size));
      }

      if (imm_off.writeback) {
        SetOperand(imm_off.base, address);
      }
    } break;

    case decoder::kRegisterOffset: {
      decoder::RegisterOffset reg_off =
          absl::get<decoder::RegisterOffset>(operand);
      auto base = GetOperand(reg_off.base);
      auto offset = ApplyExtend(reg_off.extend, GetOperand(reg_off.offset));
      auto address = Str(Add(base, offset), Tmp(64));
      if (reg_off.post_index) {
        value = Ldm(base, Tmp(reg_off.size));
      } else {
        value = Ldm(address, Tmp(reg_off.size));
      }

      if (reg_off.writeback) {
        SetOperand(reg_off.base, address);
      }
    } break;

    default: { valid_ = false; }
  }

  return value;
}

Operand Translation::GetOperand(uint8_t index) {
  assert(index < di_.operands.size());
  return GetOperand(di_.operands[index]);
}

void Translation::SetOperand(const decoder::Operand& operand,
                             const Operand& value) {
  Operand target;

  switch (operand.index()) {
    case decoder::kRegister: {
      decoder::Register reg = absl::get<decoder::Register>(operand);
      if (reg.name != decoder::Register::kXzr) {
        Str(value, Register(reg.name));
      }
    } break;

    case decoder::kImmediateOffset: {
      decoder::ImmediateOffset imm_off =
          absl::get<decoder::ImmediateOffset>(operand);
      auto base = GetOperand(imm_off.base);
      auto offset = ApplyShift(imm_off.shift, GetOperand(imm_off.offset));
      auto address = Str(Add(base, offset), Tmp(64));
      if (imm_off.post_index) {
        Stm(Str(value, Tmp(imm_off.size)), base);
      } else {
        Stm(Str(value, Tmp(imm_off.size)), address);
      }

      if (imm_off.writeback) {
        SetOperand(imm_off.base, address);
      }
    } break;

    case decoder::kRegisterOffset: {
      decoder::RegisterOffset reg_off =
          absl::get<decoder::RegisterOffset>(operand);
      auto base = GetOperand(reg_off.base);
      auto offset = ApplyExtend(reg_off.extend, GetOperand(reg_off.offset));
      auto address = Str(Add(base, offset), Tmp(64));
      if (reg_off.post_index) {
        Stm(Str(value, Tmp(reg_off.size)), base);
      } else {
        Stm(Str(value, Tmp(reg_off.size)), address);
      }

      if (reg_off.writeback) {
        SetOperand(reg_off.base, address);
      }
    } break;

    default: { valid_ = false; }
  }
}

void Translation::SetOperand(uint8_t index, const Operand& value) {
  assert(index < di_.operands.size());
  return SetOperand(di_.operands[index], value);
}

Operand Translation::ApplyExtend(const decoder::Extend& extend,
                                 const Operand& value) {
  uint8_t result_size = Size(value);
  uint8_t size = 64;

  if (extend.type == decoder::Extend::kSxtb ||
      extend.type == decoder::Extend::kUxtb) {
    size = 8;
  } else if (extend.type == decoder::Extend::kSxth ||
             extend.type == decoder::Extend::kUxth) {
    size = 16;
  } else if (extend.type == decoder::Extend::kSxtw ||
             extend.type == decoder::Extend::kUxtw) {
    size = 32;
  }

  // TODO: check handling of kLsl here.

  auto tmp = Str(value, Tmp(size));
  if (extend.type >= decoder::Extend::kSxtb) {
    tmp = Sex(tmp, Tmp(result_size));
  } else {
    tmp = Str(tmp, Tmp(result_size));
  }

  return Lshl(tmp, Imm8(extend.count));
}

Operand Translation::ApplyExtend(uint8_t index, const Operand& value) {
  return ApplyExtend(absl::get<decoder::Extend>(di_.operands[index]), value);
}

Operand Translation::ApplyShift(const decoder::Shift& shift,
                                const Operand& value) {
  uint8_t size = Size(value);
  Operand result;

  switch (shift.type) {
    case decoder::Shift::kNone: {
      result = value;
    } break;

    case decoder::Shift::kLsl: {
      result = Lshl(value, Imm8(shift.count));
    } break;

    case decoder::Shift::kLsr: {
      result = Lshr(value, Imm8(shift.count));
    } break;

    case decoder::Shift::kAsr: {
      result = Ashr(value, Imm8(shift.count));
    } break;

    case decoder::Shift::kRol: {
      result = Or(Lshl(value, Imm8(shift.count)),
                  Lshr(value, Imm8(size - shift.count)));
    } break;

    case decoder::Shift::kRor: {
      result = Or(Lshr(value, Imm8(shift.count)),
                  Lshl(value, Imm8(size - shift.count)));
    } break;

    default: { valid_ = false; }
  }

  return result;
}

Operand Translation::ApplyShift(uint8_t index, const Operand& value) {
  return ApplyShift(absl::get<decoder::Shift>(di_.operands[index]), value);
}

std::tuple<Operand, Operand, Operand, Operand, Operand>
Translation::AddWithCarry(const Operand& lhs, const Operand& rhs,
                          const Operand& carry) {
  uint16_t size = Size(lhs);

  // calculation
  auto tmp1 = Str(carry, Tmp(size));
  auto tmp2 = Add(rhs, tmp1);
  auto tmp3 = Str(lhs, Tmp(size * 2));
  auto tmp4 = Add(tmp2, tmp3);
  auto tmp5 = Str(tmp4, Tmp(size * 2));
  auto result = Str(tmp4, Tmp(size));

  // sign flag
  auto sign_result = And(result, Immediate::SignBit(size));
  auto n = Bisnz(sign_result);

  // zero flag
  auto z = Bisz(result);

  // carry flag
  auto tmp6 = And(tmp5, Immediate::CarryBit(size));
  auto c = Bisnz(tmp6);

  // overflow flag
  auto sign_lhs = And(lhs, Immediate::SignBit(size));
  auto sign_rhs = And(rhs, Immediate::SignBit(size));
  auto tmp7 = Xor(sign_lhs, sign_result);
  auto tmp8 = Xor(sign_rhs, sign_result);
  auto tmp9 = And(tmp7, tmp8);
  auto v = Bisnz(tmp9);

  return {result, n, z, c, v};
}

Operand Translation::CountLeadingSignBits(const Operand& value) {
  uint16_t size = Size(value);

  auto tmp1 = Lshl(And(Not(Immediate::SignBit(size)), value), Imm8(1));
  auto tmp2 = Or(Xor(tmp1, value), Imm(size, 1));

  return CountLeadingZeroBits(tmp2);
}

Operand Translation::CountLeadingZeroBits(const Operand& value) {
  // we provide different implementations here, since the obvious implementation
  // is very branch-heavy.
  uint16_t size = Size(value);
  Operand result;

  if (flags_ & kMinimalBranches) {
    Operand no_bit = Imm16(1);
    result = Imm16(0);
    for (int16_t i = size - 1; i >= 0; --i) {
      auto tmp1 = And(Lshl(Imm(size, 1), Imm8(i)), value);
      no_bit = And(no_bit, Bisz(tmp1, Tmp(16)));
      result = Str(Add(result, no_bit), Tmp(16));
    }
  } else {
    auto done = Label();
    result = Tmp(16);
    std::vector<Operand> labels;

    for (int16_t i = size - 1; i >= 0; --i) {
      auto tmp1 = And(Lshl(Imm(size, 1), Imm8(i)), value);
      auto label = Label();
      labels.push_back(label);
      Jcc(tmp1, label);
    }

    Str(Imm16(size), result);
    Jcc(Imm8(1), done);

    for (int16_t i = size - 1; i >= 0; --i) {
      Nop(labels[i]);
      Str(Imm16(i), result);
      Jcc(Imm8(1), done);
    }

    Nop(done);
  }

  return result;
}

Operand Translation::Pack(const std::vector<Operand>& values) {
  uint8_t value_size = Size(values[0]);
  uint8_t result_size = value_size * values.size();
  Operand result = Imm(result_size, 0);
  for (size_t i = 0; i < values.size(); ++i) {
    auto tmp1 = Lshl(Str(values[i], Tmp(result_size)), Imm8(value_size * i));
    result = Or(result, tmp1);
  }
  return result;
}

std::vector<Operand> Translation::Unpack(const Operand& value, uint8_t count) {
  std::vector<Operand> results(count);
  uint8_t result_size = Size(value) / count;
  for (uint8_t i = 0; i < count; ++i) {
    results[i] = Str(Lshr(value, Imm8(result_size * i)), Tmp(result_size));
  }
  return results;
}

Operand Translation::ConditionHolds() {
  Operand result;

  switch (di_.cc & 0b1110) {
    case 0b0000: {  // EQ or NE
      result = z_;
    } break;

    case 0b0010: {  // CS or CC
      result = c_;
    } break;

    case 0b0100: {  // MI or PL
      result = n_;
    } break;

    case 0b0110: {  // VS or VC
      result = v_;
    } break;

    case 0b1000: {  // HI or LS
      auto tmp1 = Bisz(z_);
      result = And(c_, tmp1);
    } break;

    case 0b1010: {  // GE or LT
      result = Equ(n_, v_);
    } break;

    case 0b1100: {  // GT or LE
      auto tmp1 = Equ(n_, v_);
      auto tmp2 = Bisz(z_);
      result = And(tmp1, tmp2);
    } break;

    case 0b1110: {  // AL
      result = Imm8(1);
    } break;
  }

  if (di_.cc != 0b1111 && (di_.cc & 0b0001) == 0b0001) {
    result = Bisz(result);
  }

  return result;
}

std::tuple<Operand, Operand> Translation::DecodeBitMasks(uint64_t immN,
                                                         uint64_t imms,
                                                         uint64_t immr,
                                                         bool immediate) {
  // imms + 1 bits of 1, rotated by immr, then replicated to the target size.

  uint64_t element_size = 0;
  uint64_t element_mask = 0;
  if (immN) {
    element_size = 64;
    element_mask = 0xffffffffffffffffull;
  } else {
    element_size = 32;
    while (!(element_size & (~imms))) {
      element_size >>= 1;
    }
    element_mask = (1ull << element_size) - 1;
  }

  uint64_t s = imms & (element_size - 1);
  uint64_t r = immr & (element_size - 1);
  uint64_t d = (s - r) & (element_size - 1);

  // TODO: validation for valid values?

  uint64_t welement = 0xffffffffffffffffull >> (64 - (s + 1));
  uint64_t welement_left = (welement << (element_size - r)) & element_mask;
  uint64_t welement_right = welement >> r;
  welement = welement_left | welement_right;

  uint64_t telement = 0xffffffffffffffffull >> (64 - (d + 1));

  // always replicate to 64-bits and truncate later.
  uint64_t wmask = welement;
  uint64_t tmask = telement;
  while (element_size < 64) {
    wmask |= (wmask << element_size);
    tmask |= (tmask << element_size);
    element_size <<= 1;
  }

  return {Imm64(wmask), Imm64(tmask)};
}

void Translation::TranslatePcRelativeAddressing() {
  auto pc = Register(decoder::Register::kPc);
  if (di_.opcode == decoder::kAdrp) {
    pc = And(pc, Imm64(0xfffffffffffff000ull));
  }
  auto tmp1 = Str(Add(pc, ApplyShift(2, GetOperand(1))), Tmp(64));
  SetOperand(0, tmp1);
}

void Translation::TranslateAddSubtractImmediate() {
  auto lhs = GetOperand(1);
  auto rhs = ApplyShift(3, GetOperand(2));
  Operand result, n, z, c, v;

  if (di_.opcode == decoder::kAddImmediate) {
    std::tie(result, n, z, c, v) = AddWithCarry(lhs, rhs, Imm8(0));
  } else if (di_.opcode == decoder::kSubImmediate) {
    std::tie(result, n, z, c, v) = AddWithCarry(lhs, Not(rhs), Imm8(1));
  } else {
    valid_ = false;
    return;
  }

  if (di_.set_flags) {
    Str(n, n_);
    Str(z, z_);
    Str(c, c_);
    Str(v, v_);
  }

  SetOperand(0, result);
}

void Translation::TranslateLogicalImmediate() {
  auto lhs = GetOperand(1);
  auto rhs = GetOperand(2);
  Operand result;

  switch (di_.opcode) {
    case decoder::kAndImmediate: {
      result = And(lhs, rhs);
      if (di_.set_flags) {
        auto sign_result = And(result, Immediate::SignBit(Size(lhs)));
        Bisnz(sign_result, n_);
        Bisz(result, z_);
        Str(Imm8(0), c_);
        Str(Imm8(0), v_);
      }
    } break;

    case decoder::kOrrImmediate: {
      result = Or(lhs, rhs);
    } break;

    case decoder::kEorImmediate: {
      result = Xor(lhs, rhs);
    } break;

    default: {
      valid_ = false;
      return;
    }
  }

  SetOperand(0, result);
}

void Translation::TranslateMoveWideImmediate() {
  auto result = ApplyShift(2, GetOperand(1));

  switch (di_.opcode) {
    case decoder::kMovn: {
      result = Not(result);
    } break;

    case decoder::kMovz: {
    } break;

    case decoder::kMovk: {
      auto tmp1 = GetOperand(0);
      auto tmp2 = Not(ApplyShift(2, Imm(Size(tmp1), 0xffffull)));
      result = Or(And(tmp1, tmp2), result);
    } break;

    default: {
      valid_ = false;
      return;
    }
  }

  SetOperand(0, result);
}

void Translation::TranslateBitfield() {
  uint64_t wmask, tmask;

  Operand src = GetOperand(1);
  Operand dst = GetOperand(0);
  uint8_t size = Size(src);

  auto immr = absl::get<decoder::Immediate>(di_.operands[2]);
  auto imms = absl::get<decoder::Immediate>(di_.operands[3]);

  std::tie(wmask, tmask) = decoder::DecodeBitMasks(size, imms, immr);

  Operand result;
  switch (di_.opcode) {
    case decoder::kSbfm: {
      auto tmp1 = Lshl(src, Imm8(size - immr.value));
      auto tmp2 = Lshr(src, Imm8(immr.value));
      auto tmp3 = Or(tmp1, tmp2);
      auto tmp4 = And(Imm(size, tmask), Imm(size, wmask));
      auto tmp5 = And(tmp3, tmp4);
      auto tmp6 = And(src, Imm(size, 1ull << imms.value));
      auto tmp7 = Bisnz(tmp6, Tmp(size));
      auto tmp8 = Str(Mul(Immediate::Mask(size), tmp7), Tmp(size));
      auto tmp9 = And(tmp8, Not(Imm(size, tmask)));
      result = Or(tmp5, tmp9);
    } break;

    case decoder::kBfm: {
      // TODO: I think with some simple equivalences we can make this more
      // optimisable, so that constant folding can eliminate half of the tmask
      // and wmask operations.
      auto tmp1 = Lshl(src, Imm8(size - immr.value));
      auto tmp2 = Lshr(src, Imm8(immr.value));
      auto tmp3 = Or(tmp1, tmp2);
      auto tmp4 = And(tmp3, Imm(size, wmask));
      auto tmp5 = And(dst, Not(Imm(size, wmask)));
      auto tmp6 = Or(tmp4, tmp5);
      auto tmp7 = And(tmp6, Imm(size, tmask));
      auto tmp8 = And(dst, Not(Imm(size, tmask)));
      result = Or(tmp7, tmp8);
    } break;

    case decoder::kUbfm: {
      auto tmp1 = Lshl(src, Imm8(size - immr.value));
      auto tmp2 = Lshr(src, Imm8(immr.value));
      auto tmp3 = Or(tmp1, tmp2);
      auto tmp4 = And(Imm(size, tmask), Imm(size, wmask));
      result = And(tmp3, tmp4);
    } break;

    default: {
      valid_ = false;
      return;
    }
  }

  SetOperand(0, result);
}

void Translation::TranslateExtract() {
  uint8_t size = Size(GetOperand(0));

  auto tmp1 = Str(GetOperand(1), Tmp(size * 2));
  auto tmp2 = Str(GetOperand(2), Tmp(size * 2));
  auto tmp3 = Lshl(tmp1, Imm8(size));
  auto tmp4 = Or(tmp2, tmp3);
  auto tmp5 = Lshr(tmp4, GetOperand(3));
  auto result = Str(tmp5, Tmp(size));

  SetOperand(0, result);
}

void Translation::TranslateConditionalBranch() {
  auto pc = Register(decoder::Register::kPc);
  Jump(ConditionHolds(), Str(Add(pc, GetOperand(0)), Tmp(64)));
}

void Translation::TranslateExceptionGeneration() {
  switch (di_.opcode) {
    case decoder::kSvc: {
      Sys(GetOperand(0));
    } break;

    case decoder::kHvc:
    case decoder::kSmc:
    case decoder::kBrk:
    case decoder::kHlt:
    case decoder::kDcps1:
    case decoder::kDcps2:
    case decoder::kDcps3: {
      Unkn();
    } break;

    default: {
      valid_ = false;
      return;
    }
  }
}

void Translation::TranslateSystem() {
  switch (di_.opcode) {
    case decoder::kNop:
    case decoder::kYield:
    case decoder::kWfe:
    case decoder::kWfi:
    case decoder::kSev:
    case decoder::kSevl:
    case decoder::kXpaclri:
    case decoder::kPacia1716:
    case decoder::kPacib1716:
    case decoder::kAutia1716:
    case decoder::kAutib1716:
    case decoder::kEsb:
    case decoder::kPsbCsync:
    case decoder::kPaciaz:
    case decoder::kPaciasp:
    case decoder::kPacibz:
    case decoder::kPacibsp:
    case decoder::kAutiaz:
    case decoder::kAutiasp:
    case decoder::kAutibz:
    case decoder::kAutibsp:
    case decoder::kHint:
    case decoder::kClrex:
    case decoder::kDsb:
    case decoder::kDmb:
    case decoder::kIsb: {
      Nop();
    } break;

    case decoder::kSys:
    case decoder::kMsr:
    case decoder::kSysl:
    case decoder::kMrs: {
      Unkn();
    } break;

    default: {
      valid_ = false;
      return;
    }
  }
}

void Translation::TranslateBranchRegister() {
  switch (di_.opcode) {
    case decoder::kBr:
    case decoder::kBraa:
    case decoder::kBrab:
    case decoder::kBraaz:
    case decoder::kBrabz: {
      Jump(GetOperand(0));
    } break;

    case decoder::kBlr:
    case decoder::kBlraa:
    case decoder::kBlrab:
    case decoder::kBlraaz:
    case decoder::kBlrabz: {
      auto pc = Register(decoder::Register::kPc);
      Str(Add(pc, Imm64(4)), Register(decoder::Register::kX30));
      Call(GetOperand(0));
    } break;

    case decoder::kRet:
    case decoder::kRetaa:
    case decoder::kRetab: {
      Return(GetOperand(0));
    } break;

    case decoder::kEret:
    case decoder::kEretaa:
    case decoder::kEretab:
    case decoder::kDrps: {
      Unkn();
    } break;

    default: {
      valid_ = false;
      return;
    }
  }
}

void Translation::TranslateBranchImmediate() {
  auto pc = Register(decoder::Register::kPc);
  switch (di_.opcode) {
    case decoder::kB: {
      Jump(Str(Add(pc, GetOperand(0)), Tmp(64)));
    } break;

    case decoder::kBl: {
      Str(Add(pc, Imm64(4)), Register(decoder::Register::kX30));
      Call(Str(Add(pc, GetOperand(0)), Tmp(64)));
    } break;

    default: {
      valid_ = false;
      return;
    }
  }
}

void Translation::TranslateCompareAndBranch() {
  auto pc = Register(decoder::Register::kPc);
  Operand cond;

  if (di_.opcode == decoder::kCbz) {
    cond = Bisz(GetOperand(0));
  } else if (di_.opcode == decoder::kCbnz) {
    cond = GetOperand(0);
  }

  Jump(cond, Str(Add(pc, Str(GetOperand(1), Tmp(64))), Tmp(64)));
}

void Translation::TranslateTestAndBranch() {
  auto pc = Register(decoder::Register::kPc);
  auto tmp1 = Str(GetOperand(0), Tmp(64));
  auto tmp2 = And(tmp1, Lshl(Imm64(1), GetOperand(1)));
  Operand cond;

  if (di_.opcode == decoder::kTbz) {
    cond = Bisz(tmp2);
  } else if (di_.opcode == decoder::kTbnz) {
    cond = tmp2;
  } else {
    valid_ = false;
    return;
  }

  Jump(cond, Str(Add(pc, Str(GetOperand(2), Tmp(64))), Tmp(64)));
}

void Translation::TranslateLoadStoreExclusive() {
  switch (di_.opcode) {
    case decoder::kCas:
    case decoder::kCasa:
    case decoder::kCasal:
    case decoder::kCasl: {
      auto tmp1 = GetOperand(2);
      auto tmp2 = Equ(GetOperand(0), tmp1);
      if (flags_ & kMinimalBranches) {
        Operand tmp3 = Ite(tmp2, GetOperand(1), tmp1);
        SetOperand(2, tmp3);
      } else {
        auto done = Label();
        auto tmp3 = Bisz(tmp2);
        Jcc(tmp3, done);
        SetOperand(2, GetOperand(1));
        Nop(done);
      }
    } break;

    case decoder::kCasp:
    case decoder::kCaspa:
    case decoder::kCaspal:
    case decoder::kCaspl: {
      // TODO: implement
      valid_ = false;
      return;
    } break;

    case decoder::kLdxr:
    case decoder::kLdaxr:
    case decoder::kLdlar:
    case decoder::kLdar: {
      SetOperand(0, GetOperand(1));
    } break;

    case decoder::kLdxp:
    case decoder::kLdaxp: {
      auto values = Unpack(GetOperand(2), 2);
      SetOperand(0, values[0]);
      SetOperand(1, values[1]);
    } break;

    case decoder::kStllr:
    case decoder::kStlr: {
      SetOperand(1, GetOperand(0));
    } break;

    case decoder::kStxr:
    case decoder::kStlxr: {
      SetOperand(0, Imm64(0));
      SetOperand(2, GetOperand(1));
    } break;

    case decoder::kStxp:
    case decoder::kStlxp: {
      SetOperand(0, Imm64(0));
      SetOperand(3, Pack({GetOperand(1), GetOperand(2)}));
    } break;

    default: {
      valid_ = false;
      return;
    }
  }
}

void Translation::TranslateLoadLiteral() {
  switch (di_.opcode) {
    case decoder::kLdrLiteral: {
      SetOperand(0, GetOperand(1));
    } break;

    case decoder::kLdrsLiteral: {
      SetOperand(0, Sex(GetOperand(1), Tmp(64)));
    } break;

    case decoder::kPrfmLiteral: {
      Nop();
    } break;

    default: {
      valid_ = false;
      return;
    }
  }
}

void Translation::TranslateLoadStorePair() {
  switch (di_.opcode) {
    case decoder::kLdp:
    case decoder::kLdnp: {
      auto values = Unpack(GetOperand(2), 2);
      SetOperand(0, values[0]);
      SetOperand(1, values[1]);
    } break;

    case decoder::kLdpsw: {
      auto values = Unpack(GetOperand(2), 2);
      SetOperand(0, Sex(values[0], Tmp(64)));
      SetOperand(1, Sex(values[1], Tmp(64)));
    } break;

    case decoder::kStp:
    case decoder::kStnp: {
      SetOperand(2, Pack({GetOperand(0), GetOperand(1)}));
    } break;

    default: {
      valid_ = false;
      return;
    }
  }
}

void Translation::TranslateLoadStore() {
  switch (di_.opcode) {
    case decoder::kLdr:
    case decoder::kLdur:
    case decoder::kLdtr: {
      SetOperand(0, GetOperand(1));
    } break;

    case decoder::kLdrs:
    case decoder::kLdurs:
    case decoder::kLdtrs: {
      SetOperand(0, Sex(GetOperand(1), Tmp(Size(GetOperand(0)))));
    } break;

    case decoder::kStr:
    case decoder::kStur:
    case decoder::kSttr: {
      SetOperand(1, GetOperand(0));
    } break;

    case decoder::kPrfm: {
      Nop();
    } break;

    default: {
      valid_ = false;
      return;
    }
  }
}

void Translation::TranslateDataProcessingTwoSource() {
  auto lhs = GetOperand(1);
  auto rhs = GetOperand(2);
  Operand result;

  switch (di_.opcode) {
    case decoder::kUdiv: {
      result = Div(lhs, rhs);
    } break;

    case decoder::kSdiv: {
      result = Sdiv(lhs, rhs);
    } break;

    case decoder::kLsl: {
      result = Lshl(lhs, Mod(rhs, Imm(Size(rhs), Size(lhs))));
    } break;

    case decoder::kLsr: {
      result = Lshr(lhs, Mod(rhs, Imm(Size(rhs), Size(lhs))));
    } break;

    case decoder::kAsr: {
      result = Ashr(lhs, Mod(rhs, Imm(Size(rhs), Size(lhs))));
    } break;

    case decoder::kRor: {
      auto shift = Mod(rhs, Imm(Size(rhs), Size(lhs)));
      auto tmp1 = Lshr(lhs, shift);
      auto tmp2 =
          Lshl(lhs, Str(Sub(Imm(Size(rhs), Size(lhs)), shift), Tmp(Size(lhs))));
      result = Or(tmp1, tmp2);
    } break;

    case decoder::kPacga:
    case decoder::kCrc32b:
    case decoder::kCrc32h:
    case decoder::kCrc32w:
    case decoder::kCrc32x:
    case decoder::kCrc32cb:
    case decoder::kCrc32ch:
    case decoder::kCrc32cw:
    case decoder::kCrc32cx: {
      Unkn();
      return;
    }

    default: {
      valid_ = false;
      return;
    }
  }

  SetOperand(0, result);
}

void Translation::TranslateDataProcessingOneSource() {
  auto value = GetOperand(1);
  uint8_t size = Size(value);
  Operand result;

  switch (di_.opcode) {
    case decoder::kRbit: {
      result = Imm(size, 0);
      for (uint8_t i = 0; i < size; ++i) {
        auto tmp1 = Lshl(Imm(size, 1), Imm8(i));
        auto tmp2 = Bisnz(And(value, tmp1), Tmp(size));
        auto tmp3 = Lshl(tmp2, Imm8(size - 1 - i));
        result = Or(result, tmp3);
      }
    } break;

    case decoder::kRev16: {
      auto tmp1 = Str(value, Tmp(64));
      auto tmp2 = Lshr(And(Imm64(0xff00ff00ff00ff00ull), tmp1), Imm8(8));
      auto tmp3 = Lshl(And(Imm64(0x00ff00ff00ff00ffull), tmp1), Imm8(8));
      result = Str(Or(tmp2, tmp3), Tmp(size));
    } break;

    case decoder::kRev32: {
      auto tmp1 = Str(value, Tmp(64));
      auto tmp2 = Lshr(And(Imm64(0xff000000ff000000ull), tmp1), Imm8(24));
      auto tmp3 = Lshr(And(Imm64(0x00ff000000ff0000ull), tmp1), Imm8(8));
      auto tmp4 = Lshl(And(Imm64(0x0000ff000000ff00ull), tmp1), Imm8(8));
      auto tmp5 = Lshl(And(Imm64(0x000000ff000000ffull), tmp1), Imm8(24));
      result = Str(Or(Or(tmp2, tmp3), Or(tmp4, tmp5)), Tmp(size));
    } break;

    case decoder::kRev: {
      result = Imm(size, 0);
      for (uint8_t i = 0; i < size / 8; ++i) {
        auto tmp1 = And(Lshr(value, Imm8(size - 8 - (i * 8))), Imm(size, 0xff));
        result = Or(result, Lshl(tmp1, Imm8(i * 8)));
      }
    } break;

    case decoder::kClz: {
      result = Str(CountLeadingZeroBits(value), Tmp(size));
    } break;

    case decoder::kCls: {
      result = Str(CountLeadingSignBits(value), Tmp(size));
    } break;

    case decoder::kAutia:
    case decoder::kAutib:
    case decoder::kAutda:
    case decoder::kAutdb:
    case decoder::kPacia:
    case decoder::kPacib:
    case decoder::kPacda:
    case decoder::kPacdb:
    case decoder::kXpaci:
    case decoder::kXpacd: {
      Unkn();
      return;
    }

    default: {
      valid_ = false;
      return;
    }
  }

  SetOperand(0, result);
}

void Translation::TranslateLogicalShiftedRegister() {
  auto lhs = GetOperand(1);
  auto rhs = ApplyShift(3, GetOperand(2));
  Operand result;

  switch (di_.opcode) {
    case decoder::kAndShiftedRegister: {
      result = And(lhs, rhs);
    } break;

    case decoder::kBicShiftedRegister: {
      result = And(lhs, Not(rhs));
    } break;

    case decoder::kOrrShiftedRegister: {
      result = Or(lhs, rhs);
    } break;

    case decoder::kOrnShiftedRegister: {
      result = Or(lhs, Not(rhs));
    } break;

    case decoder::kEorShiftedRegister: {
      result = Xor(lhs, rhs);
    } break;

    case decoder::kEonShiftedRegister: {
      result = Xor(lhs, Not(rhs));
    } break;

    default: {
      valid_ = false;
      return;
    }
  }

  if (di_.set_flags) {
    auto sign_result = And(result, Immediate::SignBit(Size(result)));
    Bisnz(sign_result, n_);
    Bisz(result, z_);
    Str(Imm8(0), c_);
    Str(Imm8(0), v_);
  }

  SetOperand(0, result);
}

void Translation::TranslateAddSubtractShiftedRegister() {
  auto lhs = GetOperand(1);
  auto rhs = ApplyShift(3, GetOperand(2));
  Operand result, n, z, c, v;

  if (di_.opcode == decoder::kAddShiftedRegister) {
    std::tie(result, n, z, c, v) = AddWithCarry(lhs, rhs, Imm8(0));
  } else if (di_.opcode == decoder::kSubShiftedRegister) {
    std::tie(result, n, z, c, v) = AddWithCarry(lhs, Not(rhs), Imm8(1));
  } else {
    valid_ = false;
    return;
  }

  if (di_.set_flags) {
    Str(n, n_);
    Str(z, z_);
    Str(c, c_);
    Str(v, v_);
  }

  SetOperand(0, result);
}

void Translation::TranslateAddSubtractExtendedRegister() {
  auto lhs = GetOperand(1);
  // NB: ApplyExtend extends to the size of the second argument, and due to the
  // way that this instruction can be a 64-bit operation with a 32-bit argument
  // register, we need to do the extra Str to make sure we extend to the
  // correct size.
  auto rhs = ApplyExtend(3, Str(GetOperand(2), Tmp(Size(lhs))));
  Operand result, n, z, c, v;

  if (di_.opcode == decoder::kAddExtendedRegister) {
    std::tie(result, n, z, c, v) = AddWithCarry(lhs, rhs, Imm8(0));
  } else if (di_.opcode == decoder::kSubExtendedRegister) {
    std::tie(result, n, z, c, v) = AddWithCarry(lhs, Not(rhs), Imm8(1));
  } else {
    valid_ = false;
    return;
  }

  if (di_.set_flags) {
    Str(n, n_);
    Str(z, z_);
    Str(c, c_);
    Str(v, v_);
  }

  SetOperand(0, result);
}

void Translation::TranslateAddSubtractWithCarry() {
  auto lhs = GetOperand(1);
  auto rhs = GetOperand(2);
  Operand result, n, z, c, v;

  if (di_.opcode == decoder::kAdc) {
    std::tie(result, n, z, c, v) = AddWithCarry(lhs, rhs, c_);
  } else if (di_.opcode == decoder::kSbc) {
    std::tie(result, n, z, c, v) = AddWithCarry(lhs, Not(rhs), c_);
  } else {
    valid_ = false;
    return;
  }

  if (di_.set_flags) {
    Str(n, n_);
    Str(z, z_);
    Str(c, c_);
    Str(v, v_);
  }

  SetOperand(0, result);
}

void Translation::TranslateConditionalCompare() {
  auto lhs = GetOperand(0);
  auto rhs = GetOperand(1);
  uint64_t nzcv = absl::get<decoder::Immediate>(di_.operands[2]).value;
  Operand result, _, n, z, c, v;

  // separate implementation for kMinimalBranches, since we can use a branch to
  // optimise the computation necessary if we don't consider branches to be
  // expensive

  if (flags_ & kMinimalBranches) {
    if (di_.opcode == decoder::kCcmn) {
      std::tie(_, n, z, c, v) = AddWithCarry(lhs, Not(rhs), Imm8(1));
    } else if (di_.opcode == decoder::kCcmp) {
      std::tie(_, n, z, c, v) = AddWithCarry(lhs, rhs, Imm8(0));
    } else {
      valid_ = false;
      return;
    }

    // NB: this looks weird, but otherwise condition may be one of the registers
    // we are about to update, and would be invalidated during the block of ite
    // instructions.
    auto condition = Str(ConditionHolds(), Tmp(8));

    Ite(condition, n, Imm8((nzcv >> 3) & 0b1), n_);
    Ite(condition, z, Imm8((nzcv >> 2) & 0b1), z_);
    Ite(condition, c, Imm8((nzcv >> 1) & 0b1), c_);
    Ite(condition, v, Imm8((nzcv >> 0) & 0b1), v_);
  } else {
    auto if_condition_holds = Label();
    auto end = Label();

    Jcc(ConditionHolds(), if_condition_holds);

    Str(Imm8((nzcv >> 3) & 0b1), n_);
    Str(Imm8((nzcv >> 2) & 0b1), z_);
    Str(Imm8((nzcv >> 1) & 0b1), c_);
    Str(Imm8((nzcv >> 0) & 0b1), v_);
    Jcc(Imm8(1), end);

    Nop(if_condition_holds);
    if (di_.opcode == decoder::kCcmn) {
      std::tie(_, n, z, c, v) = AddWithCarry(lhs, Not(rhs), Imm8(1));
    } else if (di_.opcode == decoder::kCcmp) {
      std::tie(_, n, z, c, v) = AddWithCarry(lhs, rhs, Imm8(0));
    } else {
      valid_ = false;
      return;
    }

    Str(n, n_);
    Str(z, z_);
    Str(c, c_);
    Str(v, v_);

    Nop(end);
  }
}

void Translation::TranslateConditionalSelect() {
  auto lhs = GetOperand(1);
  auto rhs = GetOperand(2);

  switch (di_.opcode) {
    case decoder::kCsel: {
      // Nothing to do here
    } break;

    case decoder::kCsinc: {
      rhs = Str(Add(rhs, Imm(Size(rhs), 1)), Tmp(Size(rhs)));
    } break;

    case decoder::kCsinv: {
      rhs = Not(rhs);
    } break;

    case decoder::kCsneg: {
      rhs = Str(Add(Not(rhs), Imm(Size(rhs), 1)), Tmp(Size(rhs)));
    } break;

    default: {
      valid_ = false;
      return;
    }
  }

  SetOperand(0, Ite(ConditionHolds(), lhs, rhs));
}

void Translation::TranslateDataProcessingThreeSource() {
  auto a = GetOperand(1);
  auto b = GetOperand(2);
  auto c = GetOperand(3);
  uint8_t size = Size(a);
  Operand result;

  switch (di_.opcode) {
    case decoder::kMadd: {
      result = Str(Add(Str(c, Tmp(size * 2)), Mul(a, b)), Tmp(size));
    } break;

    case decoder::kMsub: {
      result = Str(Sub(Str(c, Tmp(size * 2)), Mul(a, b)), Tmp(size));
    } break;

    case decoder::kSmaddl: {
      result = Str(Add(c, Smul(a, b)), Tmp(64));
    } break;

    case decoder::kSmsubl: {
      result = Str(Sub(c, Smul(a, b)), Tmp(64));
    } break;

    case decoder::kSmulh: {
      result = And(Lshr(Smul(a, b), Imm8(64)), Immediate::Mask(128, 64));
    } break;

    case decoder::kUmaddl: {
      result = Str(Add(c, Mul(a, b)), Tmp(64));
    } break;

    case decoder::kUmsubl: {
      result = Str(Sub(c, Mul(a, b)), Tmp(64));
    } break;

    case decoder::kUmulh: {
      result = And(Lshr(Mul(a, b), Imm8(64)), Immediate::Mask(128, 64));
    } break;

    default: {
      valid_ = false;
      return;
    }
  }

  SetOperand(0, result);
}

bool Translation::Translate() {
  valid_ = true;

  if (di_.opcode <= decoder::kAdrp) {
    TranslatePcRelativeAddressing();
  } else if (di_.opcode <= decoder::kSubImmediate) {
    TranslateAddSubtractImmediate();
  } else if (di_.opcode <= decoder::kEorImmediate) {
    TranslateLogicalImmediate();
  } else if (di_.opcode <= decoder::kMovz) {
    TranslateMoveWideImmediate();
  } else if (di_.opcode <= decoder::kUbfm) {
    TranslateBitfield();
  } else if (di_.opcode <= decoder::kExtr) {
    TranslateExtract();
  } else if (di_.opcode <= decoder::kBCond) {
    TranslateConditionalBranch();
  } else if (di_.opcode <= decoder::kSvc) {
    TranslateExceptionGeneration();
  } else if (di_.opcode <= decoder::kYield) {
    TranslateSystem();
  } else if (di_.opcode <= decoder::kRetabz) {
    TranslateBranchRegister();
  } else if (di_.opcode <= decoder::kBl) {
    TranslateBranchImmediate();
  } else if (di_.opcode <= decoder::kCbz) {
    TranslateCompareAndBranch();
  } else if (di_.opcode <= decoder::kTbz) {
    TranslateTestAndBranch();
  } else if (di_.opcode <= decoder::kStxr) {
    TranslateLoadStoreExclusive();
  } else if (di_.opcode <= decoder::kPrfmLiteral) {
    TranslateLoadLiteral();
  } else if (di_.opcode <= decoder::kStp) {
    TranslateLoadStorePair();
  } else if (di_.opcode <= decoder::kStur) {
    TranslateLoadStore();
  } else if (di_.opcode <= decoder::kUdiv) {
    TranslateDataProcessingTwoSource();
  } else if (di_.opcode <= decoder::kXpaci) {
    TranslateDataProcessingOneSource();
  } else if (di_.opcode <= decoder::kEonShiftedRegister) {
    TranslateLogicalShiftedRegister();
  } else if (di_.opcode <= decoder::kSubShiftedRegister) {
    TranslateAddSubtractShiftedRegister();
  } else if (di_.opcode <= decoder::kSubExtendedRegister) {
    TranslateAddSubtractExtendedRegister();
  } else if (di_.opcode <= decoder::kSbc) {
    TranslateAddSubtractWithCarry();
  } else if (di_.opcode <= decoder::kCcmp) {
    TranslateConditionalCompare();
  } else if (di_.opcode <= decoder::kCsneg) {
    TranslateConditionalSelect();
  } else if (di_.opcode <= decoder::kUmsubl) {
    TranslateDataProcessingThreeSource();
  } else {
    valid_ = false;
  }

  return valid();
}

std::string RegisterName(uint8_t index) {
  static std::map<uint8_t, std::string> register_names_({
    {kX0, "x0"},
    {kX1, "x1"},
    {kX2, "x2"},
    {kX3, "x3"},
    {kX4, "x4"},
    {kX5, "x5"},
    {kX6, "x6"},
    {kX7, "x7"},
    {kX8, "x8"},
    {kX9, "x9"},
    {kX10, "x10"},
    {kX11, "x11"},
    {kX12, "x12"},
    {kX13, "x13"},
    {kX14, "x14"},
    {kX15, "x15"},
    {kX16, "x16"},
    {kX17, "x17"},
    {kX18, "x18"},
    {kX19, "x19"},
    {kX20, "x20"},
    {kX21, "x21"},
    {kX22, "x22"},
    {kX23, "x23"},
    {kX24, "x24"},
    {kX25, "x25"},
    {kX26, "x26"},
    {kX27, "x27"},
    {kX28, "x28"},
    {kX29, "x29"},
    {kX30, "x30"},
    {kXzr, "xzr"},

    {kSp, "sp"},
    {kPc, "pc"},

    {kN, "n"},
    {kZ, "z"},
    {kC, "c"},
    {kV, "v"},

    {kV0, "v0"},
    {kV1, "v1"},
    {kV2, "v2"},
    {kV3, "v3"},
    {kV4, "v4"},
    {kV5, "v5"},
    {kV6, "v6"},
    {kV7, "v7"},
    {kV8, "v8"},
    {kV9, "v9"},
    {kV10, "v10"},
    {kV11, "v11"},
    {kV12, "v12"},
    {kV13, "v13"},
    {kV14, "v14"},
    {kV15, "v15"},
    {kV16, "v16"},
    {kV17, "v17"},
    {kV18, "v18"},
    {kV19, "v19"},
    {kV20, "v20"},
    {kV21, "v21"},
    {kV22, "v22"},
    {kV23, "v23"},
    {kV24, "v24"},
    {kV25, "v25"},
    {kV26, "v26"},
    {kV27, "v27"},
    {kV28, "v28"},
    {kV29, "v29"},
    {kV30, "v30"},
    {kV31, "v31"},
  });

  return register_names_.at(index);
}

uint8_t RegisterIndex(std::string name) {
  static std::map<std::string, uint8_t> register_names_({
    {"x0", kX0},
    {"x1", kX1},
    {"x2", kX2},
    {"x3", kX3},
    {"x4", kX4},
    {"x5", kX5},
    {"x6", kX6},
    {"x7", kX7},
    {"x8", kX8},
    {"x9", kX9},
    {"x10", kX10},
    {"x11", kX11},
    {"x12", kX12},
    {"x13", kX13},
    {"x14", kX14},
    {"x15", kX15},
    {"x16", kX16},
    {"x17", kX17},
    {"x18", kX18},
    {"x19", kX19},
    {"x20", kX20},
    {"x21", kX21},
    {"x22", kX22},
    {"x23", kX23},
    {"x24", kX24},
    {"x25", kX25},
    {"x26", kX26},
    {"x27", kX27},
    {"x28", kX28},
    {"x29", kX29},
    {"x30", kX30},
    {"xzr", kXzr},

    {"sp", kSp},
    {"pc", kPc},

    {"n", kN},
    {"z", kZ},
    {"c", kC},
    {"v", kV},

    {"v0", kV0},
    {"v1", kV1},
    {"v2", kV2},
    {"v3", kV3},
    {"v4", kV4},
    {"v5", kV5},
    {"v6", kV6},
    {"v7", kV7},
    {"v8", kV8},
    {"v9", kV9},
    {"v10", kV10},
    {"v11", kV11},
    {"v12", kV12},
    {"v13", kV13},
    {"v14", kV14},
    {"v15", kV15},
    {"v16", kV16},
    {"v17", kV17},
    {"v18", kV18},
    {"v19", kV19},
    {"v20", kV20},
    {"v21", kV21},
    {"v22", kV22},
    {"v23", kV23},
    {"v24", kV24},
    {"v25", kV25},
    {"v26", kV26},
    {"v27", kV27},
    {"v28", kV28},
    {"v29", kV29},
    {"v30", kV30},
    {"v31", kV31},
  });

  assert(register_names_.find(name) != register_names_.end());

  return register_names_.at(name);
}

NativeInstruction TranslateInstruction(const decoder::Instruction& di, uint32_t flags) {
  NativeInstruction ni;

  ni.address = di.address;
  ni.size = 4;

  // generate mnemonic if necessary
  if (!(flags & kNoMnemonics)) {
    std::ostringstream mnemonic;
    mnemonic << di;
    ni.mnemonic = mnemonic.str();
  }

  // perform translation
  Translation translation(flags, di);
  if (translation.Translate()) {
    ni.reil = translation.Finalise();
  } else {
    ni.reil.push_back(Unkn());
  }

  return ni;
}

NativeInstruction TranslateInstruction(uint64_t address, std::vector<uint8_t> bytes,
                            uint32_t flags) {
  return TranslateInstruction(address, bytes.data(), bytes.size(), flags);
}

NativeInstruction TranslateInstruction(uint64_t address, const uint8_t* bytes,
                            size_t bytes_len, uint32_t flags) {
  // read opcode
  uint32_t opcode = 0;
  if (bytes_len >= sizeof(opcode)) {
    memcpy(&opcode, bytes, sizeof(opcode));
  }

  // decode instruction
  auto di = decoder::DecodeInstruction(address, opcode);

  return TranslateInstruction(di, flags);
}
}  // namespace aarch64
}  // namespace reil
