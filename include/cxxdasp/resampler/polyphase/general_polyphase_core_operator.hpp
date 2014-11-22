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

#ifndef CXXDASP_RESAMPLER_POLYPHASE_GENERAL_POLYPHASE_CORE_OPERATOR_HPP_
#define CXXDASP_RESAMPLER_POLYPHASE_GENERAL_POLYPHASE_CORE_OPERATOR_HPP_

#include <cxxporthelper/compiler.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>

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
template <typename TSrcData, typename TDestData, typename TCoeffs, int NChannels>
class general_polyphase_core_operator {

    /// @cond INTERNAL_FIELD
    general_polyphase_core_operator(const general_polyphase_core_operator &) = delete;
    general_polyphase_core_operator &operator=(const general_polyphase_core_operator &) = delete;
    /// @endcond

public:
    /** Data type of source audio frame */
    typedef datatype::audio_frame<TSrcData, NChannels> src_frame_t;

    /** Data type of destination audio frame */
    typedef datatype::audio_frame<TDestData, NChannels> dest_frame_t;

    /** Data type of FIR coefficients */
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
    static CXXPH_OPTIONAL_CONSTEXPR bool is_supported() { return true; }

    /**
     * Constructor.
     */
    general_polyphase_core_operator() {}

    /**
     * Destructor.
     */
    ~general_polyphase_core_operator() {}

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
    void convolve(dest_frame_t *CXXPH_RESTRICT dest, const src_frame_t *CXXPH_RESTRICT samples,
                  const coeffs_t *CXXPH_RESTRICT coeffs, int n) const CXXPH_NOEXCEPT
    {

        dest_frame_t t;

        for (int i = 0; i < n; ++i) {
            t += samples[i] * coeffs[i];
        }

        (*dest) = t;
    }
};

// Well-known forms
typedef general_polyphase_core_operator<float, float, float, 1> f32_mono_basic_polyphase_core_operator;
typedef general_polyphase_core_operator<float, float, float, 2> f32_stereo_basic_polyphase_core_operator;

typedef general_polyphase_core_operator<double, double, float, 1> f64_mono_basic_polyphase_core_operator;
typedef general_polyphase_core_operator<double, double, float, 2> f64_stereo_basic_polyphase_core_operator;

} // namespace resampler
} // namespace cxxdasp

#endif // CXXDASP_RESAMPLER_POLYPHASE_GENERAL_POLYPHASE_CORE_OPERATOR_HPP_
