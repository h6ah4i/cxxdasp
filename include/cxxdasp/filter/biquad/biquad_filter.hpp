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

#ifndef CXXDASP_FILTER_BIQUAD_BIQUAD_FILTER_HPP_
#define CXXDASP_FILTER_BIQUAD_BIQUAD_FILTER_HPP_

#include <cxxporthelper/compiler.hpp>

#include <cxxdasp/filter/digital_filter.hpp>
#include <cxxdasp/filter/biquad/biquad_filter_coeffs.hpp>

namespace cxxdasp {
namespace filter {

/**
 * Biquad filter
 *
 * @tparam TFrame audio frame type
 * @tparam TBiquadCoreOperator core operator class
 *
 * @sa "Cookbook formulae for audio EQ biquad filter coefficients"
 *     by Robert Bristow-Johnson  <rbj@audioimagination.com>
 *     http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
 */
template <typename TFrame, class TBiquadCoreOperator>
class biquad_filter {

    /// @cond INTERNAL_FIELD
    biquad_filter(const biquad_filter &) = delete;
    biquad_filter &operator=(const biquad_filter &) = delete;
    /// @endcond

public:
    /**
     * Audio frame type.
     */
    typedef TFrame frame_type;

    /**
     * Core operator class.
     */
    typedef TBiquadCoreOperator core_operator_type;

    /**
     * Constructor.
     */
    biquad_filter();

    /**
     * Destructor.
     */
    ~biquad_filter();

    /**
     * Initialize.
     * @param params [in] general filter parameters
     * @returns Success or Failure
     */
    bool init(const filter_params_t &params) CXXPH_NOEXCEPT;

    /**
     * Initialize.
     * @param coeffs [in] biquad filter coefficients
     * @returns Success or Failure
     */
    bool init(const biquad_filter_coeffs &coeffs) CXXPH_NOEXCEPT;

    /**
     * Update filter parameters.
     * @param params [in] general filter parameters
     * @returns Success or Failure
     */
    bool update(const filter_params_t &params) CXXPH_NOEXCEPT;

    /**
     * Update filter parameters.
     * @param coeffs [in] biquad filter coefficients
     * @returns Success or Failure
     */
    bool update(const biquad_filter_coeffs &coeffs) CXXPH_NOEXCEPT;

    /**
     * Reset state.
     */
    void reset() CXXPH_NOEXCEPT;

    /**
     * Perform filtering.
     * @param src_dest [in/out] data buffer (overwrite)
     * @param n [in] count of samples
     */
    void perform(frame_type *src_dest, int n) CXXPH_NOEXCEPT;

    /**
     * Perform filtering.
     * @param src [in] source data buffer
     * @param dest [out] destination data buffer
     * @param n [in] count of samples
     */
    void perform(const frame_type *CXXPH_RESTRICT src, frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT;

private:
    /// @cond INTERNAL_FIELD
    core_operator_type core_operator_;
    /// @endcond
};

template <typename TFrame, class TBiquadCoreOperator>
inline biquad_filter<TFrame, TBiquadCoreOperator>::biquad_filter()
    : core_operator_()
{
    core_operator_.reset();
}

template <typename TFrame, class TBiquadCoreOperator>
inline biquad_filter<TFrame, TBiquadCoreOperator>::~biquad_filter()
{
}

template <typename TFrame, class TBiquadCoreOperator>
inline bool biquad_filter<TFrame, TBiquadCoreOperator>::init(const filter_params_t &params) CXXPH_NOEXCEPT
{
    biquad_filter_coeffs coeffs;

    if (!coeffs.make(params)) {
        return false;
    }

    return init(coeffs);
}

template <typename TFrame, class TBiquadCoreOperator>
inline bool biquad_filter<TFrame, TBiquadCoreOperator>::init(const biquad_filter_coeffs &coeffs) CXXPH_NOEXCEPT
{
    core_operator_.set_params(coeffs.b0, coeffs.b1, coeffs.b2, coeffs.a1, coeffs.a2);
    core_operator_.reset();

    return true;
}

template <typename TFrame, class TBiquadCoreOperator>
inline bool biquad_filter<TFrame, TBiquadCoreOperator>::update(const filter_params_t &params) CXXPH_NOEXCEPT
{
    biquad_filter_coeffs coeffs;

    if (!coeffs.make(params)) {
        return false;
    }

    return update(coeffs);
}

template <typename TFrame, class TBiquadCoreOperator>
inline bool biquad_filter<TFrame, TBiquadCoreOperator>::update(const biquad_filter_coeffs &coeffs) CXXPH_NOEXCEPT
{
    core_operator_.set_params(coeffs.b0, coeffs.b1, coeffs.b2, coeffs.a1, coeffs.a2);

    return true;
}

template <typename TFrame, class TBiquadCoreOperator>
inline void biquad_filter<TFrame, TBiquadCoreOperator>::reset() CXXPH_NOEXCEPT
{
    core_operator_.reset();
}

template <typename TFrame, class TBiquadCoreOperator>
inline void biquad_filter<TFrame, TBiquadCoreOperator>::perform(frame_type *src_dest, int n) CXXPH_NOEXCEPT
{
    core_operator_.perform(src_dest, n);
}

template <typename TFrame, class TBiquadCoreOperator>
inline void biquad_filter<TFrame, TBiquadCoreOperator>::perform(const frame_type *CXXPH_RESTRICT src,
                                                                frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    core_operator_.perform(src, dest, n);
}

} // namespace filter
} // namespace cxxdasp

#endif // CXXDASP_FILTER_BIQUAD_BIQUAD_FILTER_HPP_
