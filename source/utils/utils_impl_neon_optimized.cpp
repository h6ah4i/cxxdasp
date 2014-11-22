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

#include <cxxdasp/utils/impl/utils_impl_neon_optimized.hpp>

#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)

#include <cxxporthelper/arm_neon.hpp>
#include <cxxporthelper/platform_info.hpp>

#include <cxxdasp/utils/impl/utils_impl_core.hpp>
#include <cxxdasp/utils/impl/utils_impl_general.hpp>
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON
#include <cxxdasp/utils/neon_debug_utils.hpp>
#endif

namespace cxxdasp {
namespace utils {
namespace impl_neon {

//
// internal functions
//

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_multiply_scaler_aligned(float *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT
{
    int n1 = (n >> 4);
    int n2 = (n & 0xf);

    if (n1 > 0) {
        float *CXXPH_RESTRICT src = src_dest;

        asm volatile("vdup.32 d8, %[scale]\n"
                     "1:\n\t"
                     "vld1.32 {q0,q1}, [%[src]:128]!\n\t"
                     "vmul.f32 q0, q0, d8[0]\n\t"
                     "vmul.f32 q1, q1, d8[0]\n\t"
                     "\n\t"
                     "vld1.32 {q2,q3}, [%[src]:128]!\n\t"
                     "vmul.f32 q2, q2, d8[0]\n\t"
                     "vmul.f32 q3, q3, d8[0]\n\t"
                     "\n\t"
                     "vst1.32 {q0,q1}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "pld [%[src], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "vst1.32 {q2,q3}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(src_dest), [cnt] "+r"(n1)
                     : [scale] "r"(x)
                     : "memory", "q0", "q1", "q2", "q3", "d8");
    }

    for (int i = 0; i < n2; ++i) {
        src_dest[i] *= x;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_scaler_aligned(float *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT
{
    int n1 = (n >> 5);
    int n2 = (n & 0x1f);

    if (n1 > 0) {
        float *CXXPH_RESTRICT src = src_dest;

        asm volatile("prfm pldl1strm, [%[src], #128]\n\t"
                     "dup v16.4s, %w[scale]\n"
                     "1:\n\t"
                     "ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[src]], #64\n\t"
                     "fmul v0.4s, v0.4s, v16.4s\n\t"
                     "fmul v1.4s, v1.4s, v16.4s\n\t"
                     "fmul v2.4s, v2.4s, v16.4s\n\t"
                     "fmul v3.4s, v3.4s, v16.4s\n\t"
                     "\n\t"
                     "ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[src]], #64\n\t"
                     "fmul v4.4s, v4.4s, v16.4s\n\t"
                     "fmul v5.4s, v5.4s, v16.4s\n\t"
                     "fmul v6.4s, v6.4s, v16.4s\n\t"
                     "fmul v7.4s, v7.4s, v16.4s\n\t"
                     "\n\t"
                     "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(src_dest), [cnt] "+r"(n1)
                     : [scale] "r"(x)
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16");
    }

    for (int i = 0; i < n2; ++i) {
        src_dest[i] *= x;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_multiply_scaler_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, float x,
                                                 int n) CXXPH_NOEXCEPT
{
    int n1 = (n >> 4);
    int n2 = (n & 0xf);

    if (n1 > 0) {
        asm volatile("vdup.32 d8, %[scale]\n"
                     "1:\n\t"
                     "vld1.32 {q0,q1}, [%[src]:128]!\n\t"
                     "vmul.f32 q0, q0, d8[0]\n\t"
                     "vmul.f32 q1, q1, d8[0]\n\t"
                     "\n\t"
                     "vld1.32 {q2,q3}, [%[src]:128]!\n\t"
                     "vmul.f32 q2, q2, d8[0]\n\t"
                     "vmul.f32 q3, q3, d8[0]\n\t"
                     "\n\t"
                     "vst1.32 {q0,q1}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "pld [%[src], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "vst1.32 {q2,q3}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n1)
                     : [scale] "r"(x)
                     : "memory", "q0", "q1", "q2", "q3", "d8");
    }

    for (int i = 0; i < n2; ++i) {
        dest[i] = src[i] * x;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_scaler_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, float x,
                                                 int n) CXXPH_NOEXCEPT
{
    int n1 = (n >> 5);
    int n2 = (n & 0x1f);

    if (n1 > 0) {
        asm volatile("prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "dup v16.4s, %w[scale]\n"
                     "1:\n\t"
                     "ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[src]], #64\n\t"
                     "fmul v0.4s, v0.4s, v16.4s\n\t"
                     "fmul v1.4s, v1.4s, v16.4s\n\t"
                     "fmul v2.4s, v2.4s, v16.4s\n\t"
                     "fmul v3.4s, v3.4s, v16.4s\n\t"
                     "\n\t"
                     "ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[src]], #64\n\t"
                     "fmul v4.4s, v4.4s, v16.4s\n\t"
                     "fmul v5.4s, v5.4s, v16.4s\n\t"
                     "fmul v6.4s, v6.4s, v16.4s\n\t"
                     "fmul v7.4s, v7.4s, v16.4s\n\t"
                     "\n\t"
                     "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[dest]], #64\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n1)
                     : [scale] "r"(x)
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16");
    }

    for (int i = 0; i < n2; ++i) {
        dest[i] = src[i] * x;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_scaler_aligned(double *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT
{
    int n1 = (n >> 4);
    int n2 = (n & 0xf);

    if (n1 > 0) {
        double *CXXPH_RESTRICT src = src_dest;

        asm volatile("prfm pldl1strm, [%[src], #128]\n\t"
                     "dup v16.2d, %x[scale]\n"
                     "1:\n\t"
                     "ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[src]], #64\n\t"
                     "fmul v0.2d, v0.2d, v16.2d\n\t"
                     "fmul v1.2d, v1.2d, v16.2d\n\t"
                     "fmul v2.2d, v2.2d, v16.2d\n\t"
                     "fmul v3.2d, v3.2d, v16.2d\n\t"
                     "\n\t"
                     "ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[src]], #64\n\t"
                     "fmul v4.2d, v4.2d, v16.2d\n\t"
                     "fmul v5.2d, v5.2d, v16.2d\n\t"
                     "fmul v6.2d, v6.2d, v16.2d\n\t"
                     "fmul v7.2d, v7.2d, v16.2d\n\t"
                     "\n\t"
                     "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(src_dest), [cnt] "+r"(n1)
                     : [scale] "r"(x)
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16");
    }

    for (int i = 0; i < n2; ++i) {
        src_dest[i] *= x;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_scaler_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src,
                                                 double x, int n) CXXPH_NOEXCEPT
{
    int n1 = (n >> 4);
    int n2 = (n & 0xf);

    if (n1 > 0) {
        asm volatile("prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "dup v16.2d, %x[scale]\n"
                     "1:\n\t"
                     "ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[src]], #64\n\t"
                     "fmul v0.2d, v0.2d, v16.2d\n\t"
                     "fmul v1.2d, v1.2d, v16.2d\n\t"
                     "fmul v2.2d, v2.2d, v16.2d\n\t"
                     "fmul v3.2d, v3.2d, v16.2d\n\t"
                     "\n\t"
                     "ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[src]], #64\n\t"
                     "fmul v4.2d, v4.2d, v16.2d\n\t"
                     "fmul v5.2d, v5.2d, v16.2d\n\t"
                     "fmul v6.2d, v6.2d, v16.2d\n\t"
                     "fmul v7.2d, v7.2d, v16.2d\n\t"
                     "\n\t"
                     "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[dest]], #64\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n1)
                     : [scale] "r"(x)
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16");
    }

    for (int i = 0; i < n2; ++i) {
        dest[i] = src[i] * x;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_multiply_aligned(float *CXXPH_RESTRICT src_dest, const float *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    int n1 = (n >> 4);
    int n2 = (n & 0xf);

    if (n1 > 0) {
        const float *CXXPH_RESTRICT src = src_dest;
        asm volatile("1:\n\t"
                     "vld1.32 {q0,q1}, [%[src]:128]!\n\t"
                     "vld1.32 {q8,q9}, [%[scale]:128]!\n\t"
                     "vmul.f32 q0, q0, q8\n\t"
                     "vmul.f32 q1, q1, q9\n\t"
                     "\n\t"
                     "vld1.32 {q2,q3}, [%[src]:128]!\n\t"
                     "vld1.32 {q8,q9}, [%[scale]:128]!\n\t"
                     "vmul.f32 q2, q2, q8\n\t"
                     "vmul.f32 q3, q3, q9\n\t"
                     "\n\t"
                     "vst1.32 {q0,q1}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "pld [%[src], #128]\n\t"
                     "pld [%[scale], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "vst1.32 {q2,q3}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(src_dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "q0", "q1", "q2", "q3", "q8", "q9");
    }

    for (int i = 0; i < n2; ++i) {
        src_dest[i] *= x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_aligned(float *CXXPH_RESTRICT src_dest, const float *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{

    int n1 = (n >> 5);
    int n2 = (n & 0x1f);

    if (n1 > 0) {
        const float *CXXPH_RESTRICT src = src_dest;
        asm volatile("prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pldl1strm, [%[scale], #128]\n\t"
                     "1:\n\t"
                     "ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[src]], #64\n\t"
                     "ld1 {v16.2d, v17.2d, v18.2d, v19.2d}, [%[scale]], #64\n\t"
                     "fmul v0.4s, v0.4s, v16.4s\n\t"
                     "fmul v1.4s, v1.4s, v17.4s\n\t"
                     "fmul v2.4s, v2.4s, v18.4s\n\t"
                     "fmul v3.4s, v3.4s, v19.4s\n\t"
                     "\n\t"
                     "ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[src]], #64\n\t"
                     "ld1 {v16.2d, v17.2d, v18.2d, v19.2d}, [%[scale]], #64\n\t"
                     "fmul v4.4s, v4.4s, v16.4s\n\t"
                     "fmul v5.4s, v5.4s, v17.4s\n\t"
                     "fmul v6.4s, v6.4s, v18.4s\n\t"
                     "fmul v7.4s, v7.4s, v19.4s\n\t"
                     "\n\t"
                     "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pldl1strm, [%[scale], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(src_dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16", "v17", "v18", "v19");
    }

    for (int i = 0; i < n2; ++i) {
        src_dest[i] *= x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_multiply_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src,
                                          const float *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    int n1 = (n >> 4);
    int n2 = (n & 0xf);

    if (n1 > 0) {
        asm volatile("1:\n\t"
                     "vld1.32 {q0,q1}, [%[src]:128]!\n\t"
                     "vld1.32 {q8,q9}, [%[scale]:128]!\n\t"
                     "vmul.f32 q0, q0, q8\n\t"
                     "vmul.f32 q1, q1, q9\n\t"
                     "\n\t"
                     "vld1.32 {q2,q3}, [%[src]:128]!\n\t"
                     "vld1.32 {q8,q9}, [%[scale]:128]!\n\t"
                     "vmul.f32 q2, q2, q8\n\t"
                     "vmul.f32 q3, q3, q9\n\t"
                     "\n\t"
                     "vst1.32 {q0,q1}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "pld [%[src], #128]\n\t"
                     "pld [%[scale], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "vst1.32 {q2,q3}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "q0", "q1", "q2", "q3", "q8", "q9");
    }

    for (int i = 0; i < n2; ++i) {
        dest[i] = src[i] * x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src,
                                          const float *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{

    int n1 = (n >> 5);
    int n2 = (n & 0x1f);

    if (n1 > 0) {
        asm volatile("prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pldl1strm, [%[scale], #128]\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "1:\n\t"
                     "ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[src]], #64\n\t"
                     "ld1 {v16.2d, v17.2d, v18.2d, v19.2d}, [%[scale]], #64\n\t"
                     "fmul v0.4s, v0.4s, v16.4s\n\t"
                     "fmul v1.4s, v1.4s, v17.4s\n\t"
                     "fmul v2.4s, v2.4s, v18.4s\n\t"
                     "fmul v3.4s, v3.4s, v19.4s\n\t"
                     "\n\t"
                     "ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[src]], #64\n\t"
                     "ld1 {v16.2d, v17.2d, v18.2d, v19.2d}, [%[scale]], #64\n\t"
                     "fmul v4.4s, v4.4s, v16.4s\n\t"
                     "fmul v5.4s, v5.4s, v17.4s\n\t"
                     "fmul v6.4s, v6.4s, v18.4s\n\t"
                     "fmul v7.4s, v7.4s, v19.4s\n\t"
                     "\n\t"
                     "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pldl1strm, [%[scale], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[dest]], #64\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16", "v17", "v18", "v19");
    }

    for (int i = 0; i < n2; ++i) {
        dest[i] = x[i] * src[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_aligned(double *CXXPH_RESTRICT src_dest, const double *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{

    int n1 = (n >> 4);
    int n2 = (n & 0xf);

    if (n1 > 0) {
        const double *CXXPH_RESTRICT src = src_dest;
        asm volatile("prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pldl1strm, [%[scale], #128]\n\t"
                     "1:\n\t"
                     "ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[src]], #64\n\t"
                     "ld1 {v16.2d, v17.2d, v18.2d, v19.2d}, [%[scale]], #64\n\t"
                     "fmul v0.2d, v0.2d, v16.2d\n\t"
                     "fmul v1.2d, v1.2d, v17.2d\n\t"
                     "fmul v2.2d, v2.2d, v18.2d\n\t"
                     "fmul v3.2d, v3.2d, v19.2d\n\t"
                     "\n\t"
                     "ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[src]], #64\n\t"
                     "ld1 {v16.2d, v17.2d, v18.2d, v19.2d}, [%[scale]], #64\n\t"
                     "fmul v4.2d, v4.2d, v16.2d\n\t"
                     "fmul v5.2d, v5.2d, v17.2d\n\t"
                     "fmul v6.2d, v6.2d, v18.2d\n\t"
                     "fmul v7.2d, v7.2d, v19.2d\n\t"
                     "\n\t"
                     "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pldl1strm, [%[scale], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(src_dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16", "v17", "v18", "v19");
    }

    for (int i = 0; i < n2; ++i) {
        src_dest[i] *= x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src,
                                          const double *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{

    int n1 = (n >> 4);
    int n2 = (n & 0xf);

    if (n1 > 0) {
        asm volatile("prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pldl1strm, [%[scale], #128]\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "1:\n\t"
                     "ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[src]], #64\n\t"
                     "ld1 {v16.2d, v17.2d, v18.2d, v19.2d}, [%[scale]], #64\n\t"
                     "fmul v0.2d, v0.2d, v16.2d\n\t"
                     "fmul v1.2d, v1.2d, v17.2d\n\t"
                     "fmul v2.2d, v2.2d, v18.2d\n\t"
                     "fmul v3.2d, v3.2d, v19.2d\n\t"
                     "\n\t"
                     "ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[src]], #64\n\t"
                     "ld1 {v16.2d, v17.2d, v18.2d, v19.2d}, [%[scale]], #64\n\t"
                     "fmul v4.2d, v4.2d, v16.2d\n\t"
                     "fmul v5.2d, v5.2d, v17.2d\n\t"
                     "fmul v6.2d, v6.2d, v18.2d\n\t"
                     "fmul v7.2d, v7.2d, v19.2d\n\t"
                     "\n\t"
                     "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[dest]], #64\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pldl1strm, [%[scale], #128]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[dest]], #64\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16", "v17", "v18", "v19");
    }

    for (int i = 0; i < n2; ++i) {
        dest[i] = x[i] * src[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_multiply_aligned(std::complex<float> *CXXPH_RESTRICT src_dest,
                                          const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i

    int n1 = (n >> 3);
    int n2 = (n & 0x7);

    if (n1 > 0) {
        std::complex<float> *CXXPH_RESTRICT src = src_dest;

        // q0,q2: dest.real
        // q1,q3: dest.imag
        // q8: src.real
        // q9: src.imag
        // q10: scale.real
        // q11: scale.imag
        asm volatile("pld [%[src], #64]\n\t"
                     "pld [%[scale], #64]\n"
                     "1:\n\t"
                     "vld2.32 {q8,q9}, [%[src]:128]!\n\t"
                     "vld2.32 {q10,q11}, [%[scale]:128]!\n\t"
                     "\n\t"
                     "vmul.f32 q0, q8, q10\n\t" // a0*b0
                     "vmul.f32 q1, q8, q11\n\t" // a0*b1
                     "vmls.f32 q0, q9, q11\n\t" // (a0*b0) - (a1*b1)
                     "vmla.f32 q1, q9, q10\n\t" // (a0*b1) + (a1*b0)
                     "\n\t"
                     "vld2.32 {q8,q9}, [%[src]:128]!\n\t"
                     "vld2.32 {q10,q11}, [%[scale]:128]!\n\t"
                     "\n\t"
                     "vmul.f32 q2, q8, q10\n\t" // a0*b0
                     "vmul.f32 q3, q8, q11\n\t" // a0*b1
                     "vmls.f32 q2, q9, q11\n\t" // (a0*b0) - (a1*b1)
                     "vmla.f32 q3, q9, q10\n\t" // (a0*b1) + (a1*b0)
                     "vst2.32 {q0,q1}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "pld [%[src], #64]\n\t"
                     "pld [%[scale], #64]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "vst2.32 {q2,q3}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(src_dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11");
    }

    for (int i = 0; i < n2; ++i) {
        src_dest[i] *= x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_aligned(std::complex<float> *CXXPH_RESTRICT src_dest,
                                          const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    int n1 = (n >> 3);
    int n2 = (n & 0x7);

    if (n1 > 0) {
        std::complex<float> *CXXPH_RESTRICT src = src_dest;

        // v0,v2: dest.real
        // v1,v3: dest.imag
        // v16,v20: src.real
        // v17,v21: src.imag
        // v18,v22: scale.real
        // v19,v23: scale.imag
        asm volatile("prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pldl1strm, [%[scale], #64]\n\t"
                     "1:\n\t"
                     "ld2 {v16.4s, v17.4s}, [%[src]], #32\n\t"
                     "ld2 {v18.4s, v19.4s}, [%[scale]], #32\n\t"
                     "\n\t"
                     "fmul v0.4s, v16.4s, v18.4s\n\t" // a0*b0
                     "fmul v1.4s, v16.4s, v19.4s\n\t" // a0*b1
                     "fmls v0.4s, v17.4s, v19.4s\n\t" // (a0*b0) - (a1*b1)
                     "fmla v1.4s, v17.4s, v18.4s\n\t" // (a0*b1) + (a1*b0)
                     "\n\t"
                     "ld2 {v20.4s, v21.4s}, [%[src]], #32\n\t"
                     "ld2 {v22.4s, v23.4s}, [%[scale]], #32\n\t"
                     "\n\t"
                     "fmul v2.4s, v20.4s, v22.4s\n\t" // a0*b0
                     "fmul v3.4s, v20.4s, v23.4s\n\t" // a0*b1
                     "fmls v2.4s, v21.4s, v23.4s\n\t" // (a0*b0) - (a1*b1)
                     "fmla v3.4s, v21.4s, v22.4s\n\t" // (a0*b1) + (a1*b0)
                     "\n\t"
                     "st2 {v0.4s, v1.4s}, [%[dest]], #32\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pldl1strm, [%[scale], #64]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st2 {v2.4s, v3.4s}, [%[dest]], #32\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(src_dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23");
    }

    for (int i = 0; i < n2; ++i) {
        src_dest[i] *= x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_multiply_aligned(std::complex<float> *CXXPH_RESTRICT dest,
                                          const std::complex<float> *CXXPH_RESTRICT src,
                                          const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i

    int n1 = (n >> 3);
    int n2 = (n & 0x7);

    if (n1 > 0) {
        // q0,q2: dest.real
        // q1,q3: dest.imag
        // q8: src.real
        // q9: src.imag
        // q10: scale.real
        // q11: scale.imag
        asm volatile("pld [%[src], #64]\n\t"
                     "pld [%[scale], #64]\n"
                     "1:\n\t"
                     "vld2.32 {q8,q9}, [%[src]:128]!\n\t"
                     "vld2.32 {q10,q11}, [%[scale]:128]!\n\t"
                     "\n\t"
                     "vmul.f32 q0, q8, q10\n\t" // a0*b0
                     "vmul.f32 q1, q8, q11\n\t" // a0*b1
                     "vmls.f32 q0, q9, q11\n\t" // (a0*b0) - (a1*b1)
                     "vmla.f32 q1, q9, q10\n\t" // (a0*b1) + (a1*b0)
                     "\n\t"
                     "vld2.32 {q8,q9}, [%[src]:128]!\n\t"
                     "vld2.32 {q10,q11}, [%[scale]:128]!\n\t"
                     "\n\t"
                     "vmul.f32 q2, q8, q10\n\t" // a0*b0
                     "vmul.f32 q3, q8, q11\n\t" // a0*b1
                     "vmls.f32 q2, q9, q11\n\t" // (a0*b0) - (a1*b1)
                     "vmla.f32 q3, q9, q10\n\t" // (a0*b1) + (a1*b0)
                     "vst2.32 {q0,q1}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "pld [%[src], #64]\n\t"
                     "pld [%[scale], #64]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "vst2.32 {q2,q3}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11");
    }

    for (int i = 0; i < n2; ++i) {
        dest[i] = src[i] * x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_aligned(std::complex<float> *CXXPH_RESTRICT dest,
                                          const std::complex<float> *CXXPH_RESTRICT src,
                                          const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    int n1 = (n >> 3);
    int n2 = (n & 0x7);

    if (n1 > 0) {
        // v0,v2: dest.real
        // v1,v3: dest.imag
        // v16,v20: src.real
        // v17,v21: src.imag
        // v18,v22: scale.real
        // v19,v23: scale.imag
        asm volatile("prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pldl1strm, [%[scale], #64]\n\t"
                     "prfm pstl1strm, [%[dest], #64]\n\t"
                     "1:\n\t"
                     "ld2 {v16.4s, v17.4s}, [%[src]], #32\n\t"
                     "ld2 {v18.4s, v19.4s}, [%[scale]], #32\n\t"
                     "\n\t"
                     "fmul v0.4s, v16.4s, v18.4s\n\t" // a0*b0
                     "fmul v1.4s, v16.4s, v19.4s\n\t" // a0*b1
                     "fmls v0.4s, v17.4s, v19.4s\n\t" // (a0*b0) - (a1*b1)
                     "fmla v1.4s, v17.4s, v18.4s\n\t" // (a0*b1) + (a1*b0)
                     "\n\t"
                     "ld2 {v20.4s, v21.4s}, [%[src]], #32\n\t"
                     "ld2 {v22.4s, v23.4s}, [%[scale]], #32\n\t"
                     "\n\t"
                     "fmul v2.4s, v20.4s, v22.4s\n\t" // a0*b0
                     "fmul v3.4s, v20.4s, v23.4s\n\t" // a0*b1
                     "fmls v2.4s, v21.4s, v23.4s\n\t" // (a0*b0) - (a1*b1)
                     "fmla v3.4s, v21.4s, v22.4s\n\t" // (a0*b1) + (a1*b0)
                     "st2 {v0.4s, v1.4s}, [%[dest]], #32\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pldl1strm, [%[scale], #64]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st2 {v2.4s, v3.4s}, [%[dest]], #32\n\t"
                     "prfm pstl1strm, [%[dest], #64]\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23");
    }

    for (int i = 0; i < n2; ++i) {
        dest[i] = src[i] * x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_aligned(std::complex<double> *CXXPH_RESTRICT src_dest,
                                          const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    int n1 = (n >> 2);
    int n2 = (n & 0x3);

    if (n1 > 0) {
        std::complex<double> *CXXPH_RESTRICT src = src_dest;

        // v0,v2: dest.real
        // v1,v3: dest.imag
        // v16,v20: src.real
        // v17,v21: src.imag
        // v18,v22: scale.real
        // v19,v23: scale.imag
        asm volatile("prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pldl1strm, [%[scale], #64]\n\t"
                     "1:\n\t"
                     "ld2 {v16.2d, v17.2d}, [%[src]], #32\n\t"
                     "ld2 {v18.2d, v19.2d}, [%[scale]], #32\n\t"
                     "\n\t"
                     "fmul v0.2d, v16.2d, v18.2d\n\t" // a0*b0
                     "fmul v1.2d, v16.2d, v19.2d\n\t" // a0*b1
                     "fmls v0.2d, v17.2d, v19.2d\n\t" // (a0*b0) - (a1*b1)
                     "fmla v1.2d, v17.2d, v18.2d\n\t" // (a0*b1) + (a1*b0)
                     "\n\t"
                     "ld2 {v20.2d, v21.2d}, [%[src]], #32\n\t"
                     "ld2 {v22.2d, v23.2d}, [%[scale]], #32\n\t"
                     "\n\t"
                     "fmul v2.2d, v20.2d, v22.2d\n\t" // a0*b0
                     "fmul v3.2d, v20.2d, v23.2d\n\t" // a0*b1
                     "fmls v2.2d, v21.2d, v23.2d\n\t" // (a0*b0) - (a1*b1)
                     "fmla v3.2d, v21.2d, v22.2d\n\t" // (a0*b1) + (a1*b0)
                     "st2 {v0.2d, v1.2d}, [%[dest]], #32\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pldl1strm, [%[scale], #64]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st2 {v2.2d, v3.2d}, [%[dest]], #32\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(src_dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23");
    }

    for (int i = 0; i < n2; ++i) {
        src_dest[i] *= x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_multiply_aligned(std::complex<double> *CXXPH_RESTRICT dest,
                                          const std::complex<double> *CXXPH_RESTRICT src,
                                          const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    int n1 = (n >> 2);
    int n2 = (n & 0x3);

    if (n1 > 0) {
        // v0,v2: dest.real
        // v1,v3: dest.imag
        // v16,v20: src.real
        // v17,v21: src.imag
        // v18,v22: scale.real
        // v19,v23: scale.imag
        asm volatile("prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pldl1strm, [%[scale], #64]\n\t"
                     "prfm pstl1strm, [%[dest], #64]\n\t"
                     "1:\n\t"
                     "ld2 {v16.2d, v17.2d}, [%[src]], #32\n\t"
                     "ld2 {v18.2d, v19.2d}, [%[scale]], #32\n\t"
                     "\n\t"
                     "fmul v0.2d, v16.2d, v18.2d\n\t" // a0*b0
                     "fmul v1.2d, v16.2d, v19.2d\n\t" // a0*b1
                     "fmls v0.2d, v17.2d, v19.2d\n\t" // (a0*b0) - (a1*b1)
                     "fmla v1.2d, v17.2d, v18.2d\n\t" // (a0*b1) + (a1*b0)
                     "\n\t"
                     "ld2 {v20.2d, v21.2d}, [%[src]], #32\n\t"
                     "ld2 {v22.2d, v23.2d}, [%[scale]], #32\n\t"
                     "\n\t"
                     "fmul v2.2d, v20.2d, v22.2d\n\t" // a0*b0
                     "fmul v3.2d, v20.2d, v23.2d\n\t" // a0*b1
                     "fmls v2.2d, v21.2d, v23.2d\n\t" // (a0*b0) - (a1*b1)
                     "fmla v3.2d, v21.2d, v22.2d\n\t" // (a0*b1) + (a1*b0)
                     "st2 {v0.2d, v1.2d}, [%[dest]], #32\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pldl1strm, [%[scale], #64]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st2 {v2.2d, v3.2d}, [%[dest]], #32\n\t"
                     "prfm pstl1strm, [%[dest], #64]\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [scale] "+r"(x), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23");
    }

    for (int i = 0; i < n2; ++i) {
        dest[i] = src[i] * x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_conj_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    uint32_t *CXXPH_RESTRICT u32_src_dest = reinterpret_cast<uint32_t *>(src_dest);

    if (n == 0) {
        return;
    }

    const int n1 = n >> 3;
    const int n2 = n & 0x7;

    if (n1 > 0) {
        CXXPH_ALIGNAS(16) static const uint32_t conj_bitptn[4] = { 0x80000000ul, // imag
                                                                   0x80000000ul, // imag
                                                                   0x80000000ul, // imag
                                                                   0x80000000ul, // imag
        };

        uint32_t *CXXPH_RESTRICT u32_src = u32_src_dest;
        int cnt = n1;
        asm volatile("vld1.32 {q8}, [%[conj_bitptn]:128]\n"
                     "1:\n\t"
                     "vld2.32 {q0,q1}, [%[src]:128]!\n\t"
                     "veor.f32 q1, q8\n\t"
                     "\n\t"
                     "vld2.32 {q2,q3}, [%[src]:128]!\n\t"
                     "veor.f32 q3, q8\n\t"
                     "\n\t"
                     "vst2.32 {q0,q1}, [%[dest]:128]!\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "vst2.32 {q2,q3}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(u32_src), [dest] "+r"(u32_src_dest), [cnt] "+r"(cnt)
                     : [conj_bitptn] "r"(conj_bitptn)
                     : "memory", "q0", "q1", "q2", "q3", "q8");
    }

    for (int i = 0; i < n2; ++i) {
        u32_src_dest[1] ^= 0x80000000ul;
        u32_src_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_conj_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    uint32_t *CXXPH_RESTRICT u32_src_dest = reinterpret_cast<uint32_t *>(src_dest);

    if (n == 0) {
        return;
    }

    const int n1 = n >> 4;
    const int n2 = n & 0xf;

    if (n1 > 0) {
        CXXPH_ALIGNAS(16) static const uint32_t conj_bitptn[4] = { 0x80000000ul, // imag
                                                                   0x80000000ul, // imag
                                                                   0x80000000ul, // imag
                                                                   0x80000000ul, // imag
        };

        uint32_t *CXXPH_RESTRICT u32_src = u32_src_dest;
        int cnt = n1;
        asm volatile("prfm pldl1strm, [%[src], #128]\n\t"
                     "ld1 {v16.4s}, [%[conj_bitptn]]\n"
                     "1:\n\t"
                     "ld2 {v0.4s, v1.4s}, [%[src]], #32\n\t"
                     "eor v1.16b, v1.16b, v16.16b\n\t"
                     "\n\t"
                     "ld2 {v2.4s, v3.4s}, [%[src]], #32\n\t"
                     "eor v3.16b, v3.16b, v16.16b\n\t"
                     "\n\t"
                     "ld2 {v4.4s, v5.4s}, [%[src]], #32\n\t"
                     "eor v5.16b, v5.16b, v16.16b\n\t"
                     "\n\t"
                     "ld2 {v6.4s, v7.4s}, [%[src]], #32\n\t"
                     "eor v7.16b, v7.16b, v16.16b\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "\n\t"
                     "st2 {v0.4s, v1.4s}, [%[dest]], #32\n\t"
                     "st2 {v2.4s, v3.4s}, [%[dest]], #32\n\t"
                     "st2 {v4.4s, v5.4s}, [%[dest]], #32\n\t"
                     "st2 {v6.4s, v7.4s}, [%[dest]], #32\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(u32_src), [dest] "+r"(u32_src_dest), [cnt] "+r"(cnt)
                     : [conj_bitptn] "r"(conj_bitptn)
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16");
    }

    for (int i = 0; i < n2; ++i) {
        u32_src_dest[1] ^= 0x80000000ul;
        u32_src_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_conj_aligned(std::complex<float> *CXXPH_RESTRICT dest,
                                      const std::complex<float> *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{
    uint32_t *CXXPH_RESTRICT u32_dest = reinterpret_cast<uint32_t *>(dest);
    const uint32_t *CXXPH_RESTRICT u32_src = reinterpret_cast<const uint32_t *>(src);

    if (n == 0) {
        return;
    }

    const int n1 = n >> 3;
    const int n2 = n & 0x7;

    if (n1 > 0) {
        CXXPH_ALIGNAS(16) static const uint32_t conj_bitptn[4] = { 0x80000000ul, // imag
                                                                   0x80000000ul, // imag
                                                                   0x80000000ul, // imag
                                                                   0x80000000ul, // imag
        };

        int cnt = n1;
        asm volatile("vld1.32 {q8}, [%[conj_bitptn]:128]\n\t"
                     "1:\n\t"
                     "vld2.32 {q0,q1}, [%[src]:128]!\n\t"
                     "veor.f32 q1, q8\n\t"
                     "\n\t"
                     "vld2.32 {q2,q3}, [%[src]:128]!\n\t"
                     "veor.f32 q3, q8\n\t"
                     "\n\t"
                     "vst2.32 {q0,q1}, [%[dest]:128]!\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "vst2.32 {q2,q3}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(u32_src), [dest] "+r"(u32_dest), [cnt] "+r"(cnt)
                     : [conj_bitptn] "r"(conj_bitptn)
                     : "memory", "q0", "q1", "q2", "q3", "q8");
    }

    for (int i = 0; i < n2; ++i) {
        u32_dest[0] = u32_src[0];
        u32_dest[1] = u32_src[1] ^ 0x80000000ul;
        u32_src += 2;
        u32_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_conj_aligned(std::complex<float> *CXXPH_RESTRICT dest,
                                      const std::complex<float> *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{
    uint32_t *CXXPH_RESTRICT u32_dest = reinterpret_cast<uint32_t *>(dest);
    const uint32_t *CXXPH_RESTRICT u32_src = reinterpret_cast<const uint32_t *>(src);

    if (n == 0) {
        return;
    }

    const int n1 = n >> 4;
    const int n2 = n & 0xf;

    if (n1 > 0) {
        CXXPH_ALIGNAS(16) static const uint32_t conj_bitptn[4] = { 0x80000000ul, // imag
                                                                   0x80000000ul, // imag
                                                                   0x80000000ul, // imag
                                                                   0x80000000ul, // imag
        };

        int cnt = n1;
        asm volatile("prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "ld1 {v16.4s}, [%[conj_bitptn]]\n"
                     "1:\n\t"
                     "ld2 {v0.4s, v1.4s}, [%[src]], #32\n\t"
                     "eor v1.16b, v1.16b, v16.16b\n\t"
                     "\n\t"
                     "ld2 {v2.4s, v3.4s}, [%[src]], #32\n\t"
                     "eor v3.16b, v3.16b, v16.16b\n\t"
                     "\n\t"
                     "ld2 {v4.4s, v5.4s}, [%[src]], #32\n\t"
                     "eor v5.16b, v5.16b, v16.16b\n\t"
                     "\n\t"
                     "ld2 {v6.4s, v7.4s}, [%[src]], #32\n\t"
                     "eor v7.16b, v7.16b, v16.16b\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "\n\t"
                     "st2 {v0.4s, v1.4s}, [%[dest]], #32\n\t"
                     "st2 {v2.4s, v3.4s}, [%[dest]], #32\n\t"
                     "st2 {v4.4s, v5.4s}, [%[dest]], #32\n\t"
                     "st2 {v6.4s, v7.4s}, [%[dest]], #32\n\t"
                     "\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(u32_src), [dest] "+r"(u32_dest), [cnt] "+r"(cnt)
                     : [conj_bitptn] "r"(conj_bitptn)
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16");
    }

    for (int i = 0; i < n2; ++i) {
        u32_dest[0] = u32_src[0];
        u32_dest[1] = u32_src[1] ^ 0x80000000ul;
        u32_src += 2;
        u32_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_conj_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    uint64_t *CXXPH_RESTRICT u64_src_dest = reinterpret_cast<uint64_t *>(src_dest);

    if (n == 0) {
        return;
    }

    const int n1 = n >> 3;
    const int n2 = n & 0x7;

    if (n1 > 0) {
        CXXPH_ALIGNAS(16) static const uint64_t conj_bitptn[2] = { 0x8000000000000000ul, // imag
                                                                   0x8000000000000000ul, // imag
        };

        uint64_t *CXXPH_RESTRICT u64_src = u64_src_dest;
        int cnt = n1;
        asm volatile("vld1.32 {q0}, [%[conj_bitptn]:128]\n"
                     "1:\n\t"
                     "vld2.32 {q8,q9}, [%[src]:128]!\n\t"
                     "veor q9, q9, q0\n\t"
                     "vld2.32 {q10,q11}, [%[src]:128]!\n\t"
                     "veor q11, q11, q0\n\t"
                     "vld2.32 {q12,q13}, [%[src]:128]!\n\t"
                     "veor q13, q13, q0\n\t"
                     "vld2.32 {q14,q15}, [%[src]:128]!\n\t"
                     "veor q15, q15, q0\n\t"
                     "\n\t"
                     "vst2.32 {q8,q9}, [%[dest]:128]!\n\t"
                     "vst2.32 {q10,q11}, [%[dest]:128]!\n\t"
                     "vst2.32 {q12,q13}, [%[dest]:128]!\n\t"
                     "vst2.32 {q14,q15}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(u64_src), [dest] "+r"(u64_src_dest), [cnt] "+r"(cnt)
                     : [conj_bitptn] "r"(conj_bitptn)
                     : "memory", "q0", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15");
    }

    for (int i = 0; i < n2; ++i) {
        u64_src_dest[1] ^= 0x8000000000000000ull;
        u64_src_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_conj_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    uint64_t *CXXPH_RESTRICT u64_src_dest = reinterpret_cast<uint64_t *>(src_dest);

    if (n == 0) {
        return;
    }

    const int n1 = n >> 3;
    const int n2 = n & 0x7;

    if (n1 > 0) {
        CXXPH_ALIGNAS(16) static const uint64_t conj_bitptn[2] = { 0x8000000000000000ul, // imag
                                                                   0x8000000000000000ul, // imag
        };

        uint64_t *CXXPH_RESTRICT u64_src = u64_src_dest;
        int cnt = n1;
        asm volatile("ld1 {v16.2d}, [%[conj_bitptn]]\n"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "1:\n\t"
                     "ld2 {v0.2d, v1.2d}, [%[src]], #32\n\t"
                     "eor v1.16b, v1.16b, v16.16b\n\t"
                     "ld2 {v2.2d, v3.2d}, [%[src]], #32\n\t"
                     "eor v3.16b, v3.16b, v16.16b\n\t"
                     "ld2 {v4.2d, v5.2d}, [%[src]], #32\n\t"
                     "eor v5.16b, v5.16b, v16.16b\n\t"
                     "ld2 {v6.2d, v7.2d}, [%[src]], #32\n\t"
                     "eor v7.16b, v7.16b, v16.16b\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "\n\t"
                     "st2 {v0.2d, v1.2d}, [%[dest]], #32\n\t"
                     "st2 {v2.2d, v3.2d}, [%[dest]], #32\n\t"
                     "st2 {v4.2d, v5.2d}, [%[dest]], #32\n\t"
                     "st2 {v6.2d, v7.2d}, [%[dest]], #32\n\t"
                     "\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(u64_src), [dest] "+r"(u64_src_dest), [cnt] "+r"(cnt)
                     : [conj_bitptn] "r"(conj_bitptn)
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16");
    }

    for (int i = 0; i < n2; ++i) {
        u64_src_dest[1] ^= 0x8000000000000000ull;
        u64_src_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_conj_aligned(std::complex<double> *CXXPH_RESTRICT dest,
                                      const std::complex<double> *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{

    uint64_t *CXXPH_RESTRICT u64_dest = reinterpret_cast<uint64_t *>(dest);
    const uint64_t *CXXPH_RESTRICT u64_src = reinterpret_cast<const uint64_t *>(src);

    if (n == 0) {
        return;
    }

    const int n1 = n >> 3;
    const int n2 = n & 0x7;

    if (n1 > 0) {
        CXXPH_ALIGNAS(16) static const uint64_t conj_bitptn[2] = { 0x8000000000000000ul, // imag
                                                                   0x8000000000000000ul, // imag
        };

        int cnt = n1;
        asm volatile("vld1.32 {q0}, [%[conj_bitptn]:128]\n"
                     "1:\n\t"
                     "vld2.32 {q8,q9}, [%[src]:128]!\n\t"
                     "veor q9, q9, q0\n\t"
                     "vld2.32 {q10,q11}, [%[src]:128]!\n\t"
                     "veor q11, q11, q0\n\t"
                     "vld2.32 {q12,q13}, [%[src]:128]!\n\t"
                     "veor q13, q13, q0\n\t"
                     "vld2.32 {q14,q15}, [%[src]:128]!\n\t"
                     "veor q15, q15, q0\n\t"
                     "\n\t"
                     "vst2.32 {q8,q9}, [%[dest]:128]!\n\t"
                     "vst2.32 {q10,q11}, [%[dest]:128]!\n\t"
                     "vst2.32 {q12,q13}, [%[dest]:128]!\n\t"
                     "vst2.32 {q14,q15}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(u64_src), [dest] "+r"(u64_dest), [cnt] "+r"(cnt)
                     : [offset_m2] "r"(-2 * sizeof(double)), [conj_bitptn] "r"(conj_bitptn)
                     : "memory", "q0", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15");
    }

    for (int i = 0; i < n2; ++i) {
        u64_dest[0] = u64_src[0];
        u64_dest[1] = u64_src[1] ^ 0x8000000000000000ull;
        u64_src += 2;
        u64_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_conj_aligned(std::complex<double> *CXXPH_RESTRICT dest,
                                      const std::complex<double> *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{

    uint64_t *CXXPH_RESTRICT u64_dest = reinterpret_cast<uint64_t *>(dest);
    const uint64_t *CXXPH_RESTRICT u64_src = reinterpret_cast<const uint64_t *>(src);

    if (n == 0) {
        return;
    }

    const int n1 = n >> 3;
    const int n2 = n & 0x7;

    if (n1 > 0) {
        CXXPH_ALIGNAS(16) static const uint64_t conj_bitptn[2] = { 0x8000000000000000ul, // imag
                                                                   0x8000000000000000ul, // imag
        };

        int cnt = n1;
        asm volatile("ld1 {v16.2d}, [%[conj_bitptn]]\n"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "1:\n\t"
                     "ld2 {v0.2d, v1.2d}, [%[src]], #32\n\t"
                     "eor v1.16b, v1.16b, v16.16b\n\t"
                     "ld2 {v2.2d, v3.2d}, [%[src]], #32\n\t"
                     "eor v3.16b, v3.16b, v16.16b\n\t"
                     "ld2 {v4.2d, v5.2d}, [%[src]], #32\n\t"
                     "eor v5.16b, v5.16b, v16.16b\n\t"
                     "ld2 {v6.2d, v7.2d}, [%[src]], #32\n\t"
                     "eor v7.16b, v7.16b, v16.16b\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #128]\n\t"
                     "\n\t"
                     "st2 {v0.2d, v1.2d}, [%[dest]], #32\n\t"
                     "st2 {v2.2d, v3.2d}, [%[dest]], #32\n\t"
                     "st2 {v4.2d, v5.2d}, [%[dest]], #32\n\t"
                     "st2 {v6.2d, v7.2d}, [%[dest]], #32\n\t"
                     "\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(u64_src), [dest] "+r"(dest), [cnt] "+r"(cnt)
                     : [conj_bitptn] "r"(conj_bitptn)
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16");
    }

    for (int i = 0; i < n2; ++i) {
        u64_dest[0] = u64_src[0];
        u64_dest[1] = u64_src[1] ^ 0x8000000000000000ull;
        u64_src += 2;
        u64_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_mirror_conj_aligned(std::complex<float> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
    const uint32_t *CXXPH_RESTRICT src = reinterpret_cast<const uint32_t *>(&center[-1]);
    uint32_t *CXXPH_RESTRICT dest = reinterpret_cast<uint32_t *>(&center[1]);

    if (n == 0) {
        return;
    }

    const int n1 = 1;
    const int n2 = (n - n1) >> 3;
    const int n3 = (n - n1) & 0x7;

    // adjust alignment
    for (int i = 0; i < n1; ++i) {
        dest[0] = src[0];
        dest[1] = src[1] ^ 0x80000000ul;
        src -= 2;
        dest += 2;
    }

    if (n2 > 0) {
        int cnt = n2;
        asm volatile("vld2.32 {q12,q13}, [%[src]:128]\n\t"
                     "add %[src], %[src], %[offset_m8]\n" // add offset for pre-decrement opeartion
                     "1:\n\t"
                     "vld2.32 {q10,q11}, [%[src]:128], %[offset_m8]\n\t"
                     "vld2.32 {q8,q9}, [%[src]:128], %[offset_m8]\n\t"
                     "\n\t"
                     "pld [%[src], #-128]\n\t"
                     "\n\t"
                     "vext.32 q2, q10, q12, #1\n\t"
                     "vext.32 q3, q11, q13, #1\n\t"
                     "vext.32 q0, q8, q10, #1\n\t"
                     "vext.32 q1, q9, q11, #1\n\t"
                     "\n\t"
                     "vrev64.32 q2, q2\n\t"
                     "vrev64.32 q3, q3\n\t"
                     "vrev64.32 q0, q0\n\t"
                     "vrev64.32 q1, q1\n\t"
                     "\n\t"
                     "vswp.32 d4, d5\n\t"
                     "vswp.32 d6, d7\n\t"
                     "vswp.32 d0, d1\n\t"
                     "vswp.32 d2, d3\n\t"
                     "\n\t"
                     "vneg.f32 q3,q3\n\t"
                     "vneg.f32 q1,q1\n\t"
                     "\n\t"
                     "vmov q12, q8\n\t"
                     "vmov q13, q9\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "vst2.32 {q2,q3}, [%[dest]:128]!\n\t"
                     "vst2.32 {q0,q1}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     "sub %[src], %[src], %[offset_m8]\n\t" // correct src pointer position
                     : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(cnt)
                     : [offset_m8] "r"(-8 * sizeof(float))
                     : "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13");
    }

    for (int i = 0; i < n3; ++i) {
        dest[0] = src[0];
        dest[1] = src[1] ^ 0x80000000ul;
        src -= 2;
        dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_mirror_conj_aligned(std::complex<float> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
    const uint64_t *CXXPH_RESTRICT src = reinterpret_cast<const uint64_t *>(&center[-1LL]);
    uint64_t *CXXPH_RESTRICT dest = reinterpret_cast<uint64_t *>(&center[1]);

    if (n == 0) {
        return;
    }

    const int n1 = 1;
    const int n2 = (n - n1) >> 3;
    const int n3 = (n - n1) & 0x7;

    // adjust alignment
    for (int i = 0; i < n1; ++i) {
        dest[0] = src[0] ^ 0x8000000000000000ull;
        src -= 1;
        dest += 1;
    }

    if (n2 > 0) {
        CXXPH_ALIGNAS(16) static const uint8_t arrange_table[16] = { 16, 17, 18, 19, 20, 21, 22, 23,
                                                                     8,  9,  10, 11, 12, 13, 14, 15, };
        CXXPH_ALIGNAS(16) static const uint32_t conj_bitptn[4] = { 0x00000000ul, 0x80000000ul, 0x00000000ul,
                                                                   0x80000000ul, };

        int cnt = n2;
        asm volatile("prfm pldl1strm, [%[src], %[pldoffset]]\n\t"
                     "prfm pstl1strm, [%[dest], #64]\n\t"
                     "\n\t"
                     "ld1 {v4.2d}, [%[arrange_table]]\n\t"
                     "ld1 {v5.2d}, [%[conj_bitptn]]\n\t"
                     "ld1 {v20.2d}, [%[src]]\n"
                     "1:\n\t"
                     "sub %[src], %[src], #64\n"
                     "ld1 {v16.2d, v17.2d, v18.2d, v19.2d}, [%[src]]\n\t"
                     "prfm pldl1strm, [%[src], %[pldoffset]]\n\t"
                     "\n\t"
                     "tbl v0.16b, {v19.16b, v20.16b}, v4.16b\n\t"
                     "tbl v1.16b, {v18.16b, v19.16b}, v4.16b\n\t"
                     "tbl v2.16b, {v17.16b, v18.16b}, v4.16b\n\t"
                     "tbl v3.16b, {v16.16b, v17.16b}, v4.16b\n\t"
                     "\n\t"
                     "eor v0.16b, v0.16b, v5.16b\n\t"
                     "eor v1.16b, v1.16b, v5.16b\n\t"
                     "eor v2.16b, v2.16b, v5.16b\n\t"
                     "eor v3.16b, v3.16b, v5.16b\n\t"
                     "\n\t"
                     "mov v20.16b, v16.16b\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[dest]], #64\n\t"
                     "prfm pstl1strm, [%[dest], #64]\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(cnt)
                     : [arrange_table] "r"(arrange_table), [conj_bitptn] "r"(conj_bitptn), [pldoffset] "r"(-64LL)
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v16", "v17", "v18", "v19", "v20");
    }

    for (int i = 0; i < n3; ++i) {
        dest[0] = src[0] ^ 0x8000000000000000ull;
        src -= 1;
        dest += 1;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
static void aarch32_neon_mirror_conj_aligned(std::complex<double> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
    const uint64_t *CXXPH_RESTRICT src = reinterpret_cast<const uint64_t *>(&center[-1]);
    uint64_t *CXXPH_RESTRICT dest = reinterpret_cast<uint64_t *>(&center[1]);

    if (n == 0) {
        return;
    }

    const int n1 = n >> 3;
    const int n2 = n & 0x7;

    if (n1 > 0) {
        CXXPH_ALIGNAS(16) static const uint64_t conj_bitptn[2] = { 0x0000000000000000ull, // real
                                                                   0x8000000000000000ull, // imag
        };

        int cnt = n1;
        asm volatile("vld1.32 {q0}, [%[conj_bitptn]:128]\n"
                     "1:\n\t"
                     "vld1.32 {q8}, [%[src]:128], %[offset_m2]\n\t"
                     "veor q8, q8, q0\n\t"
                     "vld1.32 {q9}, [%[src]:128], %[offset_m2]\n\t"
                     "veor q9, q9, q0\n\t"
                     "vld1.32 {q10}, [%[src]:128], %[offset_m2]\n\t"
                     "veor q10, q10, q0\n\t"
                     "vld1.32 {q11}, [%[src]:128], %[offset_m2]\n\t"
                     "veor q11, q11, q0\n\t"
                     "vld1.32 {q12}, [%[src]:128], %[offset_m2]\n\t"
                     "veor q12, q12, q0\n\t"
                     "vld1.32 {q13}, [%[src]:128], %[offset_m2]\n\t"
                     "veor q13, q13, q0\n\t"
                     "vld1.32 {q14}, [%[src]:128], %[offset_m2]\n\t"
                     "veor q14, q14, q0\n\t"
                     "vld1.32 {q15}, [%[src]:128], %[offset_m2]\n\t"
                     "veor q15, q15, q0\n\t"
                     "\n\t"
                     "pld [%[src], #-128]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "vst1.32 {q8}, [%[dest]:128]!\n\t"
                     "vst1.32 {q9}, [%[dest]:128]!\n\t"
                     "vst1.32 {q10}, [%[dest]:128]!\n\t"
                     "vst1.32 {q11}, [%[dest]:128]!\n\t"
                     "vst1.32 {q12}, [%[dest]:128]!\n\t"
                     "vst1.32 {q13}, [%[dest]:128]!\n\t"
                     "vst1.32 {q14}, [%[dest]:128]!\n\t"
                     "vst1.32 {q15}, [%[dest]:128]!\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(cnt)
                     : [offset_m2] "r"(-2 * sizeof(double)), [conj_bitptn] "r"(conj_bitptn)
                     : "q0", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15");
    }

    for (int i = 0; i < n2; ++i) {
        dest[0] = src[0];
        dest[1] = src[1] ^ 0x8000000000000000ull;
        src -= 2;
        dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
static void aarch64_neon_mirror_conj_aligned(std::complex<double> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
    const uint64_t *CXXPH_RESTRICT src = reinterpret_cast<const uint64_t *>(&center[-1LL]);
    uint64_t *CXXPH_RESTRICT dest = reinterpret_cast<uint64_t *>(&center[1]);

    if (n == 0) {
        return;
    }

    const int n1 = n >> 3;
    const int n2 = n & 0x7;

    if (n1 > 0) {
        CXXPH_ALIGNAS(16) static const uint64_t conj_bitptn[2] = { 0x0000000000000000ull, // real
                                                                   0x8000000000000000ull, // imag
        };

        int cnt = n1;
        asm volatile("sub %[src], %[src], #(128 - 16)\n"
                     "prfm pldl1strm, [%[src], %[pldoffset]]\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "ld1 {v16.2d}, [%[conj_bitptn]]\n"
                     "1:\n\t"
                     "\n\t"
                     "ld1 {v0.2d}, [%[src]], #16\n\t"
                     "eor v0.16b, v0.16b, v16.16b\n\t"
                     "ld1 {v1.2d}, [%[src]], #16\n\t"
                     "eor v1.16b, v1.16b, v16.16b\n\t"
                     "ld1 {v2.2d}, [%[src]], #16\n\t"
                     "eor v2.16b, v2.16b, v16.16b\n\t"
                     "ld1 {v3.2d}, [%[src]], #16\n\t"
                     "eor v3.16b, v3.16b, v16.16b\n\t"
                     "ld1 {v4.2d}, [%[src]], #16\n\t"
                     "eor v4.16b, v4.16b, v16.16b\n\t"
                     "ld1 {v5.2d}, [%[src]], #16\n\t"
                     "eor v5.16b, v5.16b, v16.16b\n\t"
                     "ld1 {v6.2d}, [%[src]], #16\n\t"
                     "eor v6.16b, v6.16b, v16.16b\n\t"
                     "ld1 {v7.2d}, [%[src]], #16\n\t"
                     "eor v7.16b, v7.16b, v16.16b\n\t"
                     "\n\t"
                     "sub %[src], %[src], #256\n\t"
                     "prfm pldl1strm, [%[src], %[pldoffset]]\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "\n\t"
                     "st1 {v7.2d}, [%[dest]], #16\n\t"
                     "st1 {v6.2d}, [%[dest]], #16\n\t"
                     "st1 {v5.2d}, [%[dest]], #16\n\t"
                     "st1 {v4.2d}, [%[dest]], #16\n\t"
                     "st1 {v3.2d}, [%[dest]], #16\n\t"
                     "st1 {v2.2d}, [%[dest]], #16\n\t"
                     "st1 {v1.2d}, [%[dest]], #16\n\t"
                     "st1 {v0.2d}, [%[dest]], #16\n\t"
                     "\n\t"
                     "prfm pstl1strm, [%[dest], #128]\n\t"
                     "\n\t"
                     "bne 1b\n\t"
                     "add %[src], %[src], #(128 - 16)\n\t"
                     : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(cnt)
                     : [conj_bitptn] "r"(conj_bitptn), [pldoffset] "r"(-128LL)
                     : "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16");
    }

    for (int i = 0; i < n2; ++i) {
        dest[0] = src[0];
        dest[1] = src[1] ^ 0x8000000000000000ull;
        src -= 2;
        dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
void aarch32_neon_interleave(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src1,
                             const float *CXXPH_RESTRICT src2, int n) CXXPH_NOEXCEPT
{
    uint32_t *CXXPH_RESTRICT u32_dest = reinterpret_cast<uint32_t *>(dest);
    const uint32_t *CXXPH_RESTRICT u32_src1 = reinterpret_cast<const uint32_t *>(src1);
    const uint32_t *CXXPH_RESTRICT u32_src2 = reinterpret_cast<const uint32_t *>(src2);

    int n1 = n >> 4;
    int n2 = n & 0xf;

    if (n1 > 0) {
        asm volatile("pld [%[src1], #64]\n\t"
                     "pld [%[src2], #64]\n\t"
                     "1:\n\t"
                     "vld1.32 {q8, q9}, [%[src1]]!\n\t"
                     "vld1.32 {q10, q11}, [%[src2]]!\n\t"
                     "vswp q9, q10\n\t"
                     "vzip.32 q8, q9\n\t"
                     "vzip.32 q10, q11\n\t"
                     "vst1.32 {q8, q9}, [%[dest]]!\n\t"
                     "vst1.32 {q10, q11}, [%[dest]]!\n\t"
                     "\n\t"
                     "vld1.32 {q12, q13}, [%[src1]]!\n\t"
                     "vld1.32 {q14, q15}, [%[src2]]!\n\t"
                     "vswp q13, q14\n\t"
                     "vzip.32 q12, q13\n\t"
                     "vzip.32 q14, q15\n\t"
                     "vst1.32 {q12, q13}, [%[dest]]!\n\t"
                     "vst1.32 {q14, q15}, [%[dest]]!\n\t"
                     "\n\t"
                     "pld [%[src1], #64]\n\t"
                     "pld [%[src2], #64]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [dest] "+r"(u32_dest), [src1] "+r"(u32_src1), [src2] "+r"(u32_src2), [cnt] "+r"(n1)
                     :
                     : "memory", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15");
    }

    while (n2 > 0) {
        u32_dest[0] = u32_src1[0];
        u32_dest[1] = u32_src2[0];
        u32_dest += 2;
        u32_src1 += 1;
        u32_src2 += 1;

        n2 -= 1;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
void aarch64_neon_interleave(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src1,
                             const float *CXXPH_RESTRICT src2, int n) CXXPH_NOEXCEPT
{
    uint32_t *CXXPH_RESTRICT u32_dest = reinterpret_cast<uint32_t *>(dest);
    const uint32_t *CXXPH_RESTRICT u32_src1 = reinterpret_cast<const uint32_t *>(src1);
    const uint32_t *CXXPH_RESTRICT u32_src2 = reinterpret_cast<const uint32_t *>(src2);

    int n1 = n >> 4;
    int n2 = n & 0xf;

    if (n1 > 0) {
        asm volatile("prfm pldl1strm, [%[src1], #64]\n\t"
                     "prfm pldl1strm, [%[src2], #64]\n\t"
                     "prfm pstl1strm, [%[dest], #64]\n\t"
                     "1:\n\t"
                     "ld1 {v0.4s, v1.4s}, [%[src1]], #32\n\t"
                     "ld1 {v2.4s, v3.4s}, [%[src2]], #32\n\t"
                     "mov v8.16b, v1.16b\n\t"
                     "mov v1.16b, v2.16b\n\t"
                     "mov v2.16b, v8.16b\n\t"
                     "st2 {v0.4s, v1.4s}, [%[dest]], #32\n\t"
                     "st2 {v2.4s, v3.4s}, [%[dest]], #32\n\t"
                     "\n\t"
                     "ld1 {v4.4s, v5.4s}, [%[src1]], #32\n\t"
                     "ld1 {v6.4s, v7.4s}, [%[src2]], #32\n\t"
                     "mov v8.16b, v5.16b\n\t"
                     "mov v5.16b, v6.16b\n\t"
                     "mov v6.16b, v8.16b\n\t"
                     "st2 {v4.4s, v5.4s}, [%[dest]], #32\n\t"
                     "st2 {v6.4s, v7.4s}, [%[dest]], #32\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src1], #64]\n\t"
                     "prfm pldl1strm, [%[src2], #64]\n\t"
                     "prfm pstl1strm, [%[dest], #64]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [dest] "+r"(u32_dest), [src1] "+r"(u32_src1), [src2] "+r"(u32_src2), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8");
    }

    while (n2 > 0) {
        u32_dest[0] = u32_src1[0];
        u32_dest[1] = u32_src2[0];
        u32_dest += 2;
        u32_src1 += 1;
        u32_src2 += 1;

        n2 -= 1;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
void aarch32_neon_interleave(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src1,
                             const double *CXXPH_RESTRICT src2, int n) CXXPH_NOEXCEPT
{
    uint64_t *CXXPH_RESTRICT u64_dest = reinterpret_cast<uint64_t *>(dest);
    const uint64_t *CXXPH_RESTRICT u64_src1 = reinterpret_cast<const uint64_t *>(src1);
    const uint64_t *CXXPH_RESTRICT u64_src2 = reinterpret_cast<const uint64_t *>(src2);

    int n1 = n >> 3;
    int n2 = n & 0x7;

    if (n1 > 0) {
        asm volatile("pld [%[src1], #64]\n\t"
                     "pld [%[src2], #64]\n\t"
                     "1:\n\t"
                     "vld1.64 {q8, q9}, [%[src1]]!\n\t"
                     "vld1.64 {q10, q11}, [%[src2]]!\n\t"
                     "vswp q9, q10\n\t"
                     "vswp d17, d18\n\t"
                     "vswp d21, d22\n\t"
                     "vst1.64 {q8, q9}, [%[dest]]!\n\t"
                     "vst1.64 {q10, q11}, [%[dest]]!\n\t"
                     "\n\t"
                     "vld1.64 {q12, q13}, [%[src1]]!\n\t"
                     "vld1.64 {q14, q15}, [%[src2]]!\n\t"
                     "vswp q13, q14\n\t"
                     "vswp d25, d26\n\t"
                     "vswp d29, d30\n\t"
                     "vst1.64 {q12, q13}, [%[dest]]!\n\t"
                     "vst1.64 {q14, q15}, [%[dest]]!\n\t"
                     "\n\t"
                     "pld [%[src1], #64]\n\t"
                     "pld [%[src2], #64]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [dest] "+r"(u64_dest), [src1] "+r"(u64_src1), [src2] "+r"(u64_src2), [cnt] "+r"(n1)
                     :
                     : "memory", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15");
    }

    while (n2 > 0) {
        u64_dest[0] = u64_src1[0];
        u64_dest[1] = u64_src2[0];
        u64_dest += 2;
        u64_src1 += 1;
        u64_src2 += 1;

        n2 -= 1;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
void aarch64_neon_interleave(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src1,
                             const double *CXXPH_RESTRICT src2, int n) CXXPH_NOEXCEPT
{
    uint64_t *CXXPH_RESTRICT u64_dest = reinterpret_cast<uint64_t *>(dest);
    const uint64_t *CXXPH_RESTRICT u64_src1 = reinterpret_cast<const uint64_t *>(src1);
    const uint64_t *CXXPH_RESTRICT u64_src2 = reinterpret_cast<const uint64_t *>(src2);

    int n1 = n >> 3;
    int n2 = n & 0x7;

    if (n1 > 0) {
        asm volatile("prfm pldl1strm, [%[src1], #64]\n\t"
                     "prfm pldl1strm, [%[src2], #64]\n\t"
                     "prfm pstl1strm, [%[dest], #64]\n\t"
                     "1:\n\t"
                     "ld1 {v0.2d, v1.2d}, [%[src1]], #32\n\t"
                     "ld1 {v2.2d, v3.2d}, [%[src2]], #32\n\t"
                     "mov v8.16b, v1.16b\n\t"
                     "mov v1.16b, v2.16b\n\t"
                     "mov v2.16b, v8.16b\n\t"
                     "st2 {v0.2d, v1.2d}, [%[dest]], #32\n\t"
                     "st2 {v2.2d, v3.2d}, [%[dest]], #32\n\t"
                     "\n\t"
                     "ld1 {v4.2d, v5.2d}, [%[src1]], #32\n\t"
                     "ld1 {v6.2d, v7.2d}, [%[src2]], #32\n\t"
                     "mov v8.16b, v5.16b\n\t"
                     "mov v5.16b, v6.16b\n\t"
                     "mov v6.16b, v8.16b\n\t"
                     "st2 {v4.2d, v5.2d}, [%[dest]], #32\n\t"
                     "st2 {v6.2d, v7.2d}, [%[dest]], #32\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src1], #64]\n\t"
                     "prfm pldl1strm, [%[src2], #64]\n\t"
                     "prfm pstl1strm, [%[dest], #64]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [dest] "+r"(u64_dest), [src1] "+r"(u64_src1), [src2] "+r"(u64_src2), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8");
    }

    while (n2 > 0) {
        u64_dest[0] = u64_src1[0];
        u64_dest[1] = u64_src2[0];
        u64_dest += 2;
        u64_src1 += 1;
        u64_src2 += 1;

        n2 -= 1;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
void aarch32_neon_deinterleave(float *CXXPH_RESTRICT dest1, float *CXXPH_RESTRICT dest2,
                               const float *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{
    uint32_t *CXXPH_RESTRICT u32_dest1 = reinterpret_cast<uint32_t *>(dest1);
    uint32_t *CXXPH_RESTRICT u32_dest2 = reinterpret_cast<uint32_t *>(dest2);
    const uint32_t *CXXPH_RESTRICT u32_src = reinterpret_cast<const uint32_t *>(src);

    int n1 = n >> 4;
    int n2 = n & 0xf;

    if (n1 > 0) {
        asm volatile("pld [%[src], #64]\n\t"
                     "1:\n\t"
                     "vld1.32 {q8, q9}, [%[src]]!\n\t"
                     "vld1.32 {q10, q11}, [%[src]]!\n\t"
                     "vuzp.32 q8, q9\n\t"
                     "vuzp.32 q10, q11\n\t"
                     "vswp q9, q10\n\t"
                     "vst1.32 {q8, q9}, [%[dest1]]!\n\t"
                     "vst1.32 {q10, q11}, [%[dest2]]!\n\t"
                     "\n\t"
                     "vld1.32 {q12, q13}, [%[src]]!\n\t"
                     "vld1.32 {q14, q15}, [%[src]]!\n\t"
                     "vuzp.32 q12, q13\n\t"
                     "vuzp.32 q14, q15\n\t"
                     "vswp q13, q14\n\t"
                     "vst1.32 {q12, q13}, [%[dest1]]!\n\t"
                     "vst1.32 {q14, q15}, [%[dest2]]!\n\t"
                     "\n\t"
                     "pld [%[src], #64]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [dest1] "+r"(u32_dest1), [dest2] "+r"(u32_dest2), [src] "+r"(u32_src), [cnt] "+r"(n1)
                     :
                     : "memory", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15");
    }

    while (n2 > 0) {
        u32_dest1[0] = u32_src[0];
        u32_dest2[0] = u32_src[1];
        u32_dest1 += 1;
        u32_dest2 += 1;
        u32_src += 2;

        n2 -= 1;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
void aarch64_neon_deinterleave(float *CXXPH_RESTRICT dest1, float *CXXPH_RESTRICT dest2,
                               const float *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{
    uint32_t *CXXPH_RESTRICT u32_dest1 = reinterpret_cast<uint32_t *>(dest1);
    uint32_t *CXXPH_RESTRICT u32_dest2 = reinterpret_cast<uint32_t *>(dest2);
    const uint32_t *CXXPH_RESTRICT u32_src = reinterpret_cast<const uint32_t *>(src);

    int n1 = n >> 4;
    int n2 = n & 0xf;

    if (n1 > 0) {
        asm volatile("prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pstl1strm, [%[dest1], #64]\n\t"
                     "prfm pstl1strm, [%[dest2], #64]\n\t"
                     "1:\n\t"
                     "ld2 {v0.4s, v1.4s}, [%[src]], #32\n\t"
                     "ld2 {v2.4s, v3.4s}, [%[src]], #32\n\t"
                     "mov v8.16b, v1.16b\n\t"
                     "mov v1.16b, v2.16b\n\t"
                     "mov v2.16b, v8.16b\n\t"
                     "st1 {v0.4s, v1.4s}, [%[dest1]], #32\n\t"
                     "st1 {v2.4s, v3.4s}, [%[dest2]], #32\n\t"
                     "\n\t"
                     "ld2 {v4.4s, v5.4s}, [%[src]], #32\n\t"
                     "ld2 {v6.4s, v7.4s}, [%[src]], #32\n\t"
                     "mov v8.16b, v5.16b\n\t"
                     "mov v5.16b, v6.16b\n\t"
                     "mov v6.16b, v8.16b\n\t"
                     "st1 {v4.4s, v5.4s}, [%[dest1]], #32\n\t"
                     "st1 {v6.4s, v7.4s}, [%[dest2]], #32\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pstl1strm, [%[dest1], #64]\n\t"
                     "prfm pstl1strm, [%[dest2], #64]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [dest1] "+r"(u32_dest1), [dest2] "+r"(u32_dest2), [src] "+r"(u32_src), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8");
    }

    while (n2 > 0) {
        u32_dest1[0] = u32_src[0];
        u32_dest2[0] = u32_src[1];
        u32_dest1 += 1;
        u32_dest2 += 1;
        u32_src += 2;

        n2 -= 1;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
void aarch32_neon_deinterleave(double *CXXPH_RESTRICT dest1, double *CXXPH_RESTRICT dest2,
                               const double *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{
    uint64_t *CXXPH_RESTRICT u64_dest1 = reinterpret_cast<uint64_t *>(dest1);
    uint64_t *CXXPH_RESTRICT u64_dest2 = reinterpret_cast<uint64_t *>(dest2);
    const uint64_t *CXXPH_RESTRICT u64_src = reinterpret_cast<const uint64_t *>(src);

    int n1 = n >> 3;
    int n2 = n & 0x7;

    if (n1 > 0) {
        asm volatile("pld [%[src], #64]\n\t"
                     "1:\n\t"
                     "vld1.64 {q8, q9}, [%[src]]!\n\t"
                     "vld1.64 {q10, q11}, [%[src]]!\n\t"
                     "vswp d17, d18\n\t"
                     "vswp d21, d22\n\t"
                     "vswp q9, q10\n\t"
                     "vst1.64 {q8, q9}, [%[dest1]]!\n\t"
                     "vst1.64 {q10, q11}, [%[dest2]]!\n\t"
                     "\n\t"
                     "vld1.64 {q12, q13}, [%[src]]!\n\t"
                     "vld1.64 {q14, q15}, [%[src]]!\n\t"
                     "vswp d25, d26\n\t"
                     "vswp d29, d30\n\t"
                     "vswp q13, q14\n\t"
                     "vst1.64 {q12, q13}, [%[dest1]]!\n\t"
                     "vst1.64 {q14, q15}, [%[dest2]]!\n\t"
                     "\n\t"
                     "pld [%[src], #64]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [dest1] "+r"(u64_dest1), [dest2] "+r"(u64_dest2), [src] "+r"(u64_src), [cnt] "+r"(n1)
                     :
                     : "memory", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15");
    }

    while (n2 > 0) {
        u64_dest1[0] = u64_src[0];
        u64_dest2[0] = u64_src[1];
        u64_dest1 += 1;
        u64_dest2 += 1;
        u64_src += 2;

        n2 -= 1;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
void aarch64_neon_deinterleave(double *CXXPH_RESTRICT dest1, double *CXXPH_RESTRICT dest2,
                               const double *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{
    uint64_t *CXXPH_RESTRICT u64_dest1 = reinterpret_cast<uint64_t *>(dest1);
    uint64_t *CXXPH_RESTRICT u64_dest2 = reinterpret_cast<uint64_t *>(dest2);
    const uint64_t *CXXPH_RESTRICT u64_src = reinterpret_cast<const uint64_t *>(src);

    int n1 = n >> 3;
    int n2 = n & 0x7;

    if (n1 > 0) {
        asm volatile("prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pstl1strm, [%[dest1], #64]\n\t"
                     "prfm pstl1strm, [%[dest2], #64]\n\t"
                     "1:\n\t"
                     "ld2 {v0.2d, v1.2d}, [%[src]], #32\n\t"
                     "ld2 {v2.2d, v3.2d}, [%[src]], #32\n\t"
                     "mov v8.16b, v1.16b\n\t"
                     "mov v1.16b, v2.16b\n\t"
                     "mov v2.16b, v8.16b\n\t"
                     "st1 {v0.2d, v1.2d}, [%[dest1]], #32\n\t"
                     "st1 {v2.2d, v3.2d}, [%[dest2]], #32\n\t"
                     "\n\t"
                     "ld2 {v4.2d, v5.2d}, [%[src]], #32\n\t"
                     "ld2 {v6.2d, v7.2d}, [%[src]], #32\n\t"
                     "mov v8.16b, v5.16b\n\t"
                     "mov v5.16b, v6.16b\n\t"
                     "mov v6.16b, v8.16b\n\t"
                     "st1 {v4.2d, v5.2d}, [%[dest1]], #32\n\t"
                     "st1 {v6.2d, v7.2d}, [%[dest2]], #32\n\t"
                     "\n\t"
                     "prfm pldl1strm, [%[src], #64]\n\t"
                     "prfm pstl1strm, [%[dest1], #64]\n\t"
                     "prfm pstl1strm, [%[dest2], #64]\n\t"
                     "\n\t"
                     "subs %[cnt], %[cnt], #1\n\t"
                     "bne 1b\n\t"
                     : [dest1] "+r"(u64_dest1), [dest2] "+r"(u64_dest2), [src] "+r"(u64_src), [cnt] "+r"(n1)
                     :
                     : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8");
    }

    while (n2 > 0) {
        u64_dest1[0] = u64_src[0];
        u64_dest2[0] = u64_src[1];
        u64_dest1 += 1;
        u64_dest2 += 1;
        u64_src += 2;

        n2 -= 1;
    }
}
#endif

//
// exposed functions
//

void multiply_scaler_aligned(float *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_multiply_scaler_aligned(src_dest, x, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_scaler_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(src_dest, x, n);
}

void multiply_scaler_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, float x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_multiply_scaler_aligned(dest, src, x, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_scaler_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(dest, src, x, n);
}

void multiply_scaler_aligned(double *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_scaler_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(src_dest, x, n);
}

void multiply_scaler_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src, double x, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_scaler_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(dest, src, x, n);
}

void multiply_aligned(float *CXXPH_RESTRICT src_dest, const float *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_multiply_aligned(src_dest, x, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    impl_general::multiply_aligned(src_dest, x, n);
}

void multiply_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, const float *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_multiply_aligned(dest, src, x, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    impl_general::multiply_aligned(dest, src, x, n);
}

void multiply_aligned(double *CXXPH_RESTRICT src_dest, const double *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    impl_general::multiply_aligned(src_dest, x, n);
}

void multiply_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src, const double *CXXPH_RESTRICT x,
                      int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    impl_general::multiply_aligned(dest, src, x, n);
}

void multiply_scaler_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_multiply_scaler_aligned(reinterpret_cast<float *>(src_dest), x, (2 * n));
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_scaler_aligned(reinterpret_cast<float *>(src_dest), x, (2 * n));
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(src_dest, x, n);
}

void multiply_scaler_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src,
                             float x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_multiply_scaler_aligned(reinterpret_cast<float *>(dest), reinterpret_cast<const float *>(src), x,
                                             (2 * n));
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_scaler_aligned(reinterpret_cast<float *>(dest), reinterpret_cast<const float *>(src), x,
                                             (2 * n));
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(dest, src, x, n);
}

void multiply_scaler_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_scaler_aligned(reinterpret_cast<double *>(src_dest), x, (2 * n));
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(src_dest, x, n);
}

void multiply_scaler_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                             double x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_scaler_aligned(reinterpret_cast<double *>(dest), reinterpret_cast<const double *>(src), x,
                                             (2 * n));
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(dest, src, x, n);
}

void multiply_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, const std::complex<float> *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_multiply_aligned(src_dest, x, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_aligned(src_dest, x, n);
}

void multiply_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src,
                      const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{

    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_multiply_aligned(dest, src, x, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_aligned(dest, src, x, n);
}

void multiply_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, const std::complex<double> *CXXPH_RESTRICT x,
                      int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_aligned(src_dest, x, n);
}

void multiply_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                      const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_multiply_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::multiply_aligned(dest, src, x, n);
}

void conj_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_conj_aligned(src_dest, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_conj_aligned(src_dest, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::conj_aligned(src_dest, n);
}

void conj_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_conj_aligned(dest, src, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_conj_aligned(dest, src, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::conj_aligned(dest, src, n);
}

void conj_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_conj_aligned(src_dest, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_conj_aligned(src_dest, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::conj_aligned(src_dest, n);
}

void conj_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_conj_aligned(dest, src, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_conj_aligned(dest, src, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::conj_aligned(dest, src, n);
}

void mirror_conj_aligned(std::complex<float> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_mirror_conj_aligned(center, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_mirror_conj_aligned(center, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::mirror_conj_aligned(center, n);
}

void mirror_conj_aligned(std::complex<double> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_mirror_conj_aligned(center, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_mirror_conj_aligned(center, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::mirror_conj_aligned(center, n);
}

void interleave(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src1, const float *CXXPH_RESTRICT src2, int n)
    CXXPH_NOEXCEPT
{
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_interleave(dest, src1, src2, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_interleave(dest, src1, src2, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::interleave(dest, src1, src2, n);
}

void interleave(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src1, const double *CXXPH_RESTRICT src2,
                int n) CXXPH_NOEXCEPT
{
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_interleave(dest, src1, src2, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_interleave(dest, src1, src2, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::interleave(dest, src1, src2, n);
}

void deinterleave(float *CXXPH_RESTRICT dest1, float *CXXPH_RESTRICT dest2, const float *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_deinterleave(dest1, dest2, src, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_deinterleave(dest1, dest2, src, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::deinterleave(dest1, dest2, src, n);
}

void deinterleave(double *CXXPH_RESTRICT dest1, double *CXXPH_RESTRICT dest2, const double *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch32_neon_deinterleave(dest1, dest2, src, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
    if (cxxporthelper::platform_info::support_arm_neon()) {
        aarch64_neon_deinterleave(dest1, dest2, src, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::deinterleave(dest1, dest2, src, n);
}

} // namespace impl_neon
} // namespace utils
} // namespace cxxdasp

#endif // (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
