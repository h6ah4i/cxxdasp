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

#include <cxxdasp/converter/s16_to_f32_stereo_neon_fast_sample_format_converter_core_operator.hpp>

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

static inline void aarch32_perform_s16_to_f32_neon_unaligned(const int16_t *CXXPH_RESTRICT src,
                                                             float *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{

    asm volatile("1:\n\t"
                 "ldrh r5, [%[src]], #2\n\t"
                 "\n\t"
                 "vmov s0, r5\n\t"
                 "vcvt.f32.s16 s0, s0, #15\n\t"
                 "\n\t"
                 "subs %[cnt], %[cnt], #1\n\t"
                 "\n\t"
                 "vstmia %[dest]!, {s0}\n\t"
                 "\n\t"
                 "bne 1b\n\t"
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)
                 :
                 : "memory", "r5", "q0", "s4");
}

#define AARCH32_NEON_S16_TO_F32_ASM(SRC_ALIGN, DEST_ALIGN)                                                             \
    asm volatile("pld [%[src], #64]\n\t"                                                                               \
                 "1:\n\t"                                                                                              \
                 "vld1.32 {d0,d1,d2,d3}, [%[src]" SRC_ALIGN "]!\n\t"                                                   \
                 "\n\t"                                                                                                \
                 "vmovl.s16 q2, d0\n\t"                                                                                \
                 "vmovl.s16 q3, d1\n\t"                                                                                \
                 "vmovl.s16 q0, d2\n\t"                                                                                \
                 "vmovl.s16 q1, d3\n\t"                                                                                \
                 "vcvt.f32.s32 q2, q2, #15\n\t"                                                                        \
                 "vcvt.f32.s32 q3, q3, #15\n\t"                                                                        \
                 "vcvt.f32.s32 q0, q0, #15\n\t"                                                                        \
                 "vcvt.f32.s32 q1, q1, #15\n\t"                                                                        \
                 "\n\t"                                                                                                \
                 "pld [%[src], #64]\n\t"                                                                               \
                 "subs %[cnt], %[cnt], #1\n\t"                                                                         \
                 "\n\t"                                                                                                \
                 "vst1.32 {q2,q3}, [%[dest]" DEST_ALIGN "]!\n\t"                                                       \
                 "vst1.32 {q0,q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                       \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)                                                   \
                 :                                                                                                     \
                 : "memory", "q0", "q1", "q2", "q3")

static inline void aarch32_perform_s16_to_f32_neon_both_aligned_x16(const int16_t *CXXPH_RESTRICT src,
                                                                    float *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 16));
    assert(utils::is_aligned(dest, 16));
    AARCH32_NEON_S16_TO_F32_ASM(":128", ":128");
}

static inline void aarch32_perform_s16_to_f32_neon_src_aligned_x16(const int16_t *CXXPH_RESTRICT src,
                                                                   float *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 16));
    AARCH32_NEON_S16_TO_F32_ASM(":128", "");
}

void s16_to_f32_stereo_neon_fast_sample_format_converter_core_operator::perform_impl(const int16_t *CXXPH_RESTRICT src,
                                                                                     float *CXXPH_RESTRICT dest, int n)
    CXXPH_NOEXCEPT
{

    assert(utils::is_aligned(src, 2));
    assert(utils::is_aligned(dest, 4));
    assert(n >= 0);

    n *= 2;

    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int nx = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 2, n);

        aarch32_perform_s16_to_f32_neon_unaligned(src, dest, nx);

        src += nx;
        dest += nx;
        n -= nx;
    }

    if (n >= 16) {
        if (utils::is_aligned(dest, 16)) {
            aarch32_perform_s16_to_f32_neon_both_aligned_x16(src, dest, (n / 16));
        } else {
            aarch32_perform_s16_to_f32_neon_src_aligned_x16(src, dest, (n / 16));
        }

        src += (n / 16) * 16;
        dest += (n / 16) * 16;
        n &= 0xf;
    }

    if (n > 0) {
        aarch32_perform_s16_to_f32_neon_unaligned(src, dest, n);
    }
}

#elif(CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
// for AArch64

static inline void aarch64_perform_s16_to_f32_neon_unaligned(const int16_t *CXXPH_RESTRICT src,
                                                             float *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{

    asm volatile("prfm pldl1strm, [%[src], #0]\n\t"
                 "prfm pstl1strm, [%[dest], #0]\n\t"
                 "1:\n\t"
                 "eor v0.8b, v0.8b, v0.8b\n\t"
                 "ldr h0, [%[src]], #2\n\t"
                 "\n\t"
                 "sxtl v0.4s, v0.4h\n\t"
                 "scvtf s0, s0, #15\n\t"
                 "\n\t"
                 "subs %[cnt], %[cnt], #1\n\t"
                 "\n\t"
                 "str s0, [%[dest]], #4\n\t"
                 "\n\t"
                 "bne 1b\n\t"
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)
                 :
                 : "memory", "s0");
}

#define AARCH64_NEON_S16_TO_F32_ASM(SRC_ALIGN, DEST_ALIGN)                                                             \
    asm volatile("prfm pldl1strm, [%[src], #32]\n\t"                                                                   \
                 "prfm pstl1strm, [%[dest], #64]\n\t"                                                                  \
                 "1:\n\t"                                                                                              \
                 "ld1 {v4." SRC_ALIGN ", v5." SRC_ALIGN "}, [%[src]], #32\n\t"                                         \
                 "prfm pldl1strm, [%[src], #32]\n\t"                                                                   \
                 "\n\t"                                                                                                \
                 "sxtl v0.4s, v4.4h\n\t"                                                                               \
                 "sxtl2 v1.4s, v4.8h\n\t"                                                                              \
                 "sxtl v2.4s, v5.4h\n\t"                                                                               \
                 "sxtl2 v3.4s, v5.8h\n\t"                                                                              \
                 "\n\t"                                                                                                \
                 "scvtf v0.4s, v0.4s, #15\n\t"                                                                         \
                 "scvtf v1.4s, v1.4s, #15\n\t"                                                                         \
                 "scvtf v2.4s, v2.4s, #15\n\t"                                                                         \
                 "scvtf v3.4s, v3.4s, #15\n\t"                                                                         \
                 "\n\t"                                                                                                \
                 "subs %[cnt], %[cnt], #1\n\t"                                                                         \
                 "\n\t"                                                                                                \
                 "st1 {v0." DEST_ALIGN ", v1." DEST_ALIGN ", "                                                         \
                 "v2." DEST_ALIGN ", v3." DEST_ALIGN "}, [%[dest]], #64\n\t"                                           \
                 "prfm pstl1strm, [%[dest], #64]\n\t"                                                                  \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)                                                   \
                 :                                                                                                     \
                 : "memory", "v0", "v1", "v2", "v3", "v4", "v5")

static inline void aarch64_perform_s16_to_f32_neon_both_aligned_x16(const int16_t *CXXPH_RESTRICT src,
                                                                    float *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 16));
    assert(utils::is_aligned(dest, 16));
    AARCH64_NEON_S16_TO_F32_ASM("2d", "2d"); // 16 bytes - 16 bytes
}

static inline void aarch64_perform_s16_to_f32_neon_src_aligned_x16(const int16_t *CXXPH_RESTRICT src,
                                                                   float *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 16));
    AARCH64_NEON_S16_TO_F32_ASM("2d", "4s"); // 16 bytes - 4 bytes
}

void s16_to_f32_stereo_neon_fast_sample_format_converter_core_operator::perform_impl(const int16_t *CXXPH_RESTRICT src,
                                                                                     float *CXXPH_RESTRICT dest, int n)
    CXXPH_NOEXCEPT
{

    assert(utils::is_aligned(src, 2));
    assert(utils::is_aligned(dest, 4));
    assert(n >= 0);

    n *= 2;

    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int nx = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 2, n);

        aarch64_perform_s16_to_f32_neon_unaligned(src, dest, nx);

        src += nx;
        dest += nx;
        n -= nx;
    }

    if (n >= 16) {
        if (utils::is_aligned(dest, 16)) {
            aarch64_perform_s16_to_f32_neon_both_aligned_x16(src, dest, (n / 16));
        } else {
            aarch64_perform_s16_to_f32_neon_src_aligned_x16(src, dest, (n / 16));
        }

        src += (n / 16) * 16;
        dest += (n / 16) * 16;
        n &= 0xf;
    }

    if (n > 0) {
        aarch64_perform_s16_to_f32_neon_unaligned(src, dest, n);
    }
}

#endif

} // namespace converter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
