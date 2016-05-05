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

#include <cxxdasp/converter/s16_mono_to_f32_stereo_neon_sample_format_converter_core_operator.hpp>

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

static inline void aarch32_perform_s16_mono_to_f32_stereo_neon_unaligned(const int16_t *CXXPH_RESTRICT src,
                                                                         float *CXXPH_RESTRICT dest, int n)
    CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        const float t = src[i] * (1.0f / 32767);
        dest[2 * i + 0] = t;
        dest[2 * i + 1] = t;
    }
}

#define AARCH32_NEON_S16_MONO_TO_F32_STEREO_ASM(SRC_ALIGN, DEST_ALIGN)                                                 \
    asm volatile("pld [%[src], #64]\n\t"                                                                               \
                 "vdup.32 d5, %[scale]\n"                                                                              \
                 "1:\n\t"                                                                                              \
                 "vld1.32 {d0,d1,d2,d3}, [%[src]" SRC_ALIGN "]!\n\t"                                                   \
                 "\n\t"                                                                                                \
                 "vmovl.s16 q4, d0\n\t"                                                                                \
                 "vmovl.s16 q5, d1\n\t"                                                                                \
                 "vmovl.s16 q6, d2\n\t"                                                                                \
                 "vmovl.s16 q7, d3\n\t"                                                                                \
                 "vcvt.f32.s32 q4, q4, #15\n\t"                                                                        \
                 "vcvt.f32.s32 q5, q5, #15\n\t"                                                                        \
                 "vcvt.f32.s32 q6, q6, #15\n\t"                                                                        \
                 "vcvt.f32.s32 q7, q7, #15\n\t"                                                                        \
                 "vmul.f32 q4, q4, d5[0]\n\t"                                                                          \
                 "vmul.f32 q5, q5, d5[0]\n\t"                                                                          \
                 "vmul.f32 q6, q6, d5[0]\n\t"                                                                          \
                 "vmul.f32 q7, q7, d5[0]\n\t"                                                                          \
                 "\n\t"                                                                                                \
                 "pld [%[src], #64]\n\t"                                                                               \
                 "subs %[cnt], %[cnt], #1\n\t"                                                                         \
                 "\n\t"                                                                                                \
                 "vmov.f32 q0, q4\n\t"                                                                                 \
                 "vmov.f32 q1, q5\n\t"                                                                                 \
                 "\n\t"                                                                                                \
                 "vzip.32 q0, q4\n\t"                                                                                  \
                 "vzip.32 q1, q5\n\t"                                                                                  \
                 "vst1.32 {q0}, [%[dest]" DEST_ALIGN "]!\n\t"                                                          \
                 "vst1.32 {q4}, [%[dest]" DEST_ALIGN "]!\n\t"                                                          \
                 "vst1.32 {q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                          \
                 "vst1.32 {q5}, [%[dest]" DEST_ALIGN "]!\n\t"                                                          \
                 "vmov.f32 q0, q6\n\t"                                                                                 \
                 "vmov.f32 q1, q7\n\t"                                                                                 \
                 "vzip.32 q0, q6\n\t"                                                                                  \
                 "vzip.32 q1, q7\n\t"                                                                                  \
                 "vst1.32 {q0}, [%[dest]" DEST_ALIGN "]!\n\t"                                                          \
                 "vst1.32 {q6}, [%[dest]" DEST_ALIGN "]!\n\t"                                                          \
                 "vst1.32 {q1}, [%[dest]" DEST_ALIGN "]!\n\t"                                                          \
                 "vst1.32 {q7}, [%[dest]" DEST_ALIGN "]!\n\t"                                                          \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)                                                   \
                 : [scale] "r"(32768.0f / 32767)                                                                       \
                 : "memory", "q0", "q1", "d5", "q4", "q5", "q6", "q7")

static inline void aarch32_perform_s16_mono_to_f32_stereo_neon_both_aligned(const int16_t *CXXPH_RESTRICT src,
                                                                            float *CXXPH_RESTRICT dest, int n)
    CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 16));
    assert(utils::is_aligned(dest, 16));
    AARCH32_NEON_S16_MONO_TO_F32_STEREO_ASM(":128", ":128");
}

static inline void aarch32_perform_s16_mono_to_f32_stereo_neon_src_aligned(const int16_t *CXXPH_RESTRICT src,
                                                                           float *CXXPH_RESTRICT dest, int n)
    CXXPH_NOEXCEPT
{
    assert(utils::is_aligned(src, 16));
    AARCH32_NEON_S16_MONO_TO_F32_STEREO_ASM(":128", "");
}

void s16_mono_to_f32_stereo_neon_sample_format_converter_core_operator::perform_impl(const int16_t *CXXPH_RESTRICT src,
                                                                                     float *CXXPH_RESTRICT dest, int n)
    CXXPH_NOEXCEPT
{

    assert(utils::is_aligned(src, 2));
    assert(utils::is_aligned(dest, 4));
    assert(n >= 0);

    const float norm_coeff = (1.0f / std::numeric_limits<int16_t>::max());

    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int nx = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 2, n);

        aarch32_perform_s16_mono_to_f32_stereo_neon_unaligned(src, dest, nx);

        src += nx;
        dest += 2 * nx;
        n -= nx;
    }

    if (n >= 16) {
        if (utils::is_aligned(dest, 16)) {
            aarch32_perform_s16_mono_to_f32_stereo_neon_both_aligned(src, dest, (n / 16));
        } else {
            aarch32_perform_s16_mono_to_f32_stereo_neon_src_aligned(src, dest, (n / 16));
        }

        src += (n / 16) * 16;
        dest += (n / 16) * 16 * 2;
        n &= 0xf;
    }

    if (n > 0) {
        aarch32_perform_s16_mono_to_f32_stereo_neon_unaligned(src, dest, n);
    }
}

#elif(CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
// for AArch64

static inline void aarch64_perform_s16_mono_to_f32_stereo_neon_unaligned(const int16_t *CXXPH_RESTRICT src,
                                                                         float *CXXPH_RESTRICT dest, int n)
{
    for (int i = 0; i < n; ++i) {
        const float t = src[i] * (1.0f / 32767);
        dest[2 * i + 0] = t;
        dest[2 * i + 1] = t;
    }
}

#define AARCH64_NEON_S16_MONO_TO_F32_STEREO_ASM(SRC_ALIGN, DEST_ALIGN)                                                 \
    asm volatile("prfm pldl1strm, [%[src], #32]\n\t"                                                                   \
                 "prfm pstl1strm, [%[dest], #128]\n\t"                                                                 \
                 "dup v6.4s, %w[scale]\n"                                                                              \
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
                 "fmul v0.4s, v0.4s, v6.4s\n\t"                                                                        \
                 "fmul v1.4s, v1.4s, v6.4s\n\t"                                                                        \
                 "fmul v2.4s, v2.4s, v6.4s\n\t"                                                                        \
                 "fmul v3.4s, v3.4s, v6.4s\n\t"                                                                        \
                 "\n\t"                                                                                                \
                 "subs %w[cnt], %w[cnt], #1\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "zip1 v16.4s, v0.4s, v0.4s\n\t"                                                                       \
                 "zip2 v17.4s, v0.4s, v0.4s\n\t"                                                                       \
                 "zip1 v18.4s, v1.4s, v1.4s\n\t"                                                                       \
                 "zip2 v19.4s, v1.4s, v1.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v16." DEST_ALIGN ", v17." DEST_ALIGN ", "                                                       \
                 "v18." DEST_ALIGN ", v19." DEST_ALIGN "}, [%[dest]], #64\n\t"                                         \
                 "\n\t"                                                                                                \
                 "zip1 v16.4s, v2.4s, v2.4s\n\t"                                                                       \
                 "zip2 v17.4s, v2.4s, v2.4s\n\t"                                                                       \
                 "zip1 v18.4s, v3.4s, v3.4s\n\t"                                                                       \
                 "zip2 v19.4s, v3.4s, v3.4s\n\t"                                                                       \
                 "\n\t"                                                                                                \
                 "st1 {v16." DEST_ALIGN ", v17." DEST_ALIGN ", "                                                       \
                 "v18." DEST_ALIGN ", v19." DEST_ALIGN "}, [%[dest]], #64\n\t"                                         \
                 "prfm pstl1strm, [%[dest], #128]\n\t"                                                                 \
                 "\n\t"                                                                                                \
                 "bne 1b\n\t"                                                                                          \
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)                                                   \
                 : [scale] "r"(32768.0f / 32767)                                                                       \
                 : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v16", "v17", "v18", "v19")

static inline void aarch64_perform_s16_mono_to_f32_stereo_neon_both_aligned(const int16_t *CXXPH_RESTRICT src,
                                                                            float *CXXPH_RESTRICT dest, int32_t n)
{
    assert(utils::is_aligned(src, 16));
    assert(utils::is_aligned(dest, 16));
    AARCH64_NEON_S16_MONO_TO_F32_STEREO_ASM("2d", "2d"); // 16 bytes - 16 bytes
}

static inline void aarch64_perform_s16_mono_to_f32_stereo_neon_src_aligned(const int16_t *CXXPH_RESTRICT src,
                                                                           float *CXXPH_RESTRICT dest, int32_t n)
{
    assert(utils::is_aligned(src, 16));
    AARCH64_NEON_S16_MONO_TO_F32_STEREO_ASM("2d", "4s"); // 16 bytes - 4 bytes
}

void s16_mono_to_f32_stereo_neon_sample_format_converter_core_operator::perform_impl(const int16_t *CXXPH_RESTRICT src,
                                                                                     float *CXXPH_RESTRICT dest, int n)
    CXXPH_NOEXCEPT
{

    assert(utils::is_aligned(src, 2));
    assert(utils::is_aligned(dest, 4));
    assert(n >= 0);

    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int nx = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 2, n);

        aarch64_perform_s16_mono_to_f32_stereo_neon_unaligned(src, dest, nx);

        src += nx;
        dest += 2 * nx;
        n -= nx;
    }

    if (n >= 16) {
        if (utils::is_aligned(dest, 16)) {
            aarch64_perform_s16_mono_to_f32_stereo_neon_both_aligned(src, dest, (n / 16));
        } else {
            aarch64_perform_s16_mono_to_f32_stereo_neon_src_aligned(src, dest, (n / 16));
        }

        src += (n / 16) * 16;
        dest += (n / 16) * 16 * 2;
        n &= 0xf;
    }

    if (n > 0) {
        aarch64_perform_s16_mono_to_f32_stereo_neon_unaligned(src, dest, n);
    }
}

#endif

} // namespace converter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
