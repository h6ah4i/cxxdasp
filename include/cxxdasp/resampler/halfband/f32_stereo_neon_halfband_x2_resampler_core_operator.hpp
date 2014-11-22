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

#ifndef CXXDASP_RESAMPLER_F32_STEREO_NEON_HALFBAND_X2_RESAMPLER_CORE_OPERATOR_HPP_
#define CXXDASP_RESAMPLER_F32_STEREO_NEON_HALFBAND_X2_RESAMPLER_CORE_OPERATOR_HPP_

#include <cxxporthelper/compiler.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

#include <cxxporthelper/cstdint>
#include <cxxporthelper/arm_neon.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>
#include <cxxdasp/utils/utils.hpp>

namespace cxxdasp {
namespace resampler {

class f32_stereo_neon_halfband_x2_resampler_core_operator {
public:
    /**
     * Source audio frame type.
     */
    typedef datatype::f32_stereo_frame_t src_frame_t;

    /**
     * Destination audio frame type.
     */
    typedef datatype::f32_stereo_frame_t dest_frame_t;

    /**
     * FIR coefficients type.
     */
    typedef float coeffs_t;

/** Number of channels */
#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
    static constexpr int num_channels = 2;
#else
    enum { num_channels = 2 };
#endif

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    static bool is_supported() CXXPH_NOEXCEPT { return cxxporthelper::platform_info::support_arm_neon(); }

    void dual_convolve(dest_frame_t *CXXPH_RESTRICT dest, const src_frame_t *CXXPH_RESTRICT src1,
                       const src_frame_t *CXXPH_RESTRICT src2, const coeffs_t *CXXPH_RESTRICT coeffs1,
                       const coeffs_t *CXXPH_RESTRICT coeffs2, int n) const CXXPH_NOEXCEPT
    {

        const float32_t *CXXPH_RESTRICT f32_src1 = reinterpret_cast<const float32_t *>(src1);
        const float32_t *CXXPH_RESTRICT f32_src2 = reinterpret_cast<const float32_t *>(src2);
        const float32_t *CXXPH_RESTRICT f32_coeffs1 =
            reinterpret_cast<const float32_t *>(utils::assume_aligned(coeffs1, CXXPH_PLATFORM_SIMD_ALIGNMENT));
        const float32_t *CXXPH_RESTRICT f32_coeffs2 =
            reinterpret_cast<const float32_t *>(utils::assume_aligned(coeffs2, CXXPH_PLATFORM_SIMD_ALIGNMENT));

        float32x4_t tl, tr;

        tl = vdupq_n_f32(0.0f);
        tr = vdupq_n_f32(0.0f);

        assert((n & 0x3) == 0);
        int n2 = (n >> 2);

        do {
            const float32x4_t c1 = vld1q_f32(f32_coeffs1);
            f32_coeffs1 += 4;
            const float32x4x2_t s1 = vld2q_f32(f32_src1);
            f32_src1 += (2 * 4);

            tl = vmlaq_f32(tl, c1, s1.val[0]);
            tr = vmlaq_f32(tr, c1, s1.val[1]);

            const float32x4_t c2 = vld1q_f32(f32_coeffs2);
            f32_coeffs2 += 4;
            const float32x4x2_t s2 = vld2q_f32(f32_src2);
            f32_src2 += (2 * 4);

            tl = vmlaq_f32(tl, c2, s2.val[0]);
            tr = vmlaq_f32(tr, c2, s2.val[1]);

            --n2;
        } while (CXXPH_LIKELY(n2 != 0));

        const float32x2_t tl2 = vadd_f32(vget_low_f32(tl), vget_high_f32(tl));
        const float32x2_t tr2 = vadd_f32(vget_low_f32(tr), vget_high_f32(tr));

        const float32x2_t t = vpadd_f32(tl2, tr2);

        vst1_f32(reinterpret_cast<float32_t *>(dest), t);
    }
};

} // namespace resampler
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
#endif // CXXDASP_RESAMPLER_F32_STEREO_NEON_HALFBAND_X2_RESAMPLER_CORE_OPERATOR_HPP_
