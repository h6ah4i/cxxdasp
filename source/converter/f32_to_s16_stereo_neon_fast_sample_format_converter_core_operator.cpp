//
//    Copyright (C) 2014 Haruki Hasegawa
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#include <cxxdasp/converter/f32_to_s16_stereo_neon_fast_sample_format_converter_core_operator.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

#include <limits>
#include <cassert>
#include <algorithm>

#include <cxxporthelper/arm_neon.hpp>

#include <cxxdasp/utils/utils.hpp>

namespace cxxdasp {
namespace converter {

#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
// for AArch32

static inline void aarch32_perform_f32_to_s16_neon_unaligned(const float *CXXPH_RESTRICT src,
                                                             int16_t *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{

    asm volatile("1:\n\t"
                 "vldmia %[src]!, {s0}\n\t"
                 "\n\t"
                 "vcvt.s32.f32 s0, s0, #16\n\t"
                 "vqshrn.s32 d0, q0, #1\n\t"
                 "\n\t"
                 "subs %[cnt], %[cnt], #1\n\t"
                 "\n\t"
                 "vmov.s16 r5, s0\n\t"
                 "strh r5, [%[dest]], #2\n\t"
                 "\n\t"
                 "bne 1b\n\t"
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)
                 :
                 : "memory", "r5", "q0");
}

#define AARCH32_NEON_S16_TO_F32_ASM(SRC_ALIGN, DEST_ALIGN)                                                             \
    asm volatile("pld [%[src], #64]\n\t"                                                                               \
                 "1:\n\t"                                                                                              \
                 "vld1.32 {q0,q1}, [%[src]" SRC_ALIGN "]!\n\t"                                                         \
                 "\n\t"                                                                                                \
                 "vcvt.s32.f32 q0, q0, #16\n\t"                                                                        \
                 "vcvt.s32.f32 q1, q1, #16\n\t"                                                                        \
                 "\n\t"                                                                                                \
                 "vld1.32 {q2,q3}, [%[src]" SRC_ALIGN "]!\n\t"                                                         \
                 "\n\t"                                                                                                \
                 "vcvt.s32.f32 q2, q2, #16\n\t"                                                                        \
                 "vcvt.s32.f32 q3, q3, #16\n\t"                                                                        \
                 "vqshrn.s32 d0, q0, #1\n\t"                                                                           \
                 "vqshrn.s32 d1, q1, #1\n\t"                                                                           \
                 "vqshrn.s32 d2, q2, #1\n\t"                                                                           \
                 "vqshrn.s32 d3, q3, #1\n\t"                                                                           \
                 "\n\t"                                                                                                \
                 "pld [%[src], #64]\n\t"                                                                               \
                 "subs %[cnt], %[cnt], #1\n\t"                                                                         \
                 "\n\t"                                                                                                \
                 "vst1.32 {d0,d1,d2,d3}, [%[dest]" DEST_ALIGN "]!\n\t"                                                 \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)                                                   \
                 :                                                                                                     \
                 : "memory", "q0", "q1", "q2", "q3", "q4")

static inline void aarch32_perform_f32_to_s16_neon_both_aligned_x16(const float *CXXPH_RESTRICT src,
                                                                    int16_t *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 16));
    assert(utils::is_aligned(dest, 16));
    AARCH32_NEON_S16_TO_F32_ASM(":128", ":128");
}

static inline void aarch32_perform_f32_to_s16_neon_src_aligned_x16(const float *CXXPH_RESTRICT src,
                                                                   int16_t *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 16));
    AARCH32_NEON_S16_TO_F32_ASM(":128", "");
}

void f32_to_s16_stereo_neon_fast_sample_format_converter_core_operator::perform_impl(const float *CXXPH_RESTRICT src,
                                                                                     int16_t *CXXPH_RESTRICT dest,
                                                                                     int n) CXXPH_NOEXCEPT
{

    assert(utils::is_aligned(src, 4));
    assert(utils::is_aligned(dest, 2));
    assert(n >= 0);

    n *= 2;

    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int nx = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 4, n);

        aarch32_perform_f32_to_s16_neon_unaligned(src, dest, nx);

        src += nx;
        dest += nx;
        n -= nx;
    }

    if (n >= 16) {
        const int nx = n / 16;

        if (utils::is_aligned(dest, 16)) {
            aarch32_perform_f32_to_s16_neon_both_aligned_x16(src, dest, nx);
        } else {
            aarch32_perform_f32_to_s16_neon_src_aligned_x16(src, dest, nx);
        }

        src += nx * 16;
        dest += nx * 16;
        n &= 0xf;
    }

    if (n > 0) {
        aarch32_perform_f32_to_s16_neon_unaligned(src, dest, n);
    }
}

#elif(CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
// for AArch64

static inline void aarch64_perform_f32_to_s16_neon_unaligned(const float *CXXPH_RESTRICT src,
                                                             int16_t *CXXPH_RESTRICT dest, int32_t n) CXXPH_NOEXCEPT
{

    asm volatile("prfm pldl1strm, [%[src], #0]\n\t"
                 "prfm pstl1strm, [%[dest], #0]\n\t"
                 "1:\n\t"
                 "ldr s0, [%[src]], #4\n\t"
                 "\n\t"
                 "fcvtzs s0, s0, #16\n\t"
                 "sqrshrn h0, s0, #1\n\t"
                 "\n\t"
                 "subs %w[cnt], %w[cnt], #1\n\t"
                 "\n\t"
                 "str h0, [%[dest]], #2\n\t"
                 "\n\t"
                 "bne 1b\n\t"
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)
                 :
                 : "memory", "s0");
}

#define AARCH64_NEON_S16_TO_F32_ASM_X16(SRC_ALIGN, DEST_ALIGN)                                                         \
    asm volatile("prfm pldl1strm, [%[src], #64]\n\t"                                                                   \
                 "prfm pstl1strm, [%[dest], #32]\n\t"                                                                  \
                 "1:\n\t"                                                                                              \
                 "ld1 {v0." SRC_ALIGN ", v1." SRC_ALIGN ", "                                                           \
                 "v2." SRC_ALIGN ", v3." SRC_ALIGN "}, [%[src]], #64\n\t"                                              \
                 "prfm pldl1strm, [%[src], #64]\n\t"                                                                   \
                 "\n\t"                                                                                                \
                 "fcvtzs v0.4s, v0.4s, #16\n\t"                                                                        \
                 "fcvtzs v1.4s, v1.4s, #16\n\t"                                                                        \
                 "fcvtzs v2.4s, v2.4s, #16\n\t"                                                                        \
                 "fcvtzs v3.4s, v3.4s, #16\n\t"                                                                        \
                 "\n\t"                                                                                                \
                 "sqrshrn v4.4h, v0.4s, #1\n\t"                                                                        \
                 "sqrshrn2 v4.8h, v1.4s, #1\n\t"                                                                       \
                 "sqrshrn v5.4h, v2.4s, #1\n\t"                                                                        \
                 "sqrshrn2 v5.8h, v3.4s, #1\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "subs %w[cnt], %w[cnt], #1\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v4." DEST_ALIGN ", v5." DEST_ALIGN "}, [%[dest]], #32\n\t"                                      \
                 "prfm pstl1strm, [%[dest], #32]\n\t"                                                                  \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)                                                   \
                 :                                                                                                     \
                 : "memory", "v0", "v1", "v2", "v3", "v4", "v5")

static inline void aarch64_perform_f32_to_s16_neon_both_aligned_x16(const float *CXXPH_RESTRICT src,
                                                                    int16_t *CXXPH_RESTRICT dest, int32_t n) CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 16));
    assert(utils::is_aligned(dest, 16));

    AARCH64_NEON_S16_TO_F32_ASM_X16("2d", "2d"); // 16 bytes - 16 bytes
}

static inline void aarch64_perform_f32_to_s16_neon_src_aligned_x16(const float *CXXPH_RESTRICT src,
                                                                   int16_t *CXXPH_RESTRICT dest, int32_t n) CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 16));

    AARCH64_NEON_S16_TO_F32_ASM_X16("2d", "8h"); // 16 bytes - 2 bytes
}

void f32_to_s16_stereo_neon_fast_sample_format_converter_core_operator::perform_impl(const float *CXXPH_RESTRICT src,
                                                                                     int16_t *CXXPH_RESTRICT dest,
                                                                                     int n) CXXPH_NOEXCEPT
{

    assert(utils::is_aligned(src, 4));
    assert(utils::is_aligned(dest, 2));
    assert(n >= 0);

    n *= 2;

    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int nx = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 4, n);

        aarch64_perform_f32_to_s16_neon_unaligned(src, dest, nx);

        src += nx;
        dest += nx;
        n -= nx;
    }

    if (n >= 16) {
        const int nx = n / 16;

        if (utils::is_aligned(dest, 16)) {
            aarch64_perform_f32_to_s16_neon_both_aligned_x16(src, dest, nx);
        } else {
            aarch64_perform_f32_to_s16_neon_src_aligned_x16(src, dest, nx);
        }

        src += nx * 16;
        dest += nx * 16;
        n &= 0xf;
    }

    if (n > 0) {
        aarch64_perform_f32_to_s16_neon_unaligned(src, dest, n);
    }
}

#endif

} // namespace converter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
