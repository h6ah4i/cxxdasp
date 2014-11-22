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

#include <cxxdasp/filter/cascaded_biquad/f32_stereo_neon_cascaded_2_biquad_tdf2_core_operator.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

#define SUPPRESS_COMPILER_REORDER() asm volatile("" :: : "memory")

namespace cxxdasp {
namespace filter {
namespace impl {

#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
#define vld1_f32_inc_addr(dest_var, src_addr)                                                                          \
    asm("vld1.32 {%[dest]}, [%[src]]!" : [dest] "+w"(dest_var), [src] "+r"(src_addr) : :)
#define vst1_f32_inc_addr(dest_addr, src_var)                                                                          \
    asm("vst1.32 {%[src]}, [%[dest]]!" : [dest] "+r"(dest_addr), [src] "+w"(src_var) : :)
#else
#define vld1_f32_inc_addr(dest_var, src_addr)                                                                          \
    dest_var = vld1_f32(src_addr);                                                                                     \
    src_addr += 2
#define vst1_f32_inc_addr(dest_addr, src_var)                                                                          \
    vst1_f32(dest_addr, src_var);                                                                                      \
    dest_addr += 2
#endif

static void perform_phase1(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    const float32_t *CXXPH_RESTRICT src = src_dest;
    float32_t *CXXPH_RESTRICT dest = src_dest;

    float32x2_t s1 = vld1_f32(&s_[2 * 0]);
    float32x2_t s2 = vld1_f32(&s_[2 * 1]);
    float32x2_t s3 = vld1_f32(&s_[2 * 2]);
    float32x2_t s4 = vld1_f32(&s_[2 * 3]);
    float32x2_t y0;
    const float32x4_t b0q = vcombine_f32(vld1_dup_f32(&b_[2 * 0 + 0]), vld1_dup_f32(&b_[2 * 0 + 1]));
    const float32x4_t b1q = vcombine_f32(vld1_dup_f32(&b_[2 * 1 + 0]), vld1_dup_f32(&b_[2 * 1 + 1]));
    const float32x4_t b2q = vcombine_f32(vld1_dup_f32(&b_[2 * 2 + 0]), vld1_dup_f32(&b_[2 * 2 + 1]));
    const float32x4_t ia1q = vcombine_f32(vld1_dup_f32(&ia_[2 * 0 + 0]), vld1_dup_f32(&ia_[2 * 0 + 1]));
    const float32x4_t ia2q = vcombine_f32(vld1_dup_f32(&ia_[2 * 1 + 0]), vld1_dup_f32(&ia_[2 * 1 + 1]));

    // === PROLOGUE ===
    asm("// === PROLOGUE ===\n\t");
    {
        float32x2_t x0, z0;

        __builtin_prefetch(&dest[8], 1, 1);

        // ---
        x0 = vld1_f32(src);
        src += 2;

        y0 = vmla_f32(s1, x0, vget_low_f32(b0q));
        s1 = vmla_f32(s2, x0, vget_low_f32(b1q));
        s2 = vmul_f32(x0, vget_low_f32(b2q));
        s1 = vmla_f32(s1, y0, vget_low_f32(ia1q));
        s2 = vmla_f32(s2, y0, vget_low_f32(ia2q));

        z0 = vmla_f32(s3, y0, vget_high_f32(b0q));
        s3 = vmla_f32(s4, y0, vget_high_f32(b1q));
        s4 = vmul_f32(y0, vget_high_f32(b2q));
        s3 = vmla_f32(s3, z0, vget_high_f32(ia1q));
        s4 = vmla_f32(s4, z0, vget_high_f32(ia2q));

        vst1_f32_inc_addr(dest, z0);

        // ---
        vld1_f32_inc_addr(x0, src);

        y0 = vmla_f32(s1, x0, vget_low_f32(b0q));
        s1 = vmla_f32(s2, x0, vget_low_f32(b1q));
        s2 = vmul_f32(x0, vget_low_f32(b2q));
        s1 = vmla_f32(s1, y0, vget_low_f32(ia1q));
        s2 = vmla_f32(s2, y0, vget_low_f32(ia2q));

        n -= 2;
    }

    // === LOOP ===
    asm("// === LOOP ===\n\t");
    {
        float32x4_t x0y0, y0x0, s1s3, s2s4;

        s1s3 = vcombine_f32(s1, s3);
        s2s4 = vcombine_f32(s2, s4);

        do {
            float32x2_t x0, z0;
            float32x4_t y1z0;

            __builtin_prefetch(&dest[8], 1, 1);

            // ---
            vld1_f32_inc_addr(x0, src);

            x0y0 = vcombine_f32(x0, y0);

            y1z0 = vmlaq_f32(s1s3, x0y0, b0q);
            s1s3 = vmlaq_f32(s2s4, x0y0, b1q);
            s2s4 = vmulq_f32(x0y0, b2q);
            s1s3 = vmlaq_f32(s1s3, y1z0, ia1q);
            s2s4 = vmlaq_f32(s2s4, y1z0, ia2q);

            z0 = vget_high_f32(y1z0);

            vst1_f32_inc_addr(dest, z0);

            y0 = vget_low_f32(y1z0);

            // ---
            vld1_f32_inc_addr(x0, src);

            x0y0 = vcombine_f32(x0, y0);

            y1z0 = vmlaq_f32(s1s3, x0y0, b0q);
            s1s3 = vmlaq_f32(s2s4, x0y0, b1q);
            s2s4 = vmulq_f32(x0y0, b2q);
            s1s3 = vmlaq_f32(s1s3, y1z0, ia1q);
            s2s4 = vmlaq_f32(s2s4, y1z0, ia2q);

            SUPPRESS_COMPILER_REORDER(); // to optimize loop performance

            z0 = vget_high_f32(y1z0);

            vst1_f32_inc_addr(dest, z0);

            y0 = vget_low_f32(y1z0);
            // ---

            n -= 2;
        } while (CXXPH_LIKELY(n != 0));

        s1 = vget_low_f32(s1s3);
        s3 = vget_high_f32(s1s3);
        s2 = vget_low_f32(s2s4);
        s4 = vget_high_f32(s2s4);
    }

    // === EPILOGUE ===
    asm("// === EPILOGUE ===\n\t");
    {
        float32x2_t z0;

        z0 = vmla_f32(s3, y0, vget_high_f32(b0q));
        s3 = vmla_f32(s4, y0, vget_high_f32(b1q));
        s4 = vmul_f32(y0, vget_high_f32(b2q));
        s3 = vmla_f32(s3, z0, vget_high_f32(ia1q));
        s4 = vmla_f32(s4, z0, vget_high_f32(ia2q));

        vst1_f32(dest, z0);
    }

    vst1_f32(&s_[2 * 0], s1);
    vst1_f32(&s_[2 * 1], s2);
    vst1_f32(&s_[2 * 2], s3);
    vst1_f32(&s_[2 * 3], s4);
}

static void perform_phase1(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    float32x2_t s1 = vld1_f32(&s_[2 * 0]);
    float32x2_t s2 = vld1_f32(&s_[2 * 1]);
    float32x2_t s3 = vld1_f32(&s_[2 * 2]);
    float32x2_t s4 = vld1_f32(&s_[2 * 3]);
    float32x2_t y0;
    const float32x4_t b0q = vcombine_f32(vld1_dup_f32(&b_[2 * 0 + 0]), vld1_dup_f32(&b_[2 * 0 + 1]));
    const float32x4_t b1q = vcombine_f32(vld1_dup_f32(&b_[2 * 1 + 0]), vld1_dup_f32(&b_[2 * 1 + 1]));
    const float32x4_t b2q = vcombine_f32(vld1_dup_f32(&b_[2 * 2 + 0]), vld1_dup_f32(&b_[2 * 2 + 1]));
    const float32x4_t ia1q = vcombine_f32(vld1_dup_f32(&ia_[2 * 0 + 0]), vld1_dup_f32(&ia_[2 * 0 + 1]));
    const float32x4_t ia2q = vcombine_f32(vld1_dup_f32(&ia_[2 * 1 + 0]), vld1_dup_f32(&ia_[2 * 1 + 1]));

    // === PROLOGUE ===
    asm("// === PROLOGUE ===\n\t");
    {
        float32x2_t x0, z0;

        __builtin_prefetch(&src[8], 0, 1);
        __builtin_prefetch(&dest[8], 1, 1);

        // ---
        x0 = vld1_f32(src);
        src += 2;

        y0 = vmla_f32(s1, x0, vget_low_f32(b0q));
        s1 = vmla_f32(s2, x0, vget_low_f32(b1q));
        s2 = vmul_f32(x0, vget_low_f32(b2q));
        s1 = vmla_f32(s1, y0, vget_low_f32(ia1q));
        s2 = vmla_f32(s2, y0, vget_low_f32(ia2q));

        z0 = vmla_f32(s3, y0, vget_high_f32(b0q));
        s3 = vmla_f32(s4, y0, vget_high_f32(b1q));
        s4 = vmul_f32(y0, vget_high_f32(b2q));
        s3 = vmla_f32(s3, z0, vget_high_f32(ia1q));
        s4 = vmla_f32(s4, z0, vget_high_f32(ia2q));

        vst1_f32_inc_addr(dest, z0);

        // ---
        vld1_f32_inc_addr(x0, src);

        y0 = vmla_f32(s1, x0, vget_low_f32(b0q));
        s1 = vmla_f32(s2, x0, vget_low_f32(b1q));
        s2 = vmul_f32(x0, vget_low_f32(b2q));
        s1 = vmla_f32(s1, y0, vget_low_f32(ia1q));
        s2 = vmla_f32(s2, y0, vget_low_f32(ia2q));

        n -= 2;
    }

    // === LOOP ===
    asm("// === LOOP ===\n\t");
    {
        float32x4_t x0y0, y0x0, s1s3, s2s4;

        s1s3 = vcombine_f32(s1, s3);
        s2s4 = vcombine_f32(s2, s4);

        do {
            float32x2_t x0, z0;
            float32x4_t y1z0;

            __builtin_prefetch(&src[8], 0, 1);
            __builtin_prefetch(&dest[8], 1, 1);

            // ---
            vld1_f32_inc_addr(x0, src);

            x0y0 = vcombine_f32(x0, y0);

            y1z0 = vmlaq_f32(s1s3, x0y0, b0q);
            s1s3 = vmlaq_f32(s2s4, x0y0, b1q);
            s2s4 = vmulq_f32(x0y0, b2q);
            s1s3 = vmlaq_f32(s1s3, y1z0, ia1q);
            s2s4 = vmlaq_f32(s2s4, y1z0, ia2q);

            z0 = vget_high_f32(y1z0);

            vst1_f32_inc_addr(dest, z0);

            y0 = vget_low_f32(y1z0);

            // ---
            vld1_f32_inc_addr(x0, src);

            x0y0 = vcombine_f32(x0, y0);

            y1z0 = vmlaq_f32(s1s3, x0y0, b0q);
            s1s3 = vmlaq_f32(s2s4, x0y0, b1q);
            s2s4 = vmulq_f32(x0y0, b2q);
            s1s3 = vmlaq_f32(s1s3, y1z0, ia1q);
            s2s4 = vmlaq_f32(s2s4, y1z0, ia2q);

            SUPPRESS_COMPILER_REORDER(); // to optimize loop performance

            z0 = vget_high_f32(y1z0);

            vst1_f32_inc_addr(dest, z0);

            y0 = vget_low_f32(y1z0);
            // ---

            n -= 2;
        } while (CXXPH_LIKELY(n != 0));

        s1 = vget_low_f32(s1s3);
        s3 = vget_high_f32(s1s3);
        s2 = vget_low_f32(s2s4);
        s4 = vget_high_f32(s2s4);
    }

    // === EPILOGUE ===
    asm("// === EPILOGUE ===\n\t");
    {
        float32x2_t z0;

        z0 = vmla_f32(s3, y0, vget_high_f32(b0q));
        s3 = vmla_f32(s4, y0, vget_high_f32(b1q));
        s4 = vmul_f32(y0, vget_high_f32(b2q));
        s3 = vmla_f32(s3, z0, vget_high_f32(ia1q));
        s4 = vmla_f32(s4, z0, vget_high_f32(ia2q));

        vst1_f32(dest, z0);
    }

    vst1_f32(&s_[2 * 0], s1);
    vst1_f32(&s_[2 * 1], s2);
    vst1_f32(&s_[2 * 2], s3);
    vst1_f32(&s_[2 * 3], s4);
}

static void perform_phase2(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    const float32x2_t b0 = vld1_f32(&b_[2 * 0]);
    const float32x2_t b1 = vld1_f32(&b_[2 * 1]);
    const float32x2_t b2 = vld1_f32(&b_[2 * 2]);
    const float32x2_t ia1 = vld1_f32(&ia_[2 * 0]);
    const float32x2_t ia2 = vld1_f32(&ia_[2 * 1]);

    float32x2_t s1 = vld1_f32(&s_[2 * 0]);
    float32x2_t s2 = vld1_f32(&s_[2 * 1]);
    float32x2_t s3 = vld1_f32(&s_[2 * 2]);
    float32x2_t s4 = vld1_f32(&s_[2 * 3]);
    float32x2_t x0, y0, z0;

    for (int i = 0; i < n; ++i) {
        x0 = vld1_f32(&src_dest[2 * i]);

        y0 = vmla_lane_f32(s1, x0, b0, 0);
        s1 = vmla_lane_f32(s2, x0, b1, 0);
        s2 = vmul_lane_f32(x0, b2, 0);
        s1 = vmla_lane_f32(s1, y0, ia1, 0);
        s2 = vmla_lane_f32(s2, y0, ia2, 0);

        z0 = vmla_lane_f32(s3, y0, b0, 1);
        s3 = vmla_lane_f32(s4, y0, b1, 1);
        s4 = vmul_lane_f32(y0, b2, 1);
        s3 = vmla_lane_f32(s3, z0, ia1, 1);
        s4 = vmla_lane_f32(s4, z0, ia2, 1);

        vst1_f32(&src_dest[2 * i], z0);
    }

    vst1_f32(&s_[2 * 0], s1);
    vst1_f32(&s_[2 * 1], s2);
    vst1_f32(&s_[2 * 2], s3);
    vst1_f32(&s_[2 * 3], s4);
}

static void perform_phase2(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    const float32x2_t b0 = vld1_f32(&b_[2 * 0]);
    const float32x2_t b1 = vld1_f32(&b_[2 * 1]);
    const float32x2_t b2 = vld1_f32(&b_[2 * 2]);
    const float32x2_t ia1 = vld1_f32(&ia_[2 * 0]);
    const float32x2_t ia2 = vld1_f32(&ia_[2 * 1]);

    float32x2_t s1 = vld1_f32(&s_[2 * 0]);
    float32x2_t s2 = vld1_f32(&s_[2 * 1]);
    float32x2_t s3 = vld1_f32(&s_[2 * 2]);
    float32x2_t s4 = vld1_f32(&s_[2 * 3]);
    float32x2_t x0, y0, z0;

    for (int i = 0; i < n; ++i) {
        x0 = vld1_f32(&src[2 * i]);

        y0 = vmla_lane_f32(s1, x0, b0, 0);
        s1 = vmla_lane_f32(s2, x0, b1, 0);
        s2 = vmul_lane_f32(x0, b2, 0);
        s1 = vmla_lane_f32(s1, y0, ia1, 0);
        s2 = vmla_lane_f32(s2, y0, ia2, 0);

        z0 = vmla_lane_f32(s3, y0, b0, 1);
        s3 = vmla_lane_f32(s4, y0, b1, 1);
        s4 = vmul_lane_f32(y0, b2, 1);
        s3 = vmla_lane_f32(s3, z0, ia1, 1);
        s4 = vmla_lane_f32(s4, z0, ia2, 1);

        vst1_f32(&dest[2 * i], z0);
    }

    vst1_f32(&s_[2 * 0], s1);
    vst1_f32(&s_[2 * 1], s2);
    vst1_f32(&s_[2 * 2], s3);
    vst1_f32(&s_[2 * 3], s4);
}

void f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator_impl::perform(
    float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
    const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    const int n1 = (n >> 2) << 2; // (n / 4) * 4;
    const int n2 = (n & 0x3);     // n % 4

    if (n1 != 0) {
        perform_phase1(&src_dest[2 * 0], n1, b_, ia_, s_);
    }

    if (n2 != 0) {
        perform_phase2(&src_dest[2 * n1], n2, b_, ia_, s_);
    }
}

void f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator_impl::perform(
    const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
    const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_)
    CXXPH_NOEXCEPT
{

    const int n1 = (n >> 2) << 2; // (n / 4) * 4;
    const int n2 = (n & 0x3);     // n % 4

    if (n1 != 0) {
        perform_phase1(&src[2 * 0], &dest[2 * 0], n1, b_, ia_, s_);
    }

    if (n2 != 0) {
        perform_phase2(&src[2 * n1], &dest[2 * n1], n2, b_, ia_, s_);
    }
}

} // namespace impl
} // namespace filter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
