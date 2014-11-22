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

#ifndef CXXDASP_RESAMPLER_POLYPHASE_F32_MONO_SSE3_POLYPHASE_CORE_OPERATOR_HPP_
#define CXXDASP_RESAMPLER_POLYPHASE_F32_MONO_SSE3_POLYPHASE_CORE_OPERATOR_HPP_

#include <cxxporthelper/compiler.hpp>

#if CXXPH_COMPILER_SUPPORTS_X86_SSE && CXXPH_COMPILER_SUPPORTS_X86_SSE3

#include <cxxporthelper/x86_intrinsics.hpp>
#include <cxxporthelper/platform_info.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace resampler {

/**
 * Audio frame operator (SSE optimized)
 *
 * source & dest: float32, 1 ch
 */
class f32_mono_sse3_polyphase_core_operator {

    /// @cond INTERNAL_FIELD
    f32_mono_sse3_polyphase_core_operator(const f32_mono_sse3_polyphase_core_operator &) = delete;
    f32_mono_sse3_polyphase_core_operator &operator=(const f32_mono_sse3_polyphase_core_operator &) = delete;
    /// @endcond

public:
    /** Data type of source audio frame */
    typedef datatype::audio_frame<float, 1> src_frame_t;

    /** Data type of destination audio frame */
    typedef datatype::audio_frame<float, 1> dest_frame_t;

    /** Data type of FIR coefficients */
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
    static bool is_supported()
    {
        return cxxporthelper::platform_info::support_sse() && cxxporthelper::platform_info::support_sse3();
    }

    /**
     * Constructor.
     */
    f32_mono_sse3_polyphase_core_operator() {}

    /**
     * Destructor.
     */
    ~f32_mono_sse3_polyphase_core_operator() {}

    /**
     * Copy multiple frames to dual destinations.
     *
     * @param [out] dest1 pointer of first destination frames
     * @param [out] dest2 pointer of second destination frames
     * @param [in] src pointer of source frames
     * @param [in] n copy length [frame]
     */
    void dual_copy(src_frame_t *CXXPH_RESTRICT dest1, src_frame_t *CXXPH_RESTRICT dest2,
                   const src_frame_t *CXXPH_RESTRICT src, int n) const CXXPH_NOEXCEPT
    {

        for (int i = 0; i < n; ++i) {
            dest1[i] = src[i];
            dest2[i] = src[i];
        }
    }

    /**
     * Calculate convolution.
     *
     * @param [out] dest pointer of destination frame
     * @param [in] samples pointer of souurce samples frames
     * @param [in] coeffs pointer of coefficients array frames
     * @param [in] n length [frame]
     */
    void convolve(src_frame_t *CXXPH_RESTRICT dest, const dest_frame_t *CXXPH_RESTRICT samples,
                  const float *CXXPH_RESTRICT coeffs, int n) const CXXPH_NOEXCEPT
    {
        int n_loop_1 = (n >> 3);   // (n / 8)
        int n_loop_2 = (n & 0x07); // (n % 8)

        dest_frame_t sum(0.0f);

        if (CXXPH_LIKELY(n_loop_1 > 0)) {
            float *CXXPH_RESTRICT nc_samples = reinterpret_cast<float *>(const_cast<src_frame_t *>(samples));
            float *CXXPH_RESTRICT nc_coeffs = const_cast<float *>(coeffs);

            __m128 t0 = _mm_setzero_ps();
            __m128 t1 = _mm_setzero_ps();

            for (int i = 0; i < n_loop_1; ++i) {
                const __m128 c0 = _mm_load_ps(&nc_coeffs[i * 8 + 0]);
                const __m128 c1 = _mm_load_ps(&nc_coeffs[i * 8 + 4]);
                const __m128 s0 = _mm_loadu_ps(&nc_samples[i * 8 + 0]);
                const __m128 s1 = _mm_loadu_ps(&nc_samples[i * 8 + 4]);

                const __m128 m0 = _mm_mul_ps(s0, c0);
                const __m128 m1 = _mm_mul_ps(s1, c1);

                t0 = _mm_add_ps(t0, m0);
                t1 = _mm_add_ps(t1, m1);
            }

            sum.c(0) = mm_hadd_all_ps(_mm_add_ps(t0, t1));
        }

        if (n_loop_2 > 0) {
            const src_frame_t *CXXPH_RESTRICT s = &samples[n_loop_1 * 8];
            const float *CXXPH_RESTRICT c = &coeffs[n_loop_1 * 8];

            for (int i = 0; i < n_loop_2; ++i) {
                sum.c(0) += (s[i].c(0) * c[i]);
            }
        }

        (*dest) = sum;
    }

private:
    static float mm_hadd_all_ps(const __m128 &m) CXXPH_NOEXCEPT
    {
        __m128 sum;
        sum = _mm_hadd_ps(m, m);
        sum = _mm_hadd_ps(sum, sum);
        float tmp;
        _mm_store_ss(&tmp, sum);
        return tmp;
    }
};

} // namespace resampler
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_X86_SSE && CXXPH_COMPILER_SUPPORTS_X86_SSE3
#endif // CXXDASP_RESAMPLER_POLYPHASE_F32_MONO_SSE3_POLYPHASE_CORE_OPERATOR_HPP_
