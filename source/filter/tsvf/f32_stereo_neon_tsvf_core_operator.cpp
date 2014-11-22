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

#include <cxxdasp/filter/tsvf/f32_stereo_neon_tsvf_core_operator.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

namespace cxxdasp {
namespace filter {
namespace impl {

void f32_stereo_neon_tsvf_core_operator_impl::perform(float32_t *CXXPH_RESTRICT src_dest, unsigned int n,
                                                      float32x2x2_t *CXXPH_RESTRICT pic12eq,
                                                      const float32x4x3_t *CXXPH_RESTRICT pcoeffs) CXXPH_NOEXCEPT
{

    if (n == 0)
        return;

    const float32x4_t c0 = (*pcoeffs).val[0];
    const float32x4_t c1 = (*pcoeffs).val[1];
    const float32x4_t c2 = (*pcoeffs).val[2];

    float32x2x2_t ic12eq = (*pic12eq);
    float32x4_t t0, t1;

    while (n--) {
        const float32x2_t v0 = vld1_f32(&src_dest[0]);
        t0 = vmulq_lane_f32(c2, ic12eq.val[0], 1);
        t1 = vmulq_lane_f32(c2, ic12eq.val[1], 1);
        __builtin_prefetch(&src_dest[16], 0, 1);
        t0 = vmlaq_lane_f32_compat(t0, c1, ic12eq.val[0], 0);
        t1 = vmlaq_lane_f32_compat(t1, c1, ic12eq.val[1], 0);
        t0 = vmlaq_lane_f32_compat(t0, c0, v0, 0);
        t1 = vmlaq_lane_f32_compat(t1, c0, v0, 1);

        ic12eq.val[0] = vsub_f32(vget_low_f32(t0), ic12eq.val[0]);
        ic12eq.val[1] = vsub_f32(vget_low_f32(t1), ic12eq.val[1]);

        const float32x2_t t = vext_f32(vget_high_f32(t0), vget_high_f32(t1), 1);
        vst1_f32(&src_dest[0], t);
        src_dest += 2;
    }

    (*pic12eq) = ic12eq;
}

void f32_stereo_neon_tsvf_core_operator_impl::perform(const float32_t *CXXPH_RESTRICT src,
                                                      float32_t *CXXPH_RESTRICT dest, unsigned int n,
                                                      float32x2x2_t *CXXPH_RESTRICT pic12eq,
                                                      const float32x4x3_t *CXXPH_RESTRICT pcoeffs) CXXPH_NOEXCEPT
{

    if (n == 0)
        return;

    const float32x4_t c0 = (*pcoeffs).val[0];
    const float32x4_t c1 = (*pcoeffs).val[1];
    const float32x4_t c2 = (*pcoeffs).val[2];

    float32x2x2_t ic12eq = (*pic12eq);
    float32x4_t t0, t1;

    while (n--) {
        const float32x2_t v0 = vld1_f32(&src[0]);
        src += 2;
        t0 = vmulq_lane_f32(c2, ic12eq.val[0], 1);
        t1 = vmulq_lane_f32(c2, ic12eq.val[1], 1);
        __builtin_prefetch(&src[16], 0, 1);
        t0 = vmlaq_lane_f32_compat(t0, c1, ic12eq.val[0], 0);
        t1 = vmlaq_lane_f32_compat(t1, c1, ic12eq.val[1], 0);
        __builtin_prefetch(&dest[16], 1, 1);
        t0 = vmlaq_lane_f32_compat(t0, c0, v0, 0);
        t1 = vmlaq_lane_f32_compat(t1, c0, v0, 1);

        ic12eq.val[0] = vsub_f32(vget_low_f32(t0), ic12eq.val[0]);
        ic12eq.val[1] = vsub_f32(vget_low_f32(t1), ic12eq.val[1]);

        const float32x2_t t = vext_f32(vget_high_f32(t0), vget_high_f32(t1), 1);
        vst1_f32(&dest[0], t);
        dest += 2;
    }

    (*pic12eq) = ic12eq;
}

} // namespace impl
} // namespace filter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
