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

#include <cxxdasp/filter/biquad/f32_stereo_neon_biquad_df1_core_operator.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

#include <cxxdasp/utils/utils.hpp>
// #include <cxxdasp/utils/neon_debug_utils.hpp>

namespace cxxdasp {
namespace filter {
namespace impl {

#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM

static void perform_phase1(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT x_, float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT;

static void perform_phase1(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT x_,
                           float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{
    // NOTE: it's safe to pass the same buffer for the source and destination.
    perform_phase1(src_dest, src_dest, n, b_, ia_, x_, y_);
}

static void perform_phase1(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT x_, float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{

    CXXDASP_UTIL_ASSUME_ALIGNED(src, 8);
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 8);

    n = (n >> 2) - 1;

    asm volatile("@ === PROLOGUE ===\n\t"
                 "vld1.32 {d8, d9}, [%[b]]\n\t"
                 "vld1.32 {d10}, [%[ia]]\n\t"
                 "vld1.32 {d16, d17}, [%[dx]]\n\t"
                 "vld1.32 {d18, d19}, [%[dy]]\n\t"
                 "\n\t"
                 "pld [%[src], #64]\n\t"
                 "pld [%[dest], #64]\n"
                 "\n\t"
                 "@ === y01, y23 - b0 ===\n\t"
                 "vld1.32 {d0, d1}, [%[src]]!\n\t"
                 "vmul.f32 q2, q0, d8[0]\n\t"
                 "vld1.32 {d2, d3}, [%[src]]!\n\t"
                 "vmul.f32 q3, q1, d8[0]\n\t"
                 "\n\t"
                 "@ === y01, y23 - b1 ===\n\t"
                 "vext.32 q10, q8, q0, #2\n\t"
                 "vext.32 q11, q0, q1, #2\n\t"
                 "vmla.f32 q2, q10, d8[1]\n\t"
                 "vmla.f32 q3, q11, d8[1]\n\t"
                 "\n\t"
                 "@ === y01, y23 - b2 ===\n\t"
                 "vmla.f32 q2, q8, d9[0]\n\t"
                 "vmla.f32 q3, q0, d9[0]\n\t"
                 "\n\t"
                 "vmov q8, q1                    @ dx = x23\n\t"
                 "\n\t"
                 "@ === LOOP ===\n"
                 "1:\n\t"
                 "vmla.f32 q2, q9, d10[1]        @ y01 - ia1\n\t"
                 "\n\t"
                 "vld1.32 {d24, d25}, [%[src]]!  @ x45 load\n\t"
                 "vmul.f32 q14, q12, d8[0]       @ y45 - b0\n\t"
                 "\n\t"
                 "vmla.f32 d4, d19, d10[0]       @ y0  - ia0\n\t"
                 "\n\t"
                 "vld1.32 {d26, d27}, [%[src]]!  @ x67 load\n\t"
                 "vmul.f32 q15, q13, d8[0]       @ y67 - b0\n\t"
                 "\n\t"
                 "vmla.f32 d5, d4, d10[0]        @ y1  - ia0\n\t"
                 "\n\t"
                 "vext.32 q10, q8, q12, #2       @ y45 - b1 (1)\n\t"
                 "vmla.f32 q14, q10, d8[1]       @ y45 - b1 (2)\n\t"
                 "\n\t"
                 "vmla.f32 q3, q2, d10[1]        @ y23 - ia1\n\t"
                 "\n\t"
                 "vext.32 q11, q12, q13, #2      @ y67 - b1 (1)\n\t"
                 "vmla.f32 q15, q11, d8[1]       @ y67 - b1 (2)\n\t"
                 "\n\t"
                 "vmla.f32 d6, d5, d10[0]        @ y2  - ia0\n\t"
                 "\n\t"
                 "vmla.f32 q14, q8, d9[0]        @ y45 - b2\n\t"
                 "vmla.f32 q15, q12, d9[0]       @ y67 - b2\n\t"
                 "\n\t"
                 "vmla.f32 d7, d6, d10[0]        @ y3  - ia0\n\t"
                 "\n\t"
                 "pld [%[src], #64]\n\t"
                 "pld [%[dest], #96]\n\t"
                 "subs %[cnt], %[cnt], #1\n\t"
                 "vmov q8, q13                   @ dx = x67\n\t"
                 "\n\t"
                 "vst1.32 {d4, d5}, [%[dest]]!   @ y01 store\n\t"
                 "vst1.32 {d6, d7}, [%[dest]]!   @ y23 store\n\t"
                 "\n\t"
                 "vmov q9, q3                    @ dy = y23\n\t"
                 "vmov q2, q14                   @ y01 = y45\n\t"
                 "vmov q3, q15                   @ y23 = y67\n\t"
                 "\n\t"
                 "bne 1b\n\t"
                 "\n\t"
                 "\n\t"
                 "@ === EPILOGUE ===\n\t"
                 "@ === y01 - ia0, ia1 ===\n\t"
                 "vmla.f32 q2, q9, d10[1]\n\t"
                 "vmla.f32 d4, d19, d10[0]\n\t"
                 "vmla.f32 d5, d4, d10[0]\n\t"
                 "\n\t"
                 "@ === y23 - ia0, ia1 ===\n\t"
                 "vmla.f32 q3, q2, d10[1]\n\t"
                 "vmla.f32 d6, d5, d10[0]\n\t"
                 "vmla.f32 d7, d6, d10[0]\n\t"
                 "\n\t"
                 "vst1.32 {d4, d5}, [%[dest]]!\n\t"
                 "vst1.32 {d6, d7}, [%[dest]]!\n\t"
                 "\n\t"
                 "vst1.32 {d16, d17}, [%[dx]]\n\t"
                 "vst1.32 {d6, d7}, [%[dy]]\n\t"
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(n)
                 : [b] "r"(b_), [ia] "r"(ia_), [dx] "r"(x_), [dy] "r"(y_)
                 : "memory", "q0", "q1", "q2", "q3", "d8", "d9", "d10", "q8", "q9", "q10", "q11", "q12", "q13", "q14",
                   "q15");
}

#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64

static void perform_phase1(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT x_,
                           float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{

    const float32x4_t b = vld1q_f32(b_);
    const float32x2_t ia = vld1_f32(ia_);
    float32x4_t dx = vld1q_f32(&x_[0]);
    float32x4_t dy = vld1q_f32(&y_[0]);

    __builtin_prefetch(&src_dest[32], 0, 1);

    float32x4_t y01, y23;

    // ==== PROLOGUE ====
    {
        float32x4_t x01, x23;

        // load x01, x23
        x01 = vld1q_f32(&src_dest[0]);
        x23 = vld1q_f32(&src_dest[4]);

        // y01, y23 - b0
        y01 = vmulq_lane_f32_compat(x01, vget_low_f32(b), 0);
        y23 = vmulq_lane_f32_compat(x23, vget_low_f32(b), 0);
        // y01, y23 - b1
        y01 = vmlaq_lane_f32_compat(y01, vextq_f32(dx, x01, 2), vget_low_f32(b), 1);
        y23 = vmlaq_lane_f32_compat(y23, vextq_f32(x01, x23, 2), vget_low_f32(b), 1);
        // y01, y23 - b2
        y01 = vmlaq_lane_f32_compat(y01, dx, vget_high_f32(b), 0);
        y23 = vmlaq_lane_f32_compat(y23, x01, vget_high_f32(b), 0);

        dx = x23;
    }

    // ==== LOOP ====
    for (int i = (n >> 2) - 1; i != 0; --i) {
        float32x4_t x45, x67;
        float32x4_t y45, y67;
        float32x2_t t0, t1, t2, t3;

        __builtin_prefetch(&src_dest[32], 0, 1);

        // y01 - ia0, ia1
        y01 = vmlaq_lane_f32_compat(y01, dy, ia, 1);

        {
            // load x45, x67
            x45 = vld1q_f32(&src_dest[8]);
            x67 = vld1q_f32(&src_dest[12]);
        }

        t0 = vget_low_f32(y01);
        t1 = vget_high_f32(y01);

        t0 = vmla_lane_f32(t0, vget_high_f32(dy), ia, 0);

        {
            // y45, y67 - b0
            y45 = vmulq_lane_f32_compat(x45, vget_low_f32(b), 0);
            y67 = vmulq_lane_f32_compat(x67, vget_low_f32(b), 0);
        }

        t1 = vmla_lane_f32(t1, t0, ia, 0);

        {
            // y45, y67 - b1
            y45 = vmlaq_lane_f32_compat(y45, vextq_f32(dx, x45, 2), vget_low_f32(b), 1);
            y67 = vmlaq_lane_f32_compat(y67, vextq_f32(x45, x67, 2), vget_low_f32(b), 1);
        }

        y01 = vcombine_f32(t0, t1);

        // y23 - ia0, ia1
        y23 = vmlaq_lane_f32_compat(y23, y01, ia, 1);

        // store y01
        vst1q_f32(src_dest, y01);
        src_dest += 4;

        t2 = vget_low_f32(y23);
        t3 = vget_high_f32(y23);

        t2 = vmla_lane_f32(t2, t1, ia, 0);

        {
            // y45, y67 - b2
            y45 = vmlaq_lane_f32_compat(y45, dx, vget_high_f32(b), 0);
            y67 = vmlaq_lane_f32_compat(y67, x45, vget_high_f32(b), 0);
        }

        t3 = vmla_lane_f32(t3, t2, ia, 0);

        y23 = vcombine_f32(t2, t3);

        // store y23
        vst1q_f32(src_dest, y23);
        src_dest += 4;

        dx = x67;
        dy = y23;
        y01 = y45;
        y23 = y67;
    }

    // ==== EPILOGUE ====
    {
        float32x2_t t0, t1, t2, t3;

        // y01 - ia0, ia1
        y01 = vmlaq_lane_f32_compat(y01, dy, ia, 1);

        t0 = vget_low_f32(y01);
        t1 = vget_high_f32(y01);

        t0 = vmla_lane_f32(t0, vget_high_f32(dy), ia, 0);
        t1 = vmla_lane_f32(t1, t0, ia, 0);

        y01 = vcombine_f32(t0, t1);

        // y23 - ia0, ia1
        y23 = vmlaq_lane_f32_compat(y23, y01, ia, 1);

        t2 = vget_low_f32(y23);
        t3 = vget_high_f32(y23);

        t2 = vmla_lane_f32(t2, t1, ia, 0);
        t3 = vmla_lane_f32(t3, t2, ia, 0);

        y23 = vcombine_f32(t2, t3);

        // store y01, y23
        vst1q_f32(src_dest, y01);
        src_dest += 4;
        vst1q_f32(src_dest, y23);
        src_dest += 4;

        dy = y23;
    }

    vst1q_f32(&x_[0], dx);
    vst1q_f32(&y_[0], dy);
}

static void perform_phase1(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT x_, float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{

    const float32x4_t b = vld1q_f32(b_);
    const float32x2_t ia = vld1_f32(ia_);
    float32x4_t dx = vld1q_f32(&x_[0]);
    float32x4_t dy = vld1q_f32(&y_[0]);

    __builtin_prefetch(&src[32], 0, 1);
    __builtin_prefetch(&dest[32], 1, 1);

    float32x4_t y01, y23;

    // ==== PROLOGUE ====
    {
        float32x4_t x01, x23;

        // load x01, x23
        x01 = vld1q_f32(src);
        src += 4;
        x23 = vld1q_f32(src);
        src += 4;

        // y01, y23 - b0
        y01 = vmulq_lane_f32_compat(x01, vget_low_f32(b), 0);
        y23 = vmulq_lane_f32_compat(x23, vget_low_f32(b), 0);
        // y01, y23 - b1
        y01 = vmlaq_lane_f32_compat(y01, vextq_f32(dx, x01, 2), vget_low_f32(b), 1);
        y23 = vmlaq_lane_f32_compat(y23, vextq_f32(x01, x23, 2), vget_low_f32(b), 1);
        // y01, y23 - b2
        y01 = vmlaq_lane_f32_compat(y01, dx, vget_high_f32(b), 0);
        y23 = vmlaq_lane_f32_compat(y23, x01, vget_high_f32(b), 0);

        dx = x23;
    }

    // ==== LOOP ====
    for (int i = (n >> 2) - 1; i != 0; --i) {
        float32x4_t x45, x67;
        float32x4_t y45, y67;
        float32x2_t t0, t1, t2, t3;

        __builtin_prefetch(&src[32], 0, 1);

        // y01 - ia0, ia1
        y01 = vmlaq_lane_f32_compat(y01, dy, ia, 1);

        {
            // load x45, x67
            x45 = vld1q_f32(src);
            src += 4;
            x67 = vld1q_f32(src);
            src += 4;
        }

        t0 = vget_low_f32(y01);
        t1 = vget_high_f32(y01);

        t0 = vmla_lane_f32(t0, vget_high_f32(dy), ia, 0);

        {
            // y45, y67 - b0
            y45 = vmulq_lane_f32_compat(x45, vget_low_f32(b), 0);
            y67 = vmulq_lane_f32_compat(x67, vget_low_f32(b), 0);
        }

        t1 = vmla_lane_f32(t1, t0, ia, 0);

        {
            // y45, y67 - b1
            y45 = vmlaq_lane_f32_compat(y45, vextq_f32(dx, x45, 2), vget_low_f32(b), 1);
            y67 = vmlaq_lane_f32_compat(y67, vextq_f32(x45, x67, 2), vget_low_f32(b), 1);
        }

        y01 = vcombine_f32(t0, t1);

        // y23 - ia0, ia1
        y23 = vmlaq_lane_f32_compat(y23, y01, ia, 1);

        // store y01
        vst1q_f32(dest, y01);
        dest += 4;

        t2 = vget_low_f32(y23);
        t3 = vget_high_f32(y23);

        t2 = vmla_lane_f32(t2, t1, ia, 0);

        {
            // y45, y67 - b2
            y45 = vmlaq_lane_f32_compat(y45, dx, vget_high_f32(b), 0);
            y67 = vmlaq_lane_f32_compat(y67, x45, vget_high_f32(b), 0);
        }

        t3 = vmla_lane_f32(t3, t2, ia, 0);

        y23 = vcombine_f32(t2, t3);

        __builtin_prefetch(&dest[20], 1, 1);

        // store y23
        vst1q_f32(dest, y23);
        dest += 4;

        dx = x67;
        dy = y23;
        y01 = y45;
        y23 = y67;
    }

    // ==== EPILOGUE ====
    {
        float32x2_t t0, t1, t2, t3;

        // y01 - ia0, ia1
        y01 = vmlaq_lane_f32_compat(y01, dy, ia, 1);

        t0 = vget_low_f32(y01);
        t1 = vget_high_f32(y01);

        t0 = vmla_lane_f32(t0, vget_high_f32(dy), ia, 0);
        t1 = vmla_lane_f32(t1, t0, ia, 0);

        y01 = vcombine_f32(t0, t1);

        // y23 - ia0, ia1
        y23 = vmlaq_lane_f32_compat(y23, y01, ia, 1);

        t2 = vget_low_f32(y23);
        t3 = vget_high_f32(y23);

        t2 = vmla_lane_f32(t2, t1, ia, 0);
        t3 = vmla_lane_f32(t3, t2, ia, 0);

        y23 = vcombine_f32(t2, t3);

        // store y01, y23
        vst1q_f32(dest, y01);
        dest += 4;
        vst1q_f32(dest, y23);
        dest += 4;

        dy = y23;
    }

    vst1q_f32(&x_[0], dx);
    vst1q_f32(&y_[0], dy);
}

#endif

static void perform_phase2(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT x_,
                           float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{

    const float32x4_t b = vld1q_f32(b_);
    const float32x2_t ia = vld1_f32(ia_);

    float32x2_t x0 = vld1_f32(&x_[2 * 1]);
    float32x2_t x1 = vld1_f32(&x_[2 * 0]);
    float32x2_t x2;

    float32x2_t y0 = vld1_f32(&y_[2 * 1]);
    float32x2_t y1 = vld1_f32(&y_[2 * 0]);
    float32x2_t y2;

    for (int i = 0; i < n; ++i) {

        x2 = x1;
        x1 = x0;
        x0 = vld1_f32(&src_dest[2 * i]);

        y2 = y1;
        y1 = y0;

        y0 = vmul_lane_f32(x0, vget_low_f32(b), 0);
        y0 = vmla_lane_f32(y0, x1, vget_low_f32(b), 1);
        y0 = vmla_lane_f32(y0, x2, vget_high_f32(b), 0);
        y0 = vmla_lane_f32(y0, y1, ia, 0);
        y0 = vmla_lane_f32(y0, y2, ia, 1);

        vst1_f32(&src_dest[2 * i], y0);
    }

    vst1_f32(&x_[2 * 1], x0);
    vst1_f32(&x_[2 * 0], x1);

    vst1_f32(&y_[2 * 1], y0);
    vst1_f32(&y_[2 * 0], y1);
}

static void perform_phase2(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT x_, float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{

    const float32x4_t b = vld1q_f32(b_);
    const float32x2_t ia = vld1_f32(ia_);

    float32x2_t x0 = vld1_f32(&x_[2 * 1]);
    float32x2_t x1 = vld1_f32(&x_[2 * 0]);
    float32x2_t x2;

    float32x2_t y0 = vld1_f32(&y_[2 * 1]);
    float32x2_t y1 = vld1_f32(&y_[2 * 0]);
    float32x2_t y2;

    for (int i = 0; i < n; ++i) {

        x2 = x1;
        x1 = x0;
        x0 = vld1_f32(&src[2 * i]);

        y2 = y1;
        y1 = y0;

        y0 = vmul_lane_f32(x0, vget_low_f32(b), 0);
        y0 = vmla_lane_f32(y0, x1, vget_low_f32(b), 1);
        y0 = vmla_lane_f32(y0, x2, vget_high_f32(b), 0);
        y0 = vmla_lane_f32(y0, y1, ia, 0);
        y0 = vmla_lane_f32(y0, y2, ia, 1);

        vst1_f32(&dest[2 * i], y0);
    }

    vst1_f32(&x_[2 * 1], x0);
    vst1_f32(&x_[2 * 0], x1);

    vst1_f32(&y_[2 * 1], y0);
    vst1_f32(&y_[2 * 0], y1);
}

void f32_stereo_neon_biquad_direct_form_1_core_operator_impl::perform(
    float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
    const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT x_, float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{

    const int n1 = (n >> 3) << 3; // (n / 8) * 8;
    const int n2 = (n & 0x7);     // n % 8

    if (n1 != 0) {
        perform_phase1(&src_dest[2 * 0], n1, b_, ia_, x_, y_);
    }

    if (n2 != 0) {
        perform_phase2(&src_dest[2 * n1], n2, b_, ia_, x_, y_);
    }
}

void f32_stereo_neon_biquad_direct_form_1_core_operator_impl::perform(const float32_t *CXXPH_RESTRICT src,
                                                                      float32_t *CXXPH_RESTRICT dest, unsigned int n,
                                                                      const float32_t *CXXPH_RESTRICT b_,
                                                                      const float32_t *CXXPH_RESTRICT ia_,
                                                                      float32_t *CXXPH_RESTRICT x_,
                                                                      float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{

    const int n1 = (n >> 3) << 3; // (n / 8) * 8;
    const int n2 = (n & 0x7);     // n % 8

    if (n1 != 0) {
        perform_phase1(&src[2 * 0], &dest[2 * 0], n1, b_, ia_, x_, y_);
    }

    if (n2 != 0) {
        perform_phase2(&src[2 * n1], &dest[2 * n1], n2, b_, ia_, x_, y_);
    }
}

} // namespace impl
} // namespace filter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
