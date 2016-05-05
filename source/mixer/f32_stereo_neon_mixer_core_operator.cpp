#include <cxxdasp/mixer/f32_stereo_neon_mixer_core_operator.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

#include <cxxporthelper/cstdint>
#include <cxxporthelper/arm_neon.hpp>

#include <cxxdasp/utils/utils.hpp>

#include <iostream>

namespace cxxdasp {
namespace mixer {

#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
#define AARCH32_NEON_F32_STEREO_MUL_SCALE(SRC_ALIGN, DEST_ALIGN)                                                       \
    n >>= 3;                                                                                                           \
    asm volatile("pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #128]\n\t"                                                                             \
                 "vld1.32 {d16}, [%[scale]]\n\t"                                                                       \
                 "vmov.32 d17, d16\n"                                                                                  \
                 "1:\n\t"                                                                                              \
                 "vld1.32 {q0, q1}, [%[src]" SRC_ALIGN "]!\n\t"                                                        \
                 "vmul.f32 q0, q0, q8\n\t"                                                                             \
                 "vmul.f32 q1, q1, q8\n\t"                                                                             \
                 "\n\t"                                                                                                \
                 "vld1.32 {q2, q3}, [%[src]" SRC_ALIGN "]!\n\t"                                                        \
                 "vmul.f32 q2, q2, q8\n\t"                                                                             \
                 "vmul.f32 q3, q3, q8\n\t"                                                                             \
                 "\n\t"                                                                                                \
                 "vst1.32 {q0, q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #160]\n\t"                                                                             \
                 "subs %[cnt], %[cnt], #1\n\t"                                                                         \
                 "\n\t"                                                                                                \
                 "vst1.32 {q2, q3}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)                                                   \
                 : [scale] "r"(&scale)                                                                                 \
                 : "memory", "q0", "q1", "q2", "q3", "q8")
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
#define AARCH64_NEON_F32_STEREO_MUL_SCALE(SRC_ALIGN, DEST_ALIGN)                                                       \
    n >>= 3;                                                                                                           \
    asm volatile("prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pstl1strm, [%[dest], #128]\n\t"                                                                 \
                 "ld1 {v4.2s}, [%[scale]]\n\t"                                                                         \
                 "ins v4.2d[1], v4.2d[0]\n"                                                                            \
                 "1:\n\t"                                                                                              \
                 "ld1 {v0." SRC_ALIGN ", v1." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmul v0.4s, v0.4s, v4.4s\n\t"                                                                        \
                 "fmul v1.4s, v1.4s, v4.4s\n\t"                                                                        \
                 "\n\t"                                                                                                \
                 "ld1 {v2." SRC_ALIGN ", v3." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmul v2.4s, v2.4s, v4.4s\n\t"                                                                        \
                 "fmul v3.4s, v3.4s, v4.4s\n\t"                                                                        \
                 "st1 {v0." DEST_ALIGN ", v1." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pstl1strm, [%[dest], #160]\n\t"                                                                 \
                 "subs %w[cnt], %w[cnt], #1\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v2." DEST_ALIGN ", v3." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)                                                   \
                 : [scale] "r"(&scale)                                                                                 \
                 : "memory", "v0", "v1", "v2", "v3", "v4")
#endif

static void neon_f32_stereo_mul_scale_src_aligned_dest_aligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MUL_SCALE(":128", ":128");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MUL_SCALE("2d", "2d");
#endif
}

static void neon_f32_stereo_mul_scale_src_aligned_dest_unaligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MUL_SCALE(":128", "");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MUL_SCALE("2d", "4s");
#endif
}

#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
#define AARCH32_NEON_F32_STEREO_MAC_SCALE(SRC_ALIGN, DEST_ALIGN)                                                       \
    n >>= 3;                                                                                                           \
    asm volatile("pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #128]\n\t"                                                                             \
                 "vld1.32 {d24}, [%[scale]]\n\t"                                                                       \
                 "vmov.32 d25, d24\n"                                                                                  \
                 "1:\n\t"                                                                                              \
                 "vld1.32 {q0, q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "vld1.32 {q8, q9}, [%[src]" SRC_ALIGN "]!\n\t"                                                        \
                 "vmla.f32 q0, q8, q12\n\t"                                                                            \
                 "vmla.f32 q1, q9, q12\n\t"                                                                            \
                 "\n\t"                                                                                                \
                 "vld1.32 {q2, q3}, [%[dest]" DEST_ALIGN "], %[offset_m32]\n\t"                                        \
                 "vld1.32 {q10, q11}, [%[src]" SRC_ALIGN "]!\n\t"                                                      \
                 "vmla.f32 q2, q10, q12\n\t"                                                                           \
                 "vmla.f32 q3, q11, q12\n\t"                                                                           \
                 "\n\t"                                                                                                \
                 "vst1.32 {q0, q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #160]\n\t"                                                                             \
                 "subs %[cnt], %[cnt], #1\n\t"                                                                         \
                 "\n\t"                                                                                                \
                 "vst1.32 {q2, q3}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)                                                   \
                 : [scale] "r"(&scale), [offset_m32] "r"(-32)                                                          \
                 : "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12")
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
#define AARCH64_NEON_F32_STEREO_MAC_SCALE(SRC_ALIGN, DEST_ALIGN)                                                       \
    n >>= 3;                                                                                                           \
    asm volatile("prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pldl1strm, [%[dest], #128]\n\t"                                                                 \
                 "ld1 {v16.2s}, [%[scale]]\n\t"                                                                        \
                 "ins v16.2d[1], v16.2d[0]\n"                                                                          \
                 "1:\n\t"                                                                                              \
                 "ld1 {v0." DEST_ALIGN ", v1." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "ld1 {v4." SRC_ALIGN ", v5." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmla v0.4s, v4.4s, v16.4s\n\t"                                                                       \
                 "fmla v1.4s, v5.4s, v16.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "ld1 {v2." DEST_ALIGN ", v3." DEST_ALIGN "}, [%[dest]], %[offset_m32]\n\t"                            \
                 "ld1 {v6." SRC_ALIGN ", v7." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmla v2.4s, v6.4s, v16.4s\n\t"                                                                       \
                 "fmla v3.4s, v7.4s, v16.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v0." DEST_ALIGN ", v1." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pldl1strm, [%[dest], #160]\n\t"                                                                 \
                 "subs %w[cnt], %w[cnt], #1\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v2." DEST_ALIGN ", v3." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)                                                   \
                 : [scale] "r"(&scale), [offset_m32] "r"(-32LL)                                                        \
                 : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16")
#endif

static void
neon_mac_scale_src_aligned_dest_aligned(f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
                                        const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
                                        const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n)
    CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MAC_SCALE(":128", ":128");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MAC_SCALE("2d", "2d");
#endif
}

static void
neon_mac_scale_src_unaligned_dest_aligned(f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
                                          const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
                                          const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n)
    CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MAC_SCALE("", ":128");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MAC_SCALE("4s", "2d");
#endif
}

#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
#define AARCH32_NEON_F32_STEREO_MUL_SCALE_AND_FORWARD_TABLE(SRC_ALIGN, DEST_ALIGN, TBL_ALIGN)                          \
    n >>= 3;                                                                                                           \
    asm volatile("pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #128]\n\t"                                                                             \
                 "pld [%[table], #64]\n\t"                                                                             \
                 "vld1.32 {d24}, [%[scale]]\n\t"                                                                       \
                 "vmov.32 d25, d24\n"                                                                                  \
                 "1:\n\t"                                                                                              \
                 "vld1.32 {q8}, [%[table]" TBL_ALIGN "]!\n\t"                                                          \
                 "vmov q9, q8\n\t"                                                                                     \
                 "vzip.32 q8, q9\n\t"                                                                                  \
                 "vmul.f32 q8, q8, q12\n\t"                                                                            \
                 "vmul.f32 q9, q9, q12\n\t"                                                                            \
                 "\n\t"                                                                                                \
                 "vld1.32 {q0, q1}, [%[src]" SRC_ALIGN "]!\n\t"                                                        \
                 "vmul.f32 q0, q0, q8\n\t"                                                                             \
                 "vmul.f32 q1, q1, q9\n\t"                                                                             \
                 "\n\t"                                                                                                \
                 "vld1.32 {q10}, [%[table]" TBL_ALIGN "]!\n\t"                                                         \
                 "vmov q11, q10\n\t"                                                                                   \
                 "vzip.32 q10, q11\n\t"                                                                                \
                 "vmul.f32 q10, q10, q12\n\t"                                                                          \
                 "vmul.f32 q11, q11, q12\n\t"                                                                          \
                 "\n\t"                                                                                                \
                 "pld [%[table], #64]\n\t"                                                                             \
                 "\n\t"                                                                                                \
                 "vld1.32 {q2, q3}, [%[src]" SRC_ALIGN "]!\n\t"                                                        \
                 "vmul.f32 q2, q2, q10\n\t"                                                                            \
                 "vmul.f32 q3, q3, q11\n\t"                                                                            \
                 "\n\t"                                                                                                \
                 "vst1.32 {q0, q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #160]\n\t"                                                                             \
                 "subs %[cnt], %[cnt], #1\n\t"                                                                         \
                 "\n\t"                                                                                                \
                 "vst1.32 {q2, q3}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [table] "+r"(table), [cnt] "+r"(n)                              \
                 : [scale] "r"(&scale)                                                                                 \
                 : "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12")
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
#define AARCH64_NEON_F32_STEREO_MUL_SCALE_AND_FORWARD_TABLE(SRC_ALIGN, DEST_ALIGN, TBL_ALIGN)                          \
    n >>= 3;                                                                                                           \
    asm volatile("prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pstl1strm, [%[dest], #128]\n\t"                                                                 \
                 "prfm pldl1strm, [%[table], #64]\n\t"                                                                 \
                 "ld1 {v20.2s}, [%[scale]]\n\t"                                                                        \
                 "ins v20.2d[1], v20.2d[0]\n"                                                                          \
                 "1:\n\t"                                                                                              \
                 "ld1 {v16." TBL_ALIGN "}, [%[table]], #16\n\t"                                                        \
                 "zip2 v17.4s, v16.4s, v16.4s\n\t"                                                                     \
                 "zip1 v16.4s, v16.4s, v16.4s\n\t"                                                                     \
                 "fmul v16.4s, v16.4s, v20.4s\n\t"                                                                     \
                 "fmul v17.4s, v17.4s, v20.4s\n\t"                                                                     \
                 "\n\t"                                                                                                \
                 "ld1 {v0." SRC_ALIGN ", v1." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmul v0.4s, v0.4s, v16.4s\n\t"                                                                       \
                 "fmul v1.4s, v1.4s, v17.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "ld1 {v18." TBL_ALIGN "}, [%[table]], #16\n\t"                                                        \
                 "zip2 v19.4s, v18.4s, v18.4s\n\t"                                                                     \
                 "zip1 v18.4s, v18.4s, v18.4s\n\t"                                                                     \
                 "fmul v18.4s, v18.4s, v20.4s\n\t"                                                                     \
                 "fmul v19.4s, v19.4s, v20.4s\n\t"                                                                     \
                 "\n\t"                                                                                                \
                 "prfm pldl1strm, [%[table], #64]\n\t"                                                                 \
                 "\n\t"                                                                                                \
                 "ld1 {v2." SRC_ALIGN ", v3." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmul v2.4s, v2.4s, v18.4s\n\t"                                                                       \
                 "fmul v3.4s, v3.4s, v19.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v0." DEST_ALIGN ", v1." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pstl1strm, [%[dest], #160]\n\t"                                                                 \
                 "subs %w[cnt], %w[cnt], #1\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v2." DEST_ALIGN ", v3." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [table] "+r"(table), [cnt] "+r"(n)                              \
                 : [scale] "r"(&scale)                                                                                 \
                 : "memory", "v0", "v1", "v2", "v3", "v16", "v17", "v18", "v19", "v20")
#endif

void neon_f32_stereo_mul_forward_table_and_scale_src_aligned_dest_aligned_table_aligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MUL_SCALE_AND_FORWARD_TABLE(":128", ":128", ":128");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MUL_SCALE_AND_FORWARD_TABLE("2d", "2d", "2d");
#endif
}

void neon_f32_stereo_mul_forward_table_and_scale_src_aligned_dest_aligned_table_unaligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MUL_SCALE_AND_FORWARD_TABLE(":128", ":128", "");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MUL_SCALE_AND_FORWARD_TABLE("2d", "2d", "4s");
#endif
}

void neon_f32_stereo_mul_forward_table_and_scale_src_aligned_dest_unaligned_table_aligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MUL_SCALE_AND_FORWARD_TABLE(":128", "", ":128");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MUL_SCALE_AND_FORWARD_TABLE("2d", "4s", "2d");
#endif
}

void neon_f32_stereo_mul_forward_table_and_scale_src_aligned_dest_unaligned_table_unaligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MUL_SCALE_AND_FORWARD_TABLE(":128", "", "");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MUL_SCALE_AND_FORWARD_TABLE("2d", "4s", "4s");
#endif
}

#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
#define AARCH32_NEON_F32_STEREO_MAC_SCALE_AND_FORWARD_TABLE(SRC_ALIGN, DEST_ALIGN, TBL_ALIGN)                          \
    n >>= 3;                                                                                                           \
    asm volatile("pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #128]\n\t"                                                                             \
                 "pld [%[table], #64]\n\t"                                                                             \
                 "vld1.32 {d8}, [%[scale]]\n\t"                                                                        \
                 "vmov.32 d9, d8\n"                                                                                    \
                 "1:\n\t"                                                                                              \
                 "vld1.32 {q8}, [%[table]" TBL_ALIGN "]!\n\t"                                                          \
                 "vmov q9, q8\n\t"                                                                                     \
                 "vzip.32 q8, q9\n\t"                                                                                  \
                 "vmul.f32 q8, q8, q4\n\t"                                                                             \
                 "vmul.f32 q9, q9, q4\n\t"                                                                             \
                 "\n\t"                                                                                                \
                 "vld1.32 {q0, q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "vld1.32 {q12, q13}, [%[src]" SRC_ALIGN "]!\n\t"                                                      \
                 "vmla.f32 q0, q12, q8\n\t"                                                                            \
                 "vmla.f32 q1, q13, q9\n\t"                                                                            \
                 "\n\t"                                                                                                \
                 "vld1.32 {q10}, [%[table]" TBL_ALIGN "]!\n\t"                                                         \
                 "vmov q11, q10\n\t"                                                                                   \
                 "vzip.32 q10, q11\n\t"                                                                                \
                 "vmul.f32 q10, q10, q4\n\t"                                                                           \
                 "vmul.f32 q11, q11, q4\n\t"                                                                           \
                 "\n\t"                                                                                                \
                 "pld [%[table], #64]\n\t"                                                                             \
                 "\n\t"                                                                                                \
                 "vld1.32 {q2, q3}, [%[dest]" DEST_ALIGN "], %[offset_m32]\n\t"                                        \
                 "vld1.32 {q14, q15}, [%[src]" SRC_ALIGN "]!\n\t"                                                      \
                 "vmla.f32 q2, q14, q10\n\t"                                                                           \
                 "vmla.f32 q3, q15, q11\n\t"                                                                           \
                 "\n\t"                                                                                                \
                 "vst1.32 {q0, q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #160]\n\t"                                                                             \
                 "subs %[cnt], %[cnt], #1\n\t"                                                                         \
                 "\n\t"                                                                                                \
                 "vst1.32 {q2, q3}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [table] "+r"(table), [cnt] "+r"(n)                              \
                 : [scale] "r"(&scale), [offset_m32] "r"(-32)                                                          \
                 : "memory", "q0", "q1", "q2", "q3", "q4", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15")
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
#define AARCH64_NEON_F32_STEREO_MAC_SCALE_AND_FORWARD_TABLE(SRC_ALIGN, DEST_ALIGN, TBL_ALIGN)                          \
    n >>= 3;                                                                                                           \
    asm volatile("prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pldl1strm, [%[dest], #128]\n\t"                                                                 \
                 "prfm pldl1strm, [%[table], #64]\n\t"                                                                 \
                 "ld1 {v20.2s}, [%[scale]]\n\t"                                                                        \
                 "ins v20.2d[1], v20.2d[0]\n"                                                                          \
                 "1:\n\t"                                                                                              \
                 "ld1 {v16." TBL_ALIGN "}, [%[table]], #16\n\t"                                                        \
                 "zip2 v17.4s, v16.4s, v16.4s\n\t"                                                                     \
                 "zip1 v16.4s, v16.4s, v16.4s\n\t"                                                                     \
                 "fmul v16.4s, v16.4s, v20.4s\n\t"                                                                     \
                 "fmul v17.4s, v17.4s, v20.4s\n\t"                                                                     \
                 "\n\t"                                                                                                \
                 "ld1 {v0." DEST_ALIGN ", v1." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "ld1 {v4." SRC_ALIGN ", v5." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmla v0.4s, v4.4s, v16.4s\n\t"                                                                       \
                 "fmla v1.4s, v5.4s, v17.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "ld1 {v18." TBL_ALIGN "}, [%[table]], #16\n\t"                                                        \
                 "zip2 v19.4s, v18.4s, v18.4s\n\t"                                                                     \
                 "zip1 v18.4s, v18.4s, v18.4s\n\t"                                                                     \
                 "fmul v18.4s, v18.4s, v20.4s\n\t"                                                                     \
                 "fmul v19.4s, v19.4s, v20.4s\n\t"                                                                     \
                 "\n\t"                                                                                                \
                 "prfm pldl1strm, [%[table], #64]\n\t"                                                                 \
                 "\n\t"                                                                                                \
                 "ld1 {v2." DEST_ALIGN ", v3." DEST_ALIGN "}, [%[dest]], %[offset_m32]\n\t"                            \
                 "ld1 {v6." SRC_ALIGN ", v7." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmla v2.4s, v6.4s, v18.4s\n\t"                                                                       \
                 "fmla v3.4s, v7.4s, v19.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v0." DEST_ALIGN ", v1." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pldl1strm, [%[dest], #160]\n\t"                                                                 \
                 "subs %w[cnt], %w[cnt], #1\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v2." DEST_ALIGN ", v3." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [table] "+r"(table), [cnt] "+r"(n)                              \
                 : [scale] "r"(&scale), [offset_m32] "r"(-32LL)                                                        \
                 : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16", "v17", "v18", "v19", "v20")
#endif

void neon_mac_forward_table_and_scale_src_aligned_dest_aligned_table_aligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MAC_SCALE_AND_FORWARD_TABLE(":128", ":128", ":128");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MAC_SCALE_AND_FORWARD_TABLE("2d", "2d", "2d");
#endif
}

void neon_mac_forward_table_and_scale_src_aligned_dest_aligned_table_unaligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MAC_SCALE_AND_FORWARD_TABLE(":128", ":128", "");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MAC_SCALE_AND_FORWARD_TABLE("2d", "2d", "4s");
#endif
}

void neon_mac_forward_table_and_scale_src_unaligned_dest_aligned_table_aligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MAC_SCALE_AND_FORWARD_TABLE("", ":128", ":128");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MAC_SCALE_AND_FORWARD_TABLE("4s", "2d", "2d");
#endif
}

void neon_mac_forward_table_and_scale_src_unaligned_dest_aligned_table_unaligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MAC_SCALE_AND_FORWARD_TABLE("", ":128", "");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MAC_SCALE_AND_FORWARD_TABLE("4s", "2d", "4s");
#endif
}

#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
#define AARCH32_NEON_F32_STEREO_MUL_SCALE_AND_BACKWARD_TABLE(SRC_ALIGN, DEST_ALIGN, TBL_ALIGN)                         \
    n >>= 3;                                                                                                           \
    asm volatile("pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #128]\n\t"                                                                             \
                 "pld [%[table], #(-64)]\n\t"                                                                          \
                 "sub %[table], %[table], #(4 * 3)\n\t"                                                                \
                 "vld1.32 {d24}, [%[scale]]\n\t"                                                                       \
                 "vmov.32 d25, d24\n"                                                                                  \
                 "1:\n\t"                                                                                              \
                 "vld1.32 {q8}, [%[table]" TBL_ALIGN "], %[offset_m16]\n\t"                                            \
                 "vrev64.32 q8, q8\n\t"                                                                                \
                 "vmov q9, q8\n\t"                                                                                     \
                 "vzip.32 q9, q8\n\t"                                                                                  \
                 "vmul.f32 q8, q8, q12\n\t"                                                                            \
                 "vmul.f32 q9, q9, q12\n\t"                                                                            \
                 "\n\t"                                                                                                \
                 "vld1.32 {q0, q1}, [%[src]" SRC_ALIGN "]!\n\t"                                                        \
                 "vmul.f32 q0, q0, q8\n\t"                                                                             \
                 "vmul.f32 q1, q1, q9\n\t"                                                                             \
                 "\n\t"                                                                                                \
                 "vld1.32 {q10}, [%[table]" TBL_ALIGN "], %[offset_m16]\n\t"                                           \
                 "vrev64.32 q10, q10\n\t"                                                                              \
                 "vmov q11, q10\n\t"                                                                                   \
                 "vzip.32 q11, q10\n\t"                                                                                \
                 "vmul.f32 q10, q10, q12\n\t"                                                                          \
                 "vmul.f32 q11, q11, q12\n\t"                                                                          \
                 "\n\t"                                                                                                \
                 "pld [%[table], #(-64)]\n\t"                                                                          \
                 "\n\t"                                                                                                \
                 "vld1.32 {q2, q3}, [%[src]" SRC_ALIGN "]!\n\t"                                                        \
                 "vmul.f32 q2, q2, q10\n\t"                                                                            \
                 "vmul.f32 q3, q3, q11\n\t"                                                                            \
                 "\n\t"                                                                                                \
                 "vst1.32 {q0, q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #160]\n\t"                                                                             \
                 "subs %[cnt], %[cnt], #1\n\t"                                                                         \
                 "\n\t"                                                                                                \
                 "vst1.32 {q2, q3}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [table] "+r"(table), [cnt] "+r"(n)                              \
                 : [scale] "r"(&scale), [offset_m16] "r"(-16)                                                          \
                 : "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12")
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
#define AARCH64_NEON_F32_STEREO_MUL_SCALE_AND_BACKWARD_TABLE(SRC_ALIGN, DEST_ALIGN, TBL_ALIGN)                         \
    n >>= 3;                                                                                                           \
    asm volatile("prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pstl1strm, [%[dest], #128]\n\t"                                                                 \
                 "prfm pldl1strm, [%[table], %[offset_m64]]\n\t"                                                       \
                 "sub %[table], %[table], #(4 * 3)\n\t"                                                                \
                 "ld1 {v20.2s}, [%[scale]]\n\t"                                                                        \
                 "ins v20.2d[1], v20.2d[0]\n"                                                                          \
                 "1:\n\t"                                                                                              \
                 "ld1 {v16." TBL_ALIGN "}, [%[table]], %[offset_m16]\n\t"                                              \
                 "rev64 v16.4s, v16.4s\n\t"                                                                            \
                 "zip1 v17.4s, v16.4s, v16.4s\n\t"                                                                     \
                 "zip2 v16.4s, v16.4s, v16.4s\n\t"                                                                     \
                 "fmul v16.4s, v16.4s, v20.4s\n\t"                                                                     \
                 "fmul v17.4s, v17.4s, v20.4s\n\t"                                                                     \
                 "\n\t"                                                                                                \
                 "ld1 {v0." SRC_ALIGN ", v1." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmul v0.4s, v0.4s, v16.4s\n\t"                                                                       \
                 "fmul v1.4s, v1.4s, v17.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "ld1 {v18." TBL_ALIGN "}, [%[table]], %[offset_m16]\n\t"                                              \
                 "rev64 v18.4s, v18.4s\n\t"                                                                            \
                 "zip1 v19.4s, v18.4s, v18.4s\n\t"                                                                     \
                 "zip2 v18.4s, v18.4s, v18.4s\n\t"                                                                     \
                 "fmul v18.4s, v18.4s, v20.4s\n\t"                                                                     \
                 "fmul v19.4s, v19.4s, v20.4s\n\t"                                                                     \
                 "\n\t"                                                                                                \
                 "prfm pldl1strm, [%[table], %[offset_m64]]\n\t"                                                       \
                 "\n\t"                                                                                                \
                 "ld1 {v2." SRC_ALIGN ", v3." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmul v2.4s, v2.4s, v18.4s\n\t"                                                                       \
                 "fmul v3.4s, v3.4s, v19.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v0." DEST_ALIGN ", v1." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pstl1strm, [%[dest], #160]\n\t"                                                                 \
                 "subs %w[cnt], %w[cnt], #1\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v2." DEST_ALIGN ", v3." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [table] "+r"(table), [cnt] "+r"(n)                              \
                 : [scale] "r"(&scale), [offset_m16] "r"(-16LL), [offset_m64] "r"(-64LL)                               \
                 : "memory", "v0", "v1", "v2", "v3", "v16", "v17", "v18", "v19", "v20")
#endif

void neon_f32_stereo_mul_backward_table_and_scale_src_aligned_dest_aligned_table_aligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MUL_SCALE_AND_BACKWARD_TABLE(":128", ":128", ":128");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MUL_SCALE_AND_BACKWARD_TABLE("2d", "2d", "2d");
#endif
}

void neon_f32_stereo_mul_backward_table_and_scale_src_aligned_dest_aligned_table_unaligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MUL_SCALE_AND_BACKWARD_TABLE(":128", ":128", "");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MUL_SCALE_AND_BACKWARD_TABLE("2d", "2d", "4s");
#endif
}

void neon_f32_stereo_mul_backward_table_and_scale_src_aligned_dest_unaligned_table_aligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MUL_SCALE_AND_BACKWARD_TABLE(":128", "", ":128");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MUL_SCALE_AND_BACKWARD_TABLE("2d", "4s", "2d");
#endif
}

void neon_f32_stereo_mul_backward_table_and_scale_src_aligned_dest_unaligned_table_unaligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MUL_SCALE_AND_BACKWARD_TABLE(":128", "", "");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MUL_SCALE_AND_BACKWARD_TABLE("2d", "4s", "4s");
#endif
}

#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
#define AARCH32_NEON_F32_STEREO_MAC_SCALE_AND_BACKWARD_TABLE(SRC_ALIGN, DEST_ALIGN, TBL_ALIGN)                         \
    n >>= 3;                                                                                                           \
    asm volatile("pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #128]\n\t"                                                                             \
                 "pld [%[table], #(-64)]\n\t"                                                                          \
                 "sub %[table], %[table], #(4 * 3)\n\t"                                                                \
                 "vld1.32 {d8}, [%[scale]]\n\t"                                                                        \
                 "vmov.32 d9, d8\n"                                                                                    \
                 "1:\n\t"                                                                                              \
                 "vld1.32 {q8}, [%[table]" TBL_ALIGN "], %[offset_m16]\n\t"                                            \
                 "vrev64.32 q8, q8\n\t"                                                                                \
                 "vmov q9, q8\n\t"                                                                                     \
                 "vzip.32 q9, q8\n\t"                                                                                  \
                 "vmul.f32 q8, q8, q4\n\t"                                                                             \
                 "vmul.f32 q9, q9, q4\n\t"                                                                             \
                 "\n\t"                                                                                                \
                 "vld1.32 {q0, q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "vld1.32 {q12, q13}, [%[src]" SRC_ALIGN "]!\n\t"                                                      \
                 "vmla.f32 q0, q12, q8\n\t"                                                                            \
                 "vmla.f32 q1, q13, q9\n\t"                                                                            \
                 "\n\t"                                                                                                \
                 "vld1.32 {q10}, [%[table]" TBL_ALIGN "], %[offset_m16]\n\t"                                           \
                 "vrev64.32 q10, q10\n\t"                                                                              \
                 "vmov q11, q10\n\t"                                                                                   \
                 "vzip.32 q11, q10\n\t"                                                                                \
                 "vmul.f32 q10, q10, q4\n\t"                                                                           \
                 "vmul.f32 q11, q11, q4\n\t"                                                                           \
                 "\n\t"                                                                                                \
                 "pld [%[table], #(-64)]\n\t"                                                                          \
                 "\n\t"                                                                                                \
                 "vld1.32 {q2, q3}, [%[dest]" DEST_ALIGN "], %[offset_m32]\n\t"                                        \
                 "vld1.32 {q14, q15}, [%[src]" SRC_ALIGN "]!\n\t"                                                      \
                 "vmla.f32 q2, q14, q10\n\t"                                                                           \
                 "vmla.f32 q3, q15, q11\n\t"                                                                           \
                 "\n\t"                                                                                                \
                 "vst1.32 {q0, q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "pld [%[src], #128]\n\t"                                                                              \
                 "pld [%[dest], #160]\n\t"                                                                             \
                 "subs %[cnt], %[cnt], #1\n\t"                                                                         \
                 "\n\t"                                                                                                \
                 "vst1.32 {q2, q3}, [%[dest]" DEST_ALIGN "]!\n\t"                                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [table] "+r"(table), [cnt] "+r"(n)                              \
                 : [scale] "r"(&scale), [offset_m16] "r"(-16), [offset_m32] "r"(-32)                                   \
                 : "memory", "q0", "q1", "q2", "q3", "q4", "q8", "q9", "q10", "q11", "q12", "q12", "q13", "q14",       \
                   "q15")
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
#define AARCH64_NEON_F32_STEREO_MAC_SCALE_AND_BACKWARD_TABLE(SRC_ALIGN, DEST_ALIGN, TBL_ALIGN)                         \
    n >>= 3;                                                                                                           \
    asm volatile("prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pldl1strm, [%[dest], #128]\n\t"                                                                 \
                 "prfm pldl1strm, [%[table], %[offset_m64]]\n\t"                                                       \
                 "sub %[table], %[table], #(4 * 3)\n\t"                                                                \
                 "ld1 {v20.2s}, [%[scale]]\n\t"                                                                        \
                 "ins v20.2d[1], v20.2d[0]\n"                                                                          \
                 "1:\n\t"                                                                                              \
                 "ld1 {v16." TBL_ALIGN "}, [%[table]], %[offset_m16]\n\t"                                              \
                 "rev64 v16.4s, v16.4s\n\t"                                                                            \
                 "zip1 v17.4s, v16.4s, v16.4s\n\t"                                                                     \
                 "zip2 v16.4s, v16.4s, v16.4s\n\t"                                                                     \
                 "fmul v16.4s, v16.4s, v20.4s\n\t"                                                                     \
                 "fmul v17.4s, v17.4s, v20.4s\n\t"                                                                     \
                 "\n\t"                                                                                                \
                 "ld1 {v0." DEST_ALIGN ", v1." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "ld1 {v4." SRC_ALIGN ", v5." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmla v0.4s, v4.4s, v16.4s\n\t"                                                                       \
                 "fmla v1.4s, v5.4s, v17.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "ld1 {v18." TBL_ALIGN "}, [%[table]], %[offset_m16]\n\t"                                              \
                 "rev64 v18.4s, v18.4s\n\t"                                                                            \
                 "zip1 v19.4s, v18.4s, v18.4s\n\t"                                                                     \
                 "zip2 v18.4s, v18.4s, v18.4s\n\t"                                                                     \
                 "fmul v18.4s, v18.4s, v20.4s\n\t"                                                                     \
                 "fmul v19.4s, v19.4s, v20.4s\n\t"                                                                     \
                 "\n\t"                                                                                                \
                 "prfm pldl1strm, [%[table], %[offset_m64]]\n\t"                                                       \
                 "\n\t"                                                                                                \
                 "ld1 {v2." DEST_ALIGN ", v3." DEST_ALIGN "}, [%[dest]], %[offset_m32]\n\t"                            \
                 "ld1 {v6." SRC_ALIGN ", v7." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "fmla v2.4s, v6.4s, v18.4s\n\t"                                                                       \
                 "fmla v3.4s, v7.4s, v19.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v0." DEST_ALIGN ", v1." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "prfm pldl1strm, [%[src], #128]\n\t"                                                                  \
                 "prfm pldl1strm, [%[dest], #160]\n\t"                                                                 \
                 "subs %w[cnt], %w[cnt], #1\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v2." DEST_ALIGN ", v3." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [table] "+r"(table), [cnt] "+r"(n)                              \
                 : [scale] "r"(&scale), [offset_m16] "r"(-16LL), [offset_m32] "r"(-32LL), [offset_m64] "r"(-64LL)      \
                 : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16", "v17", "v18", "v19", "v20")
#endif

void neon_f32_stereo_mac_backward_table_and_scale_src_aligned_dest_aligned_table_aligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MAC_SCALE_AND_BACKWARD_TABLE(":128", ":128", ":128");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MAC_SCALE_AND_BACKWARD_TABLE("2d", "2d", "2d");
#endif
}

void neon_f32_stereo_mac_backward_table_and_scale_src_aligned_dest_aligned_table_unaligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MAC_SCALE_AND_BACKWARD_TABLE(":128", ":128", "");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MAC_SCALE_AND_BACKWARD_TABLE("2d", "2d", "4s");
#endif
}

void neon_f32_stereo_mac_backward_table_and_scale_src_unaligned_dest_aligned_table_aligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MAC_SCALE_AND_BACKWARD_TABLE("", ":128", ":128");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MAC_SCALE_AND_BACKWARD_TABLE("4s", "2d", "2d");
#endif
}

void neon_f32_stereo_mac_backward_table_and_scale_src_unaligned_dest_aligned_table_unaligned(
    f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_stereo_neon_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_stereo_neon_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_stereo_neon_mixer_core_operator::frame_type &scale, int32_t n) CXXPH_NOEXCEPT
{
#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
    AARCH32_NEON_F32_STEREO_MAC_SCALE_AND_BACKWARD_TABLE("", ":128", "");
#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64
    AARCH64_NEON_F32_STEREO_MAC_SCALE_AND_BACKWARD_TABLE("4s", "2d", "4s");
#endif
}

void f32_stereo_neon_mixer_core_operator::mul_scale(frame_type *CXXPH_RESTRICT dest,
                                                    const frame_type *CXXPH_RESTRICT src, const frame_type &scale,
                                                    int n) const CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 8));
    assert(utils::is_aligned(dest, 8));
    assert(n >= 0);

    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int n1 = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 8, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] = src[i] * scale;
        }

        src += n1;
        dest += n1;
        n -= n1;
    }

    const int n2 = (n >> 3) << 3;
    if (n2 > 0) {
        assert(utils::is_aligned(src, 16));

        if (utils::is_aligned(dest, 16)) {
            neon_f32_stereo_mul_scale_src_aligned_dest_aligned(dest, src, scale, n2);
        } else {
            neon_f32_stereo_mul_scale_src_aligned_dest_unaligned(dest, src, scale, n2);
        }

        src += n2;
        dest += n2;
        n -= n2;
    }

    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * scale;
    }
}

void f32_stereo_neon_mixer_core_operator::mac_scale(frame_type *CXXPH_RESTRICT dest,
                                                    const frame_type *CXXPH_RESTRICT src, const frame_type &scale,
                                                    int n) const CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 8));
    assert(utils::is_aligned(dest, 8));
    assert(n >= 0);

    if (!utils::is_aligned(dest, 16)) {
        const uintptr_t dest_ptr = reinterpret_cast<uintptr_t>(dest);
        const int n1 = (std::min)((16 - static_cast<int>(dest_ptr & 0x0f)) / 8, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] += src[i] * scale;
        }

        src += n1;
        dest += n1;
        n -= n1;
    }

    const int n2 = (n >> 3) << 3;
    if (n2 > 0) {
        assert(utils::is_aligned(dest, 16));

        if (utils::is_aligned(src, 16)) {
            neon_mac_scale_src_aligned_dest_aligned(dest, src, scale, n2);
        } else {
            neon_mac_scale_src_unaligned_dest_aligned(dest, src, scale, n2);
        }

        src += n2;
        dest += n2;
        n -= n2;
    }

    for (int i = 0; i < n; ++i) {
        dest[i] += src[i] * scale;
    }
}

void f32_stereo_neon_mixer_core_operator::mul_forward_table_and_scale(frame_type *CXXPH_RESTRICT dest,
                                                                      const frame_type *CXXPH_RESTRICT src,
                                                                      const table_coeff_type *CXXPH_RESTRICT table,
                                                                      const frame_type &scale, int n) const
    CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 8));
    assert(utils::is_aligned(dest, 8));
    assert(n >= 0);

    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int n1 = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 8, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] = src[i] * table[i] * scale;
        }

        src += n1;
        dest += n1;
        table += n1;
        n -= n1;
    }

    const int n2 = (n >> 3) << 3;
    if (n2 > 0) {
        assert(utils::is_aligned(src, 16));

        if (utils::is_aligned(dest, 16) && utils::is_aligned(table, 16)) {
            neon_f32_stereo_mul_forward_table_and_scale_src_aligned_dest_aligned_table_aligned(dest, src, table, scale,
                                                                                               n2);
        } else if (utils::is_aligned(dest, 16) && !utils::is_aligned(table, 16)) {
            neon_f32_stereo_mul_forward_table_and_scale_src_aligned_dest_aligned_table_unaligned(dest, src, table,
                                                                                                 scale, n2);
        } else if (!utils::is_aligned(dest, 16) && utils::is_aligned(table, 16)) {
            neon_f32_stereo_mul_forward_table_and_scale_src_aligned_dest_unaligned_table_aligned(dest, src, table,
                                                                                                 scale, n2);
        } else {
            neon_f32_stereo_mul_forward_table_and_scale_src_aligned_dest_unaligned_table_unaligned(dest, src, table,
                                                                                                   scale, n2);
        }

        src += n2;
        dest += n2;
        table += n2;
        n -= n2;
    }

    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * table[i] * scale;
    }
}

void f32_stereo_neon_mixer_core_operator::mac_forward_table_and_scale(frame_type *CXXPH_RESTRICT dest,
                                                                      const frame_type *CXXPH_RESTRICT src,
                                                                      const table_coeff_type *CXXPH_RESTRICT table,
                                                                      const frame_type &scale, int n) const
    CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 8));
    assert(utils::is_aligned(dest, 8));
    assert(n >= 0);

    if (!utils::is_aligned(dest, 16)) {
        const uintptr_t dest_ptr = reinterpret_cast<uintptr_t>(dest);
        const int n1 = (std::min)((16 - static_cast<int>(dest_ptr & 0x0f)) / 8, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] += src[i] * table[i] * scale;
        }

        src += n1;
        dest += n1;
        table += n1;
        n -= n1;
    }

    const int n2 = (n >> 3) << 3;
    if (n2 > 0) {
        assert(utils::is_aligned(dest, 16));

        if (utils::is_aligned(src, 16) && utils::is_aligned(table, 16)) {
            neon_mac_forward_table_and_scale_src_aligned_dest_aligned_table_aligned(dest, src, table, scale, n2);
        } else if (utils::is_aligned(src, 16) && !utils::is_aligned(table, 16)) {
            neon_mac_forward_table_and_scale_src_aligned_dest_aligned_table_unaligned(dest, src, table, scale, n2);
        } else if (!utils::is_aligned(src, 16) && utils::is_aligned(table, 16)) {
            neon_mac_forward_table_and_scale_src_unaligned_dest_aligned_table_aligned(dest, src, table, scale, n2);
        } else {
            neon_mac_forward_table_and_scale_src_unaligned_dest_aligned_table_unaligned(dest, src, table, scale, n2);
        }

        src += n2;
        dest += n2;
        table += n2;
        n -= n2;
    }

    for (int i = 0; i < n; ++i) {
        dest[i] += src[i] * table[i] * scale;
    }
}

void f32_stereo_neon_mixer_core_operator::mul_backward_table_and_scale(frame_type *CXXPH_RESTRICT dest,
                                                                       const frame_type *CXXPH_RESTRICT src,
                                                                       const table_coeff_type *CXXPH_RESTRICT table,
                                                                       const frame_type &scale, int n) const
    CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 8));
    assert(utils::is_aligned(dest, 8));
    assert(n >= 0);

    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int n1 = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 8, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] = src[i] * (*table) * scale;
            --table;
        }

        src += n1;
        dest += n1;
        // table -= n1;
        n -= n1;
    }

    const int n2 = (n >> 3) << 3;
    if (n2 > 0) {
        assert(utils::is_aligned(src, 16));

        if (utils::is_aligned(dest, 16) && utils::is_aligned((table - 3), 16)) {
            neon_f32_stereo_mul_backward_table_and_scale_src_aligned_dest_aligned_table_aligned(dest, src, table, scale,
                                                                                                n2);
        } else if (utils::is_aligned(dest, 16) && !utils::is_aligned((table - 3), 16)) {
            neon_f32_stereo_mul_backward_table_and_scale_src_aligned_dest_aligned_table_unaligned(dest, src, table,
                                                                                                  scale, n2);
        } else if (!utils::is_aligned(dest, 16) && utils::is_aligned((table - 3), 16)) {
            neon_f32_stereo_mul_backward_table_and_scale_src_aligned_dest_unaligned_table_aligned(dest, src, table,
                                                                                                  scale, n2);
        } else {
            neon_f32_stereo_mul_backward_table_and_scale_src_aligned_dest_unaligned_table_unaligned(dest, src, table,
                                                                                                    scale, n2);
        }

        src += n2;
        dest += n2;
        table -= n2;
        n -= n2;
    }

    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * (*table) * scale;
        --table;
    }
}

void f32_stereo_neon_mixer_core_operator::mac_backward_table_and_scale(frame_type *CXXPH_RESTRICT dest,
                                                                       const frame_type *CXXPH_RESTRICT src,
                                                                       const table_coeff_type *CXXPH_RESTRICT table,
                                                                       const frame_type &scale, int n) const
    CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 8));
    assert(utils::is_aligned(dest, 8));
    assert(n >= 0);

    if (!utils::is_aligned(dest, 16)) {
        const uintptr_t dest_ptr = reinterpret_cast<uintptr_t>(dest);
        const int n1 = (std::min)((16 - static_cast<int>(dest_ptr & 0x0f)) / 8, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] += src[i] * (*table) * scale;
            --table;
        }

        src += n1;
        dest += n1;
        // table -= n1;
        n -= n1;
    }

    const int n2 = (n >> 3) << 3;
    if (n2 > 0) {
        assert(utils::is_aligned(dest, 16));

        if (utils::is_aligned(src, 16) && utils::is_aligned((table - 3), 16)) {
            neon_f32_stereo_mac_backward_table_and_scale_src_aligned_dest_aligned_table_aligned(dest, src, table, scale,
                                                                                                n2);
        } else if (utils::is_aligned(src, 16) && !utils::is_aligned((table - 3), 16)) {
            neon_f32_stereo_mac_backward_table_and_scale_src_aligned_dest_aligned_table_unaligned(dest, src, table,
                                                                                                  scale, n2);
        } else if (!utils::is_aligned(src, 16) && utils::is_aligned((table - 3), 16)) {
            neon_f32_stereo_mac_backward_table_and_scale_src_unaligned_dest_aligned_table_aligned(dest, src, table,
                                                                                                  scale, n2);
        } else {
            neon_f32_stereo_mac_backward_table_and_scale_src_unaligned_dest_aligned_table_unaligned(dest, src, table,
                                                                                                    scale, n2);
        }

        src += n2;
        dest += n2;
        table -= n2;
        n -= n2;
    }

    for (int i = 0; i < n; ++i) {
        dest[i] += src[i] * (*table) * scale;
        --table;
    }
}

} // namespace mixer
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
