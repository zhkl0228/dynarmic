/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * SPDX-License-Identifier: 0BSD
 */

#pragma once

#include "frontend/imm.h"
#include "frontend/A32/location_descriptor.h"
#include "frontend/A32/translate/translate.h"
#include "frontend/A32/translate/helper.h"

namespace Dynarmic::A32 {

enum class Exception;

struct ThumbTranslatorVisitor final : public A32TranslatorVisitor {
    using instruction_return_type = bool;

    explicit ThumbTranslatorVisitor(IR::Block& block, LocationDescriptor descriptor, const TranslationOptions& options) : A32TranslatorVisitor(block, descriptor, options), is_thumb_16(false) {
        ASSERT_MSG(descriptor.TFlag(), "The processor must be in Thumb mode");
    }
    
    static u32 ThumbExpandImm(Imm<1> i, Imm<3> imm3, Imm<8> imm8) {
        const auto imm12 = concatenate(i, imm3, imm8);
        if (imm12.Bits<10, 11>() == 0) {
            const u32 bytes = imm12.Bits<0, 7>();
            switch (imm12.Bits<8, 9>()) {
                case 0b00:
                    return bytes;
                case 0b01:
                    return (bytes << 16U) | bytes;
                case 0b10:
                    return (bytes << 24U) | (bytes << 8U);
                case 0b11:
                    return Common::Replicate(bytes, 8);
            }
            assert(false);
        }
        const int rotate = imm12.Bits<7, 11>();
        const Imm<8> unrotated_value = concatenate(Imm<1>(1), Imm<7>(imm12.Bits<0, 6>()));
        return Common::RotateRight<u32>(unrotated_value.ZeroExtend(), rotate);
    }

    IR::ResultAndCarry<u32> ThumbExpandImm_C(Imm<1> i, Imm<3> imm3, Imm<8> imm8, IR::U1 carry_in) {
        const u32 imm32 = ThumbExpandImm(i, imm3, imm8);
        auto carry_out = carry_in;
        if (imm3.Bit<2>() != 0 || i != 0) {
            carry_out = ir.Imm1(Common::Bit<31>(imm32));
        }
        return {imm32, carry_out};
    }

    IR::ResultAndCarry<IR::U32> DecodeShiftedReg(Reg n, Imm<3> imm3, Imm<2> imm2, Imm<2> t, IR::U1 carry_in) {
        const auto reg = ir.GetRegister(n);
        u8 shift_n = static_cast<u8>(concatenate(imm3, imm2).ZeroExtend());
        switch (t.ZeroExtend()) {
            case 0b00: {
                const auto result = ir.LogicalShiftLeft(reg, ir.Imm8(shift_n), carry_in);
                return result;
            }
            case 0b01: {
                if (shift_n == 0) {
                    shift_n = 32;
                }
                const auto result = ir.LogicalShiftRight(reg, ir.Imm8(shift_n), carry_in);
                return result;
            }
            case 0b10: {
                if (shift_n == 0) {
                    shift_n = 32;
                }
                const auto result = ir.ArithmeticShiftRight(reg, ir.Imm8(shift_n), carry_in);
                return result;
            } 
            default: { /* 0b11 */
                if (shift_n == 0) {
                    const auto result = ir.RotateRightExtended(reg, carry_in);
                    return result;
                }
                const auto result = ir.RotateRight(reg, ir.Imm8(shift_n), carry_in);
                return result;
            }  
        }
    }
    
    bool is_thumb_16;

    bool ConditionPassed();
    bool RaiseException(Exception exception);

    // thumb16
    bool thumb16_LSL_imm(Imm<5> imm5, Reg m, Reg d);
    bool thumb16_LSR_imm(Imm<5> imm5, Reg m, Reg d);
    bool thumb16_ASR_imm(Imm<5> imm5, Reg m, Reg d);
    bool thumb16_ADD_reg_t1(Reg m, Reg n, Reg d);
    bool thumb16_SUB_reg(Reg m, Reg n, Reg d);
    bool thumb16_ADD_imm_t1(Imm<3> imm3, Reg n, Reg d);
    bool thumb16_SUB_imm_t1(Imm<3> imm3, Reg n, Reg d);
    bool thumb16_MOV_imm(Reg d, Imm<8> imm8);
    bool thumb16_CMP_imm(Reg n, Imm<8> imm8);
    bool thumb16_ADD_imm_t2(Reg d_n, Imm<8> imm8);
    bool thumb16_SUB_imm_t2(Reg d_n, Imm<8> imm8);
    bool thumb16_AND_reg(Reg m, Reg d_n);
    bool thumb16_EOR_reg(Reg m, Reg d_n);
    bool thumb16_LSL_reg(Reg m, Reg d_n);
    bool thumb16_LSR_reg(Reg m, Reg d_n);
    bool thumb16_ASR_reg(Reg m, Reg d_n);
    bool thumb16_ADC_reg(Reg m, Reg d_n);
    bool thumb16_SBC_reg(Reg m, Reg d_n);
    bool thumb16_ROR_reg(Reg m, Reg d_n);
    bool thumb16_TST_reg(Reg m, Reg n);
    bool thumb16_RSB_imm(Reg n, Reg d);
    bool thumb16_CMP_reg_t1(Reg m, Reg n);
    bool thumb16_CMN_reg(Reg m, Reg n);
    bool thumb16_ORR_reg(Reg m, Reg d_n);
    bool thumb16_MUL_reg(Reg n, Reg d_m);
    bool thumb16_BIC_reg(Reg m, Reg d_n);
    bool thumb16_MVN_reg(Reg m, Reg d);
    bool thumb16_ADD_reg_t2(bool d_n_hi, Reg m, Reg d_n_lo);
    bool thumb16_CMP_reg_t2(bool n_hi, Reg m, Reg n_lo);
    bool thumb16_MOV_reg(bool d_hi, Reg m, Reg d_lo);
    bool thumb16_LDR_literal(Reg t, Imm<8> imm8);
    bool thumb16_STR_reg(Reg m, Reg n, Reg t);
    bool thumb16_STRH_reg(Reg m, Reg n, Reg t);
    bool thumb16_STRB_reg(Reg m, Reg n, Reg t);
    bool thumb16_LDRSB_reg(Reg m, Reg n, Reg t);
    bool thumb16_LDR_reg(Reg m, Reg n, Reg t);
    bool thumb16_LDRH_reg(Reg m, Reg n, Reg t);
    bool thumb16_LDRB_reg(Reg m, Reg n, Reg t);
    bool thumb16_LDRSH_reg(Reg m, Reg n, Reg t);
    bool thumb16_STR_imm_t1(Imm<5> imm5, Reg n, Reg t);
    bool thumb16_LDR_imm_t1(Imm<5> imm5, Reg n, Reg t);
    bool thumb16_STRB_imm(Imm<5> imm5, Reg n, Reg t);
    bool thumb16_LDRB_imm(Imm<5> imm5, Reg n, Reg t);
    bool thumb16_STRH_imm(Imm<5> imm5, Reg n, Reg t);
    bool thumb16_LDRH_imm(Imm<5> imm5, Reg n, Reg t);
    bool thumb16_STR_imm_t2(Reg t, Imm<8> imm8);
    bool thumb16_LDR_imm_t2(Reg t, Imm<8> imm8);
    bool thumb16_ADR(Reg d, Imm<8> imm8);
    bool thumb16_ADD_sp_t1(Reg d, Imm<8> imm8);
    bool thumb16_ADD_sp_t2(Imm<7> imm7);
    bool thumb16_SUB_sp(Imm<7> imm7);
    bool thumb16_NOP();
    bool thumb16_SEV();
    bool thumb16_SEVL();
    bool thumb16_WFE();
    bool thumb16_WFI();
    bool thumb16_YIELD();
    bool thumb16_SXTH(Reg m, Reg d);
    bool thumb16_SXTB(Reg m, Reg d);
    bool thumb16_UXTH(Reg m, Reg d);
    bool thumb16_UXTB(Reg m, Reg d);
    bool thumb16_PUSH(bool M, RegList reg_list);
    bool thumb16_POP(bool P, RegList reg_list);
    bool thumb16_SETEND(bool E);
    bool thumb16_CPS(bool, bool, bool, bool);
    bool thumb16_REV(Reg m, Reg d);
    bool thumb16_REV16(Reg m, Reg d);
    bool thumb16_REVSH(Reg m, Reg d);
    bool thumb16_BKPT([[maybe_unused]] Imm<8> imm8);
    bool thumb16_STMIA(Reg n, RegList reg_list);
    bool thumb16_LDMIA(Reg n, RegList reg_list);
    bool thumb16_CBZ_CBNZ(bool nonzero, Imm<1> i, Imm<5> imm5, Reg n);
    bool thumb16_UDF();
    bool thumb16_BX(Reg m);
    bool thumb16_BLX_reg(Reg m);
    bool thumb16_SVC(Imm<8> imm8);
    bool thumb16_B_t1(Cond cond, Imm<8> imm8);
    bool thumb16_B_t2(Imm<11> imm11);
    bool thumb16_IT(Cond firstcond, Imm<4> mask);

    // Load/Store Multiple
    bool thumb32_STMIA(bool W, Reg n, RegList reg_list);
//    bool thumb32_POP(bool P, bool M, RegList reg_list);
    bool thumb32_LDMIA(bool W, Reg n, RegList reg_list);
//    bool thumb32_PUSH(bool M, RegList reg_list);
    bool thumb32_STMDB(bool W, Reg n, RegList reg_list);
    bool thumb32_LDMDB(bool W, Reg n, RegList reg_list);

    // Load/Store Dual, Load/Store Exclusive, Table Branch
    bool thumb32_STREX(Reg n, Reg t, Reg d, Imm<8> imm8);
    bool thumb32_LDREX(Reg n, Reg t, Imm<8> imm8);
//    bool thumb32_STREXB(Reg n, Reg t, Reg d);
    bool thumb32_STREXH(Reg n, Reg d, Reg t);
    bool thumb32_STREXD(Reg n, Reg t, Reg t2, Reg d);
    bool thumb32_TBB(Reg n, Reg m);
    bool thumb32_TBH(Reg n, Reg m);
//    bool thumb32_LDREXB(Reg n, Reg t);
    bool thumb32_LDREXH(Reg n, Reg t);
    bool thumb32_LDREXD(Reg n, Reg t, Reg t2);
    bool thumb32_STRD_imm_2(bool P, bool U, bool W, Reg n, Reg t1, Reg t2, Imm<8> imm8);
    bool thumb32_LDRD_imm_2(bool P, bool U, bool W, Reg n, Reg t1, Reg t2, Imm<8> imm8);

    // Data Processing (Shifted Register)
    bool thumb32_TST_reg(Reg n, Imm<3> imm3, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_AND_reg(bool S, Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_BIC_reg(bool S, Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<2> t, Reg m);
//    bool thumb32_MOV_reg(bool S, Reg d, Reg m);
    bool thumb32_LSL_imm(bool S, Imm<3> imm3, Reg d, Imm<2> imm2, Reg m);
    bool thumb32_LSR_imm(bool S, Imm<3> imm3, Reg d, Imm<2> imm2, Reg m);
    bool thumb32_ASR_imm(bool S, Imm<3> imm3, Reg d, Imm<2> imm2, Reg m);
    bool thumb32_RRX(bool S, Reg d, Reg m);
    bool thumb32_ROR_imm(bool S, Imm<3> imm3, Reg d, Imm<2> imm2, Reg m);
    bool thumb32_ORR_reg(bool S, Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_MVN_reg(bool S, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_ORN_reg(bool S, Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_TEQ_reg(Reg n, Imm<3> imm3, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_EOR_reg(bool S, Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_PKH(Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, bool tb, Reg m);
    bool thumb32_CMN_reg(Reg n, Imm<3> imm3, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_ADD_reg(bool S, Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_ADC_reg(bool S, Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_SBC_reg(bool S, Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_CMP_reg(Reg n, Imm<3> imm3, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_SUB_reg(bool S, Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<2> t, Reg m);
    bool thumb32_RSB_reg(bool S, Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<2> t, Reg m);

    // Data Processing (Modified Immediate)
    bool thumb32_MOV_imm(Imm<1> i, bool S, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_BIC_imm(Imm<1> i, bool S, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_AND_imm(Imm<1> i, bool S, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_ORR_imm(Imm<1> i, bool S, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_TST_imm(Imm<1> i, Reg n, Imm<3> imm3, Imm<8> imm8);
    bool thumb32_MVN_imm(Imm<1> i, bool S, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_ORN_imm(Imm<1> i, bool S, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_TEQ_imm(Imm<1> i, Reg n, Imm<3> imm3, Imm<8> imm8);
    bool thumb32_EOR_imm(Imm<1> i, bool S, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_CMN_imm(Imm<1> i, Reg n, Imm<3> imm3, Imm<8> imm8);
    bool thumb32_ADD_imm_1(Imm<1> i, bool S, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_ADC_imm(Imm<1> i, bool S, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_SBC_imm(Imm<1> i, bool S, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_CMP_imm(Imm<1> i, Reg n, Imm<3> imm3, Imm<8> imm8);
    bool thumb32_SUB_imm_1(Imm<1> i, bool S, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_RSB_imm(Imm<1> i, bool S, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);

    // Data Processing (Plain Binary Immediate)
    bool thumb32_ADR_after(Imm<1> i, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_ADD_imm_2(Imm<1> i, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_MOVW_imm(Imm<1> i, Imm<4> imm4, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_ADR_before(Imm<1> i, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_SUB_imm_2(Imm<1> i, Reg n, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_MOVT(Imm<1> i, Imm<4> imm4, Imm<3> imm3, Reg d, Imm<8> imm8);
    bool thumb32_SSAT(bool sh, Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<5> sat_imm);
    bool thumb32_SSAT16(Reg n, Reg d, Imm<5> sat_imm);
    bool thumb32_SBFX(Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<5> widthm);
    bool thumb32_BFC(Imm<3> imm3, Reg d, Imm<2> imm2, Imm<5> msb);
    bool thumb32_BFI(Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<5> msb);
    bool thumb32_UBFX(Reg n, Imm<3> imm3, Reg d, Imm<2> imm2, Imm<5> widthm1);

    // Branches and Miscellaneous Control
    bool thumb32_DSB(Imm<4> option);
    bool thumb32_DMB(Imm<4> option);
//    bool thumb32_ISB(Imm<4> option);

    // Store Single Data Item
    bool thumb32_STRB_imm_1(Reg n, Reg t, bool p, bool u, bool w, Imm<8> imm8);
    bool thumb32_STRB_imm_2(Reg n, Reg t, Imm<12> imm12);
    bool thumb32_STRB(Reg n, Reg t, Imm<2> shift, Reg m);
    bool thumb32_STRH_imm_1(Reg n, Reg t, bool p, bool u, bool w, Imm<8> imm8);
    bool thumb32_STRH_imm_3(Reg n, Reg t, Imm<12> imm12);
    bool thumb32_STRH(Reg n, Reg t, Imm<2> shift, Reg m);
    bool thumb32_STR_imm_1(Reg n, Reg t, bool p, bool u, bool w, Imm<8> imm8);
    bool thumb32_STR_imm_3(Reg n, Reg t, Imm<12> imm12);
    bool thumb32_STR_reg(Reg n, Reg t, Imm<2> shift, Reg m);

    // Load Byte and Memory Hints
    bool thumb32_PLD_imm12(bool W, Reg n, Imm<12> imm12);
    bool thumb32_LDRB_lit(bool U, Reg t, Imm<12> imm12);
    bool thumb32_LDRB_reg(Reg n, Reg t, Imm<2> shift, Reg m);
    bool thumb32_LDRB_imm8(Reg n, Reg t, bool p, bool u, bool w, Imm<8> imm8);
    bool thumb32_LDRB_imm12(Reg n, Reg t, Imm<12> imm12);
    bool thumb32_LDRSB_lit(bool u, Reg t, Imm<12> imm12);
    bool thumb32_LDRSB_reg(Reg n, Reg t, Imm<2> shift, Reg m);
    bool thumb32_LDRSB_imm8(Reg n, Reg t, bool p, bool u, bool w, Imm<8> imm8);
    bool thumb32_LDRSB_imm12(Reg n, Reg t, Imm<12> imm12);

    // Load Halfword and Memory Hints
    bool thumb32_LDRH_lit(bool u, Reg t, Imm<12> imm12);
    bool thumb32_LDRH_reg(Reg n, Reg t, Imm<2> shift, Reg m);
    bool thumb32_LDRH_imm8(Reg n, Reg t, bool p, bool u, bool w, Imm<8> imm8);
    bool thumb32_LDRH_imm12(Reg n, Reg t, Imm<12> imm12);
    bool thumb32_LDRSH_lit(bool u, Reg t, Imm<12> imm12);
    bool thumb32_LDRSH_reg(Reg n, Reg t, Imm<2> shift, Reg m);
    bool thumb32_LDRSH_imm8(Reg n, Reg t, bool p, bool u, bool w, Imm<8> imm8);
    bool thumb32_LDRSH_imm12(Reg n, Reg t, Imm<12> imm12);

    // Load Word
    bool thumb32_LDR_lit(bool u, Reg t, Imm<12> imm12);
//    bool thumb32_LDRT(Reg n, Reg t, Imm<8> imm8);
    bool thumb32_LDR_reg(Reg n, Reg t, Imm<2> shift, Reg m);
    bool thumb32_LDR_imm8(Reg n, Reg t, bool p, bool u, bool w, Imm<8> imm8);
    bool thumb32_LDR_imm12(Reg n, Reg t, Imm<12> imm12);

    // Data Processing (register)
    bool thumb32_LSL_reg(bool S, Reg n, Reg d, Reg m);
    bool thumb32_LSR_reg(bool S, Reg n, Reg d, Reg m);
    bool thumb32_ROR_reg(bool S, Reg n, Reg d, Reg m);
    bool thumb32_ASR_reg(bool S, Reg n, Reg d, Reg m);
    bool thumb32_SXTH(Reg d, SignExtendRotation rotate, Reg m);
    bool thumb32_UXTH(Reg d, SignExtendRotation rotate, Reg m);
    bool thumb32_SXTB(Reg d, SignExtendRotation rotate, Reg m);
    bool thumb32_UXTB(Reg d, SignExtendRotation rotate, Reg m);
    bool thumb32_UXTAB(Reg n, Reg d, SignExtendRotation rotate, Reg m);
    bool thumb32_UXTAH(Reg n, Reg d, SignExtendRotation rotate, Reg m);

    // Parallel Addition and Subtraction (unsigned)
    bool thumb32_UADD8(Reg n, Reg d, Reg m);

    // Miscellaneous Operations
    bool thumb32_REV(Reg m1, Reg d, Reg m);
    bool thumb32_REV16(Reg m1, Reg d, Reg m);
    bool thumb32_RBIT(Reg m1, Reg d, Reg m);
    bool thumb32_SEL(Reg n, Reg d, Reg m);
    bool thumb32_CLZ(Reg m1, Reg d, Reg m);

    // Multiply, Multiply Accumulate, and Absolute Difference
    bool thumb32_MUL(Reg n, Reg d, Reg m);
    bool thumb32_MLA(Reg n, Reg a, Reg d, Reg m);
    bool thumb32_MLS(Reg n, Reg a, Reg d, Reg m);
    bool thumb32_SMMUL(Reg n, Reg d, bool R, Reg m);
    bool thumb32_SMMLA(Reg n, Reg a, Reg d, bool R, Reg m);

    // Long Multiply, Long Multiply Accumulate, and Divide
    bool thumb32_SDIV(Reg n, Reg d, Reg m);
    bool thumb32_UMULL(Reg n, Reg dLo, Reg dHi, Reg m);
    bool thumb32_UDIV(Reg n, Reg d, Reg m);
    bool thumb32_SMLAXY(Reg n, Reg a, Reg d, bool N, bool M, Reg m);
    bool thumb32_SMLAL(Reg n, Reg dLo, Reg dHi, Reg m);
    bool thumb32_UMLAL(Reg n, Reg dLo, Reg dHi, Reg m);
    bool thumb32_UMAAL(Reg n, Reg dLo, Reg dHi, Reg m);

    // Coprocessor
    bool thumb32_MRC(size_t opc1, CoprocReg CRn, Reg t, size_t coproc, size_t opc2, CoprocReg CRm);

    // Branch instructions
    bool thumb32_BL_imm(bool S, Imm<10> hi, bool j1, bool j2, Imm<11> lo);
    bool thumb32_BLX_imm(bool S, Imm<10> hi, bool j1, bool j2, Imm<11> lo);
    bool thumb32_B_cond(Imm<1> S, Cond cond, Imm<6> imm6, Imm<1> j1, Imm<1> j2, Imm<11> imm11);
    bool thumb32_B(bool S, Imm<10> imm10, bool j1, bool j2, Imm<11> imm11);

    // Three registers of the same length
    bool asimd_VAND_reg(bool D, size_t Vn, size_t Vd, bool N, bool Q, bool M, size_t Vm);

    // Two registers and a shift amount
    bool asimd_SHR(bool U, bool D, size_t imm6, size_t Vd, bool L, bool Q, bool M, size_t Vm);

    // One register and modified immediate
    bool asimd_VMOV_imm(Imm<1> a, bool D, Imm<1> b, Imm<1> c, Imm<1> d, size_t Vd,
                        Imm<4> cmode, bool Q, bool op, Imm<1> e, Imm<1> f, Imm<1> g, Imm<1> h);

    // Advanced SIMD load/store structures
    bool v8_VST_multiple(bool D, Reg n, size_t Vd, Imm<4> type, size_t size, size_t align, Reg m);
    bool v8_VLD_multiple(bool D, Reg n, size_t Vd, Imm<4> type, size_t sz, size_t align, Reg m);
    bool v8_VLD_all_lanes(bool D, Reg n, size_t Vd, size_t nn, size_t sz, bool T, bool a, Reg m);
    bool v8_VLD_single(bool D, Reg n, size_t Vd, size_t sz, size_t nn, size_t index_align, Reg m);

    // Advanced SIMD three registers with different lengths
    bool asimd_VMULL(bool U, bool D, size_t sz, size_t Vn, size_t Vd, bool P, bool N, bool M, size_t Vm);

    // Advanced SIMD three register with same length
    bool asimd_VADD_int(bool D, size_t sz, size_t Vn, size_t Vd, bool N, bool Q, bool M, size_t Vm);

    // Floating-point three-register data processing instructions
    bool vfp_VMLA(bool D, size_t Vn, size_t Vd, bool sz, bool N, bool M, size_t Vm);
    bool vfp_VMLS(bool D, size_t Vn, size_t Vd, bool sz, bool N, bool M, size_t Vm);
    bool vfp_VNMLS(bool D, size_t Vn, size_t Vd, bool sz, bool N, bool M, size_t Vm);
    bool vfp_VMUL(bool D, size_t Vn, size_t Vd, bool sz, bool N, bool M, size_t Vm);
    bool vfp_VNMUL(bool D, size_t Vn, size_t Vd, bool sz, bool N, bool M, size_t Vm);
    bool vfp_VADD(bool D, size_t Vn, size_t Vd, bool sz, bool N, bool M, size_t Vm);
    bool vfp_VSUB(bool D, size_t Vn, size_t Vd, bool sz, bool N, bool M, size_t Vm);
    bool vfp_VDIV(bool D, size_t Vn, size_t Vd, bool sz, bool N, bool M, size_t Vm);

    // Other floating-point data-processing instructions
    bool vfp_VMOV_imm(bool D, Imm<4> imm4H, size_t Vd, bool sz, Imm<4> imm4L);
    bool vfp_VMOV_reg(bool D, size_t Vd, bool sz, bool M, size_t Vm);
    bool vfp_VABS(bool D, size_t Vd, bool sz, bool M, size_t Vm);
    bool vfp_VNEG(bool D, size_t Vd, bool sz, bool M, size_t Vm);
    bool vfp_VSQRT(bool D, size_t Vd, bool sz, bool M, size_t Vm);
    bool vfp_VCMP(bool D, size_t Vd, bool sz, bool E, bool M, size_t Vm);
    bool vfp_VCMP_zero(bool D, size_t Vd, bool sz, bool E);
    bool vfp_VCVT_f_to_f(bool D, size_t Vd, bool sz, bool M, size_t Vm);
    bool vfp_VCVT_from_int(bool D, size_t Vd, bool sz, bool is_signed, bool M, size_t Vm);
    bool vfp_VCVT_to_u32(bool D, size_t Vd, bool sz, bool round_towards_zero, bool M, size_t Vm);
    bool vfp_VCVT_to_s32(bool D, size_t Vd, bool sz, bool round_towards_zero, bool M, size_t Vm);

    // Floating-point move instructions
    bool vfp_VMOV_u32_f32(size_t Vn, Reg t, bool N);
    bool vfp_VMOV_f32_u32(size_t Vn, Reg t, bool N);
    bool vfp_VMOV_2u32_f64(Reg t2, Reg t1, bool M, size_t Vm);
    bool vfp_VMOV_f64_2u32(Reg t2, Reg t1, bool M, size_t Vm);
    bool vfp_VDUP(Imm<1> B, bool Q, size_t Vd, Reg t, bool D, Imm<1> E);

    // Floating-point system register access
    bool vfp_VMRS(Reg t);

    // Extension register load-store instructions
    bool vfp_VPUSH(bool D, size_t Vd, bool sz, Imm<8> imm8);
    bool vfp_VPOP(bool D, size_t Vd, bool sz, Imm<8> imm8);
    bool vfp_VLDR(bool U, bool D, Reg n, size_t Vd, bool sz, Imm<8> imm8);
    bool vfp_VSTR(bool U, bool D, Reg n, size_t Vd, bool sz, Imm<8> imm8);
    bool vfp_VSTM_a1(bool p, bool u, bool D, bool w, Reg n, size_t Vd, Imm<8> imm8);
    bool vfp_VLDM_a1(bool p, bool u, bool D, bool w, Reg n, size_t Vd, Imm<8> imm8);
    bool vfp_VLDM_a2(bool p, bool u, bool D, bool w, Reg n, size_t Vd, Imm<8> imm8);
    bool vfp_VSTM_a2(bool p, bool u, bool D, bool w, Reg n, size_t Vd, Imm<8> imm8);

    // Misc instructions
    bool thumb32_UDF();
};

} // namespace Dynarmic::A32
