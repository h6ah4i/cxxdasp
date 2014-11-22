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

#ifndef CXXDASP_RESAMPLER_POLYPHASE_F32_STEREO_SSE_POLYPHASE_CORE_OPERATOR_HPP_
#define CXXDASP_RESAMPLER_POLYPHASE_F32_STEREO_SSE_POLYPHASE_CORE_OPERATOR_HPP_

#include <cxxporthelper/compiler.hpp>

#if CXXPH_COMPILER_SUPPORTS_X86_SSE

#include <cxxporthelper/cstdint>
#include <cxxporthelper/x86_intrinsics.hpp>
#include <cxxporthelper/platform_info.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace resampler {

/**
 * Audio frame operator (SSE optimized)
 *
 * source & dest: float32, 2 ch
 */
class f32_stereo_sse_polyphase_core_operator {

    /// @cond INTERNAL_FIELD
    f32_stereo_sse_polyphase_core_operator(const f32_stereo_sse_polyphase_core_operator &) = delete;
    f32_stereo_sse_polyphase_core_operator &operator=(const f32_stereo_sse_polyphase_core_operator &) = delete;
    /// @endcond

public:
    /** Data type of source audio frame */
    typedef datatype::audio_frame<float, 2> src_frame_t;

    /** Data type of destination audio frame */
    typedef datatype::audio_frame<float, 2> dest_frame_t;

    /** Data type of FIR coefficients */
    typedef float coeffs_t;

/* Number of channels */
#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
    static constexpr int num_channels = 2;
#else
    enum { num_channels = 2 };
#endif

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    static bool is_supported() { return cxxporthelper::platform_info::support_sse(); }

    /**
     * Constructor.
     */
    f32_stereo_sse_polyphase_core_operator() {}

    /**
     * Destructor.
     */
    ~f32_stereo_sse_polyphase_core_operator() {}

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

        if (n == 0) {
            return;
        }

        int n_loop_1 = (reinterpret_cast<uintptr_t>(src) % 16) / (sizeof(float) * 2);
        int n_loop_2 = ((n - n_loop_1) >> 2);   // ((n - n_loop_1) / 4)
        int n_loop_3 = ((n - n_loop_1) & 0x03); // ((n - n_loop_1) % 4)

        if (n_loop_1 > 0) {
            dest1[0] = src[0];
            dest2[0] = src[0];
        }

        if (CXXPH_LIKELY(n_loop_2 > 0)) {
            float *CXXPH_RESTRICT nc_dest1 = reinterpret_cast<float *>(const_cast<src_frame_t *>(&dest1[n_loop_1]));
            float *CXXPH_RESTRICT nc_dest2 = reinterpret_cast<float *>(const_cast<src_frame_t *>(&dest2[n_loop_1]));
            float *CXXPH_RESTRICT nc_src = reinterpret_cast<float *>(const_cast<src_frame_t *>(&src[n_loop_1]));

            for (int i = 0; i < n_loop_2; ++i) {
                const __m128 s0 = _mm_load_ps(&nc_src[i * 8 + 0]);
                const __m128 s1 = _mm_load_ps(&nc_src[i * 8 + 4]);

                _mm_storeu_ps(&nc_dest1[i * 8 + 0], s0);
                _mm_storeu_ps(&nc_dest2[i * 8 + 0], s0);
                _mm_storeu_ps(&nc_dest1[i * 8 + 4], s1);
                _mm_storeu_ps(&nc_dest2[i * 8 + 4], s1);
            }
        }

        if (n_loop_3 > 0) {
            for (int i = n_loop_1 + (n_loop_2 * 4); i < n; ++i) {
                dest1[i] = src[i];
                dest2[i] = src[i];
            }
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
    void convolve(dest_frame_t *CXXPH_RESTRICT dest, const src_frame_t *CXXPH_RESTRICT samples,
                  const coeffs_t *CXXPH_RESTRICT coeffs, int n) const CXXPH_NOEXCEPT
    {

        int n_loop_1 = (n >> 2);   // (n / 4)
        int n_loop_2 = (n & 0x03); // (n % 4)

        dest_frame_t sum(0.0f);

        if (CXXPH_LIKELY(n_loop_1 > 0)) {
            float *CXXPH_RESTRICT nc_samples = reinterpret_cast<float *>(const_cast<src_frame_t *>(samples));
            float *CXXPH_RESTRICT nc_coeffs = const_cast<float *>(coeffs);

            CXXPH_ALIGNAS(16) float tmp[4];

            __m128 t0 = _mm_setzero_ps();
            __m128 t1 = _mm_setzero_ps();

            for (int i = 0; i < n_loop_1; ++i) {
                const __m128 c = _mm_load_ps(&nc_coeffs[i * 4 + 0]);
                const __m128 c0 = _mm_shuffle_ps(c, c, _MM_SHUFFLE(1, 1, 0, 0));
                const __m128 c1 = _mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 3, 2, 2));
                const __m128 s0 = _mm_loadu_ps(&nc_samples[i * 8 + 0]);
                const __m128 s1 = _mm_loadu_ps(&nc_samples[i * 8 + 4]);

                const __m128 m0 = _mm_mul_ps(s0, c0);
                const __m128 m1 = _mm_mul_ps(s1, c1);

                t0 = _mm_add_ps(t0, m0);
                t1 = _mm_add_ps(t1, m1);
            }

            t0 = _mm_add_ps(t0, t1);
            _mm_store_ps(&tmp[0], t0);

            sum.c(0) = (tmp[0] + tmp[2]);
            sum.c(1) = (tmp[1] + tmp[3]);
        }

        if (n_loop_2 > 0) {
            const src_frame_t *CXXPH_RESTRICT s = &samples[n_loop_1 * 4];
            const float *CXXPH_RESTRICT c = &coeffs[n_loop_1 * 4];

            for (int i = 0; i < n_loop_2; ++i) {
                sum.c(0) += (s[i].c(0) * c[i]);
                sum.c(1) += (s[i].c(1) * c[i]);
            }
        }

        (*dest) = sum;
    }
};

} // namespace resampler
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_X86_SSE
#endif // CXXDASP_RESAMPLER_POLYPHASE_F32_STEREO_SSE_POLYPHASE_CORE_OPERATOR_HPP_
