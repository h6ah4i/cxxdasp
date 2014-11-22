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

#ifndef CXXDASP_RESAMPLER_GENERAL_HALFBAND_X2_RESAMPLER_CORE_OPERATOR_HPP_
#define CXXDASP_RESAMPLER_GENERAL_HALFBAND_X2_RESAMPLER_CORE_OPERATOR_HPP_

#include <cxxporthelper/cstdint>
#include <cxxporthelper/compiler.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>
#include <cxxdasp/utils/utils.hpp>

namespace cxxdasp {
namespace resampler {

/**
 * Poly-phase FIR based resampler class.
 *
 * @tparam TSrc source audio frame type
 * @tparam TDest destination audio frame type
 * @tparam TCoeffs FIR coefficient data type
 * @tparam NChannel num channels
 */
template <typename TSrc, typename TDest, typename TCoeffs, int NChannels>
class general_halfband_x2_resampler_core_operator {
public:
    /**
     * Source audio frame type.
     */
    typedef datatype::audio_frame<TSrc, NChannels> src_frame_t;

    /**
     * Destination audio frame type.
     */
    typedef datatype::audio_frame<TDest, NChannels> dest_frame_t;

    /**
     * FIR coefficients type.
     */
    typedef TCoeffs coeffs_t;

/** Number of channels */
#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
    static constexpr int num_channels = NChannels;
#else
    enum { num_channels = NChannels };
#endif

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    CXXPH_OPTIONAL_CONSTEXPR static bool is_supported() CXXPH_NOEXCEPT { return true; }

    void dual_convolve(dest_frame_t *CXXPH_RESTRICT dest, const src_frame_t *CXXPH_RESTRICT src1,
                       const src_frame_t *CXXPH_RESTRICT src2, const coeffs_t *CXXPH_RESTRICT coeffs1,
                       const coeffs_t *CXXPH_RESTRICT coeffs2, int n) const CXXPH_NOEXCEPT
    {

        CXXDASP_UTIL_ASSUME_ALIGNED(coeffs1, CXXPH_PLATFORM_SIMD_ALIGNMENT);
        CXXDASP_UTIL_ASSUME_ALIGNED(coeffs2, CXXPH_PLATFORM_SIMD_ALIGNMENT);

        dest_frame_t t1(0);
        dest_frame_t t2(0);

        for (int i = 0; i < n; ++i) {
            t1 += src1[i] * coeffs1[i];
            t2 += src2[i] * coeffs2[i];
        }

        (*dest) = (t1 + t2);
    }
};

// Well-known forms
typedef general_halfband_x2_resampler_core_operator<float, float, float, 1>
f32_mono_basic_halfband_x2_resampler_core_operator;
typedef general_halfband_x2_resampler_core_operator<float, float, float, 2>
f32_stereo_basic_halfband_x2_resampler_core_operator;

typedef general_halfband_x2_resampler_core_operator<double, double, float, 1>
f64_mono_basic_halfband_x2_resampler_core_operator;
typedef general_halfband_x2_resampler_core_operator<double, double, float, 2>
f64_stereo_basic_halfband_x2_resampler_core_operator;

} // namespace resampler
} // namespace cxxdasp

#endif // CXXDASP_RESAMPLER_GENERAL_HALFBAND_X2_RESAMPLER_CORE_OPERATOR_HPP_
