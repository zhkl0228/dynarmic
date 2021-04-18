// Floating-point three-register data processing instructions
INST(vfp_VMLA,             "VMLA",                    "111011100D00nnnndddd101zN0M0mmmm") // VFPv2
INST(vfp_VMLS,             "VMLS",                    "111011100D00nnnndddd101zN1M0mmmm") // VFPv2
INST(vfp_VNMLS,            "VNMLS",                   "111011100D01nnnndddd101zN0M0mmmm") // VFPv2
//INST(vfp_VNMLA,            "VNMLA",                   "cccc11100D01nnnndddd101zN1M0mmmm") // VFPv2
INST(vfp_VMUL,             "VMUL",                    "111011100D10nnnndddd101zN0M0mmmm") // VFPv2
INST(vfp_VNMUL,            "VNMUL",                   "111011100D10nnnndddd101zN1M0mmmm") // VFPv2
INST(vfp_VADD,             "VADD",                    "111011100D11nnnndddd101zN0M0mmmm") // VFPv2
INST(vfp_VSUB,             "VSUB",                    "111011100D11nnnndddd101zN1M0mmmm") // VFPv2
INST(vfp_VDIV,             "VDIV",                    "111011101D00nnnndddd101zN0M0mmmm") // VFPv2
//INST(vfp_VFNMS,            "VFNMS",                   "cccc11101D01nnnndddd101zN0M0mmmm") // VFPv4
//INST(vfp_VFNMA,            "VFNMA",                   "cccc11101D01nnnndddd101zN1M0mmmm") // VFPv4
//INST(vfp_VFMA,             "VFMA",                    "cccc11101D10nnnndddd101zN0M0mmmm") // VFPv4
//INST(vfp_VFMS,             "VFMS",                    "cccc11101D10nnnndddd101zN1M0mmmm") // VFPv4
//INST(vfp_VSEL,             "VSEL",                    "111111100Dccnnnndddd101zN0M0mmmm") // VFPv5
//INST(vfp_VMAXNM,           "VMAXNNM",                 "111111101D00nnnndddd101zN0M0mmmm") // VFPv5
//INST(vfp_VMINNM,           "VMINNM",                  "111111101D00nnnndddd101zN1M0mmmm") // VFPv5

// Other floating-point data-processing instructions
INST(vfp_VMOV_imm,         "VMOV (immediate)",        "111011101D11vvvvdddd101z0000vvvv") // VFPv3
INST(vfp_VMOV_reg,         "VMOV (reg)",              "111011101D110000dddd101z01M0mmmm") // VFPv2
INST(vfp_VABS,             "VABS",                    "111011101D110000dddd101z11M0mmmm") // VFPv2
INST(vfp_VNEG,             "VNEG",                    "111011101D110001dddd101z01M0mmmm") // VFPv2
INST(vfp_VSQRT,            "VSQRT",                   "111011101D110001dddd101z11M0mmmm") // VFPv2
//INST(vfp_VCVTB,            "VCVTB",                   "cccc11101D11001odddd101z01M0mmmm") // VFPv3HP
//INST(vfp_VCVTT,            "VCVTT",                   "cccc11101D11001odddd101z11M0mmmm") // VFPv3HP
INST(vfp_VCMP,             "VCMP",                    "111011101D110100dddd101zE1M0mmmm") // VFPv2
INST(vfp_VCMP_zero,        "VCMP (with zero)",        "111011101D110101dddd101zE1000000") // VFPv2
//INST(vfp_VRINTR,           "VRINTR",                  "cccc11101D110110dddd101z01M0mmmm") // VFPv5
//INST(vfp_VRINTZ,           "VRINTZ",                  "cccc11101D110110dddd101z11M0mmmm") // VFPv5
//INST(vfp_VRINTX,           "VRINTX",                  "cccc11101D110111dddd101z01M0mmmm") // VFPv5
INST(vfp_VCVT_f_to_f,      "VCVT (f32<->f64)",        "111011101D110111dddd101z11M0mmmm") // VFPv2
INST(vfp_VCVT_from_int,    "VCVT (from int)",         "111011101D111000dddd101zs1M0mmmm") // VFPv2
//INST(vfp_VCVT_from_fixed,  "VCVT (from fixed)",       "cccc11101D11101Udddd101zx1i0vvvv") // VFPv3
INST(vfp_VCVT_to_u32,      "VCVT (to u32)",           "111011101D111100dddd101zr1M0mmmm") // VFPv2
INST(vfp_VCVT_to_s32,      "VCVT (to s32)",           "111011101D111101dddd101zr1M0mmmm") // VFPv2
//INST(vfp_VCVT_to_fixed,    "VCVT (to fixed)",         "cccc11101D11111Udddd101zx1i0vvvv") // VFPv3
//INST(vfp_VRINT_rm,         "VRINT{A,N,P,M}",          "111111101D1110mmdddd101z01M0mmmm") // VFPv5
//INST(vfp_VCVT_rm,          "VCVT{A,N,P,M}",           "111111101D1111mmdddd101zU1M0mmmm") // VFPv5

// Floating-point move instructions
//INST(vfp_VMOV_u32_f64,     "VMOV (core to f64)",      "cccc11100000ddddtttt1011D0010000") // VFPv2
//INST(vfp_VMOV_f64_u32,     "VMOV (f64 to core)",      "cccc11100001nnnntttt1011N0010000") // VFPv2
INST(vfp_VMOV_u32_f32,     "VMOV (core to f32)",      "111011100000nnnntttt1010N0010000") // VFPv2
INST(vfp_VMOV_f32_u32,     "VMOV (f32 to core)",      "111011100001nnnntttt1010N0010000") // VFPv2
//INST(vfp_VMOV_2u32_2f32,   "VMOV (2xcore to 2xf32)",  "cccc11000100uuuutttt101000M1mmmm") // VFPv2
//INST(vfp_VMOV_2f32_2u32,   "VMOV (2xf32 to 2xcore)",  "cccc11000101uuuutttt101000M1mmmm") // VFPv2
INST(vfp_VMOV_2u32_f64,    "VMOV (2xcore to f64)",    "111011000100uuuutttt101100M1mmmm") // VFPv2
INST(vfp_VMOV_f64_2u32,    "VMOV (f64 to 2xcore)",    "111011000101uuuutttt101100M1mmmm") // VFPv2
INST(vfp_VMOV_from_i32,    "VMOV (core to i32)" ,     "1110111000i0nnnntttt1011N0010000") // VFPv4
//INST(vfp_VMOV_from_i16,    "VMOV (core to i16)" ,     "cccc111000i0nnnntttt1011Ni110000") // ASIMD
//INST(vfp_VMOV_from_i8,     "VMOV (core to i8)",       "cccc111001i0nnnntttt1011Nii10000") // ASIMD
INST(vfp_VMOV_to_i32,      "VMOV (i32 to core)" ,     "1110111000i1nnnntttt1011N0010000") // VFPv4
//INST(vfp_VMOV_to_i16,      "VMOV (i16 to core)" ,     "cccc1110U0i1nnnntttt1011Ni110000") // ASIMD
//INST(vfp_VMOV_to_i8,       "VMOV (i8 to core)",       "cccc1110U1i1nnnntttt1011Nii10000") // ASIMD
INST(vfp_VDUP,             "VDUP (from core)",        "111011101BQ0ddddtttt1011D0E10000") // ASIMD

// Floating-point system register access
//INST(vfp_VMSR,             "VMSR",                    "cccc111011100001tttt101000010000") // VFPv2
INST(vfp_VMRS,             "VMRS",                    "1110111011110001tttt101000010000") // VFPv2

// Extension register load-store instructions
INST(vfp_VPUSH,            "VPUSH",                   "111011010D101101dddd101zvvvvvvvv") // VFPv2
INST(vfp_VPOP,             "VPOP",                    "111011001D111101dddd101zvvvvvvvv") // VFPv2
INST(vfp_VLDR,             "VLDR",                    "11101101UD01nnnndddd101zvvvvvvvv") // VFPv2
INST(vfp_VSTR,             "VSTR",                    "11101101UD00nnnndddd101zvvvvvvvv") // VFPv2
INST(thumb32_UDF,          "Undefined VSTM/VLDM",     "111011000-0---------101---------") // VFPv2
INST(vfp_VSTM_a1,          "VSTM (T1)",               "1110110puDw0nnnndddd1011vvvvvvvv") // VFPv2
INST(vfp_VSTM_a2,          "VSTM (A2)",               "1110110puDw0nnnndddd1010vvvvvvvv") // VFPv2
INST(vfp_VLDM_a1,          "VLDM (T1)",               "1110110puDw1nnnndddd1011vvvvvvvv") // VFPv2
INST(vfp_VLDM_a2,          "VLDM (T2)",               "1110110puDw1nnnndddd1010vvvvvvvv") // VFPv2
