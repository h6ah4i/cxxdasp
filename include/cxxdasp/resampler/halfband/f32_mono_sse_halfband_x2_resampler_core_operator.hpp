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

#ifndef CXXDASP_RESAMPLER_F32_MONO_SSE_HALFBAND_X2_RESAMPLER_CORE_OPERATOR_HPP_
#define CXXDASP_RESAMPLER_F32_MONO_SSE_HALFBAND_X2_RESAMPLER_CORE_OPERATOR_HPP_

#include <cxxporthelper/compiler.hpp>

#if CXXPH_COMPILER_SUPPORTS_X86_SSE

#include <cxxporthelper/cstdint>
#include <cxxporthelper/x86_intrinsics.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>
#include <cxxdasp/utils/utils.hpp>

namespace cxxdasp {
namespace resampler {

class f32_mono_sse_halfband_x2_resampler_core_operator {
public:
    /**
     * Source audio frame type.
     */
    typedef datatype::f32_mono_frame_t src_frame_t;

    /**
     * Destination audio frame type.
     */
    typedef datatype::f32_mono_frame_t dest_frame_t;

    /**
     * FIR coefficients type.
     */
    typedef float coeffs_t;

/** Number of channels */
#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
    static constexpr int num_channels = 1;
#else
    enum { num_channels = 1 };
#endif

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    static bool is_supported() CXXPH_NOEXCEPT { return cxxporthelper::platform_info::support_sse(); }

    void dual_convolve(dest_frame_t *CXXPH_RESTRICT dest, const src_frame_t *CXXPH_RESTRICT src1,
                       const src_frame_t *CXXPH_RESTRICT src2, const coeffs_t *CXXPH_RESTRICT coeffs1,
                       const coeffs_t *CXXPH_RESTRICT coeffs2, int n) const CXXPH_NOEXCEPT
    {

        const float *CXXPH_RESTRICT f32_src1 = reinterpret_cast<const float *>(src1);
        const float *CXXPH_RESTRICT f32_src2 = reinterpret_cast<const float *>(src2);
        const float *CXXPH_RESTRICT f32_coeffs1 =
            reinterpret_cast<const float *>(utils::assume_aligned(coeffs1, CXXPH_PLATFORM_SIMD_ALIGNMENT));
        const float *CXXPH_RESTRICT f32_coeffs2 =
            reinterpret_cast<const float *>(utils::assume_aligned(coeffs2, CXXPH_PLATFORM_SIMD_ALIGNMENT));

        __m128 t1, t2;

        t1 = _mm_setzero_ps();
        t2 = _mm_setzero_ps();

        assert((n & 0x3) == 0);
        for (int i = 0; i < n; i += 4) {
            const __m128 c1 = _mm_load_ps(&f32_coeffs1[i]);
            const __m128 s1 = _mm_loadu_ps(&f32_src1[i]);

            t1 = _mm_add_ps(t1, _mm_mul_ps(c1, s1));

            const __m128 c2 = _mm_load_ps(&f32_coeffs2[i]);
            const __m128 s2 = _mm_loadu_ps(&f32_src2[i]);

            t2 = _mm_add_ps(t2, _mm_mul_ps(c2, s2));
        }

        (*dest) = mm_hadd_all_ps(_mm_add_ps(t1, t2));
    }

private:
    static float mm_hadd_all_ps(const __m128 &m) CXXPH_NOEXCEPT
    {
        CXXPH_ALIGNAS(16) float tmp[4];
        _mm_store_ps(&tmp[0], m);
        return (tmp[0] + tmp[1] + tmp[2] + tmp[3]);
    }
};

} // namespace resampler
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_X86_SSE
#endif // CXXDASP_RESAMPLER_F32_MONO_SSE_HALFBAND_X2_RESAMPLER_CORE_OPERATOR_HPP_
