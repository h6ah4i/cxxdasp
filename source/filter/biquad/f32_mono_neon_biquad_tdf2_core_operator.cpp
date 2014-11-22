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

#include <cxxdasp/filter/biquad/f32_mono_neon_biquad_tdf2_core_operator.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

namespace cxxdasp {
namespace filter {
namespace impl {

#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM
static void perform_phase1(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{
    const float32x4_t b = vld1q_f32(&b_[0]);
    const float32x4_t ia = vcombine_f32(vld1_f32(&ia_[0]), vdup_n_f32(0.0f));
    const float32x4_t z4 = vdupq_n_f32(0.0f);

    float32x4_t s = vcombine_f32(vld1_f32(&s_[0]), vdup_n_f32(0.0f));

    for (int i = 0; i < n; i += 4) {
        float32x4_t x;
        float32x4_t y;

        x = vld1q_f32(&src_dest[i]);
        __builtin_prefetch(&src_dest[i + 4], 0, 1);

        // 0
        s = vmlaq_lane_f32_compat(s, b, vget_low_f32(x), 0);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_compat(s, ia, vget_high_f32(y), 1);

        // 1
        s = vmlaq_lane_f32_compat(s, b, vget_low_f32(x), 1);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_compat(s, ia, vget_high_f32(y), 1);

        // 2
        s = vmlaq_lane_f32_compat(s, b, vget_high_f32(x), 0);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_compat(s, ia, vget_high_f32(y), 1);

        // 3
        s = vmlaq_lane_f32_compat(s, b, vget_high_f32(x), 1);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_compat(s, ia, vget_high_f32(y), 1);

        vst1q_f32(&src_dest[i], y);
    }

    vst1_f32(&s_[0], vget_low_f32(s));
}

static void perform_phase1(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{
    const float32x4_t b = vld1q_f32(&b_[0]);
    const float32x4_t ia = vcombine_f32(vld1_f32(&ia_[0]), vdup_n_f32(0.0f));
    const float32x4_t z4 = vdupq_n_f32(0.0f);

    float32x4_t s = vcombine_f32(vld1_f32(&s_[0]), vdup_n_f32(0.0f));

    for (int i = 0; i < n; i += 4) {
        float32x4_t x;
        float32x4_t y;

        x = vld1q_f32(&src[i]);
        __builtin_prefetch(&src[i + 4], 0, 1);

        // 0
        s = vmlaq_lane_f32_compat(s, b, vget_low_f32(x), 0);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_compat(s, ia, vget_high_f32(y), 1);

        // 1
        s = vmlaq_lane_f32_compat(s, b, vget_low_f32(x), 1);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_compat(s, ia, vget_high_f32(y), 1);

        // 2
        s = vmlaq_lane_f32_compat(s, b, vget_high_f32(x), 0);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_compat(s, ia, vget_high_f32(y), 1);

        // 3
        s = vmlaq_lane_f32_compat(s, b, vget_high_f32(x), 1);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_compat(s, ia, vget_high_f32(y), 1);

        vst1q_f32(&dest[i], y);
        __builtin_prefetch(&dest[i + 4], 1, 1);
    }

    vst1_f32(&s_[0], vget_low_f32(s));
}

#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64

static void perform_phase1(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{
    const float32x4_t b = vld1q_f32(&b_[0]);
    const float32x4_t ia = vcombine_f32(vld1_f32(&ia_[0]), vdup_n_f32(0.0f));
    const float32x4_t z4 = vdupq_n_f32(0.0f);

    float32x4_t s = vcombine_f32(vld1_f32(&s_[0]), vdup_n_f32(0.0f));

    for (int i = 0; i < n; i += 4) {
        float32x4_t x;
        float32x4_t y;

        x = vld1q_f32(&src_dest[i]);
        __builtin_prefetch(&src_dest[i + 4], 0, 1);

        // 0
        s = vmlaq_lane_f32_ex(s, b, x, 0);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_ex(s, ia, y, 3);

        // 1
        s = vmlaq_lane_f32_ex(s, b, x, 1);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_ex(s, ia, y, 3);

        // 2
        s = vmlaq_lane_f32_ex(s, b, x, 2);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_ex(s, ia, y, 3);

        // 3
        s = vmlaq_lane_f32_ex(s, b, x, 3);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_ex(s, ia, y, 3);

        vst1q_f32(&src_dest[i], y);
    }

    vst1_f32(&s_[0], vget_low_f32(s));
}

static void perform_phase1(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{
    const float32x4_t b = vld1q_f32(&b_[0]);
    const float32x4_t ia = vcombine_f32(vld1_f32(&ia_[0]), vdup_n_f32(0.0f));
    const float32x4_t z4 = vdupq_n_f32(0.0f);

    float32x4_t s = vcombine_f32(vld1_f32(&s_[0]), vdup_n_f32(0.0f));

    for (int i = 0; i < n; i += 4) {
        float32x4_t x;
        float32x4_t y;

        x = vld1q_f32(&src[i]);
        __builtin_prefetch(&src[i + 4], 0, 1);

        // 0
        s = vmlaq_lane_f32_ex(s, b, x, 0);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_ex(s, ia, y, 3);

        // 1
        s = vmlaq_lane_f32_ex(s, b, x, 1);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_ex(s, ia, y, 3);

        // 2
        s = vmlaq_lane_f32_ex(s, b, x, 2);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_ex(s, ia, y, 3);

        // 3
        s = vmlaq_lane_f32_ex(s, b, x, 3);
        y = vextq_f32(y, s, 1);
        s = vextq_f32(s, z4, 1);
        s = vmlaq_lane_f32_ex(s, ia, y, 3);

        vst1q_f32(&dest[i], y);
        __builtin_prefetch(&dest[i + 4], 1, 1);
    }

    vst1_f32(&s_[0], vget_low_f32(s));
}

#endif

static void perform_phase2(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{
    const float b0 = b_[0];
    const float b1 = b_[1];
    const float b2 = b_[2];
    const float ia1 = ia_[0];
    const float ia2 = ia_[1];

    float s1 = s_[0];
    float s2 = s_[1];

    for (int i = 0; i < n; ++i) {
        const float x = src_dest[i];
        const float y = s1 + (b0 * x);
        s1 = s2 + (ia1 * y) + (b1 * x);
        s2 = (ia2 * y) + (b2 * x);
        src_dest[i] = y;
    }

    s_[0] = s1;
    s_[1] = s2;
}

static void perform_phase2(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{
    const float b0 = b_[0];
    const float b1 = b_[1];
    const float b2 = b_[2];
    const float ia1 = ia_[0];
    const float ia2 = ia_[1];

    float s1 = s_[0];
    float s2 = s_[1];

    for (int i = 0; i < n; ++i) {
        const float x = src[i];
        const float y = s1 + (b0 * x);
        s1 = s2 + (ia1 * y) + (b1 * x);
        s2 = (ia2 * y) + (b2 * x);
        dest[i] = y;
    }

    s_[0] = s1;
    s_[1] = s2;
}

void f32_mono_neon_biquad_transposed_direct_form_2_core_operator_impl::perform(
    float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
    const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    const int n1 = (n >> 2) << 2; // (n / 4) * 4;
    const int n2 = (n & 0x3);     // n % 4

    if (n1 != 0) {
        perform_phase1(&src_dest[0], n1, b_, ia_, s_);
    }

    if (n2 != 0) {
        perform_phase2(&src_dest[n1], n2, b_, ia_, s_);
    }
}

void f32_mono_neon_biquad_transposed_direct_form_2_core_operator_impl::perform(
    const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
    const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_)
    CXXPH_NOEXCEPT
{

    const int n1 = (n >> 2) << 2; // (n / 4) * 4;
    const int n2 = (n & 0x3);     // n % 4

    if (n1 != 0) {
        perform_phase1(&src[0], &dest[0], n1, b_, ia_, s_);
    }

    if (n2 != 0) {
        perform_phase2(&src[n1], &dest[n1], n2, b_, ia_, s_);
    }
}

} // namespace impl
} // namespace filter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
