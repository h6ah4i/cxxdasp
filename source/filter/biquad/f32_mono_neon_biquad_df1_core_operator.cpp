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

#include <cxxdasp/filter/biquad/f32_mono_neon_biquad_df1_core_operator.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

// #include <cxxdasp/utils/neon_debug_utils.hpp>

namespace cxxdasp {
namespace filter {
namespace impl {

static inline float32x4_t load_b(const float32_t *b)
{
    const float32x4_t t = vld1q_f32(&b[0]);
    const float32x2_t h = vrev64_f32(vget_low_f32(t));
    const float32x2_t l = vrev64_f32(vget_high_f32(t));
    return vcombine_f32(l, h);
}

static inline float32x2_t load_ia(const float32_t *ia) { return vrev64_f32(vld1_f32(&ia[0])); }

static void perform_phase1(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT x_,
                           float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{
    const float32x4_t b = load_b(b_);
    const float32x2_t ia = load_ia(ia_);
    float32x4_t dx = vld1q_f32(&x_[0]);
    float32x2_t dy = vld1_f32(&y_[0]);

    for (int i = 0; i < n; i += 4) {
        const float32x4_t px = dx;
        float32x4_t x;
        float32x2x2_t y;
        float32x2_t t;

        x = vld1q_f32(&src_dest[i]);
        __builtin_prefetch(&src_dest[i + 4], 0, 1);

        // 0
        dx = vextq_f32(px, x, 1);
        t = vmul_f32(ia, dy);
        t = vmla_f32(t, vget_high_f32(b), vget_high_f32(dx));
        t = vmla_f32(t, vget_low_f32(b), vget_low_f32(dx));
        t = vpadd_f32(t, t);
        dy = vext_f32(dy, t, 1);

        // 1
        dx = vextq_f32(px, x, 2);
        t = vmul_f32(ia, dy);
        t = vmla_f32(t, vget_high_f32(b), vget_high_f32(dx));
        t = vmla_f32(t, vget_low_f32(b), vget_low_f32(dx));
        t = vpadd_f32(t, t);
        dy = vext_f32(dy, t, 1);

        y.val[0] = dy;

        // 2
        dx = vextq_f32(px, x, 3);
        t = vmul_f32(ia, dy);
        t = vmla_f32(t, vget_high_f32(b), vget_high_f32(dx));
        t = vmla_f32(t, vget_low_f32(b), vget_low_f32(dx));
        t = vpadd_f32(t, t);
        dy = vext_f32(dy, t, 1);

        // 3
        dx = x;
        t = vmul_f32(ia, dy);
        t = vmla_f32(t, vget_high_f32(b), vget_high_f32(dx));
        t = vmla_f32(t, vget_low_f32(b), vget_low_f32(dx));
        t = vpadd_f32(t, t);
        dy = vext_f32(dy, t, 1);

        y.val[1] = dy;

        vst1q_f32(&src_dest[i], vcombine_f32(y.val[0], y.val[1]));
    }

    vst1q_f32(&x_[0], dx);
    vst1_f32(&y_[0], dy);
}

static void perform_phase2(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT x_,
                           float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{
    const float b0 = b_[0];
    const float b1 = b_[1];
    const float b2 = b_[2];
    const float ia1 = ia_[0];
    const float ia2 = ia_[1];

    float x0 = x_[(4 - 1) - 0]; // reversed
    float x1 = x_[(4 - 1) - 1]; // reversed
    float y0 = y_[(2 - 1) - 0]; // reversed
    float y1 = y_[(2 - 1) - 1]; // reversed

    for (int i = 0; i < n; ++i) {
        float x2, y2;

        x2 = x1;
        x1 = x0;
        x0 = src_dest[i];

        y2 = y1;
        y1 = y0;
        y0 = (b0 * x0) + (b1 * x1) + (b2 * x2) + (ia1 * y1) + (ia2 * y2);

        src_dest[i] = y0;
    }

    x_[(4 - 1) - 0] = x0; // reversed
    x_[(4 - 1) - 1] = x1; // reversed
    y_[(2 - 1) - 0] = y0; // reversed
    y_[(2 - 1) - 1] = y1; // reversed
}

static void perform_phase1(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT x_, float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{
    const float32x4_t b = load_b(b_);
    const float32x2_t ia = load_ia(ia_);
    float32x4_t dx = vld1q_f32(&x_[0]);
    float32x2_t dy = vld1_f32(&y_[0]);

    for (int i = 0; i < n; i += 4) {
        const float32x4_t px = dx;
        float32x4_t x;
        float32x2x2_t y;
        float32x2_t t;

        x = vld1q_f32(&src[i]);
        __builtin_prefetch(&src[i + 4], 0, 1);

        // 0
        dx = vextq_f32(px, x, 1);
        t = vmul_f32(ia, dy);
        t = vmla_f32(t, vget_high_f32(b), vget_high_f32(dx));
        t = vmla_f32(t, vget_low_f32(b), vget_low_f32(dx));
        t = vpadd_f32(t, t);
        dy = vext_f32(dy, t, 1);

        // 1
        dx = vextq_f32(px, x, 2);
        t = vmul_f32(ia, dy);
        t = vmla_f32(t, vget_high_f32(b), vget_high_f32(dx));
        t = vmla_f32(t, vget_low_f32(b), vget_low_f32(dx));
        t = vpadd_f32(t, t);
        dy = vext_f32(dy, t, 1);

        y.val[0] = dy;

        // 2
        dx = vextq_f32(px, x, 3);
        t = vmul_f32(ia, dy);
        t = vmla_f32(t, vget_high_f32(b), vget_high_f32(dx));
        t = vmla_f32(t, vget_low_f32(b), vget_low_f32(dx));
        t = vpadd_f32(t, t);
        dy = vext_f32(dy, t, 1);

        // 3
        dx = x;
        t = vmul_f32(ia, dy);
        t = vmla_f32(t, vget_high_f32(b), vget_high_f32(dx));
        t = vmla_f32(t, vget_low_f32(b), vget_low_f32(dx));
        t = vpadd_f32(t, t);
        dy = vext_f32(dy, t, 1);

        y.val[1] = dy;

        vst1q_f32(&dest[i], vcombine_f32(y.val[0], y.val[1]));
        __builtin_prefetch(&dest[i + 4], 1, 1);
    }

    vst1q_f32(&x_[0], dx);
    vst1_f32(&y_[0], dy);
}

static void perform_phase2(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT x_, float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{
    const float b0 = b_[0];
    const float b1 = b_[1];
    const float b2 = b_[2];
    const float ia1 = ia_[0];
    const float ia2 = ia_[1];

    float x0 = x_[(4 - 1) - 0]; // reversed
    float x1 = x_[(4 - 1) - 1]; // reversed
    float y0 = y_[(2 - 1) - 0]; // reversed
    float y1 = y_[(2 - 1) - 1]; // reversed

    for (int i = 0; i < n; ++i) {
        float x2, y2;

        x2 = x1;
        x1 = x0;
        x0 = src[i];

        y2 = y1;
        y1 = y0;
        y0 = (b0 * x0) + (b1 * x1) + (b2 * x2) + (ia1 * y1) + (ia2 * y2);

        dest[i] = y0;
    }

    x_[(4 - 1) - 0] = x0; // reversed
    x_[(4 - 1) - 1] = x1; // reversed
    y_[(2 - 1) - 0] = y0; // reversed
    y_[(2 - 1) - 1] = y1; // reversed
}

void f32_mono_neon_biquad_direct_form_1_core_operator_impl::perform(float32_t *CXXPH_RESTRICT src_dest, unsigned int n,
                                                                    const float32_t *CXXPH_RESTRICT b_,
                                                                    const float32_t *CXXPH_RESTRICT ia_,
                                                                    float32_t *CXXPH_RESTRICT x_,
                                                                    float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{

    const int n1 = (n >> 2) << 2; // (n / 4) * 4;
    const int n2 = (n & 0x3);     // n % 4

    if (n1 != 0) {
        perform_phase1(&src_dest[0], n1, b_, ia_, x_, y_);
    }

    if (n2 != 0) {
        perform_phase2(&src_dest[n1], n2, b_, ia_, x_, y_);
    }
}

void f32_mono_neon_biquad_direct_form_1_core_operator_impl::perform(const float32_t *CXXPH_RESTRICT src,
                                                                    float32_t *CXXPH_RESTRICT dest, unsigned int n,
                                                                    const float32_t *CXXPH_RESTRICT b_,
                                                                    const float32_t *CXXPH_RESTRICT ia_,
                                                                    float32_t *CXXPH_RESTRICT x_,
                                                                    float32_t *CXXPH_RESTRICT y_) CXXPH_NOEXCEPT
{

    const int n1 = (n >> 2) << 2; // (n / 4) * 4;
    const int n2 = (n & 0x3);     // n % 4

    if (n1 != 0) {
        perform_phase1(&src[0], &dest[0], n1, b_, ia_, x_, y_);
    }

    if (n2 != 0) {
        perform_phase2(&src[n1], &dest[n1], n2, b_, ia_, x_, y_);
    }
}

} // namespace impl
} // namespace filter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
