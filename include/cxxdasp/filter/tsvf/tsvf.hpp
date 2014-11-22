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

#ifndef CXXDASP_FILTER_TSVF_TSVF_HPP_
#define CXXDASP_FILTER_TSVF_TSVF_HPP_

#include <cxxporthelper/compiler.hpp>

#include <cxxdasp/filter/digital_filter.hpp>
#include <cxxdasp/filter/tsvf/tsvf_coeffs.hpp>

namespace cxxdasp {
namespace filter {

/**
 * Linear Trapezoidal Integrated State Variable Filter
 *
 * @tparam TFrame audio frame type
 * @tparam TTSVFCoreOperator core operator class
 *
 * @sa "Solving the continuous SVF equations using trapezoidal integration and equivalent currents"
 *          by Andrew Simper  <andy@cytomic.com>
 *     http://www.cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
 */
template <typename TFrame, class TTSVFCoreOperator>
class trapezoidal_state_variable_filter {

    /// @cond INTERNAL_FIELD
    trapezoidal_state_variable_filter(const trapezoidal_state_variable_filter &) = delete;
    trapezoidal_state_variable_filter &operator=(const trapezoidal_state_variable_filter &) = delete;
    /// @endcond

public:
    /**
     * Audio frame type.
     */
    typedef TFrame frame_type;

    /**
     * Core operator class.
     */
    typedef TTSVFCoreOperator core_operator_type;

    /**
     * Constructor.
     */
    trapezoidal_state_variable_filter();

    /**
     * Destructor.
     */
    ~trapezoidal_state_variable_filter();

    /**
     * Initialize.
     * @param params [in] general filter parameters
     * @returns Success or Failure
     */
    bool init(const filter_params_t &params) CXXPH_NOEXCEPT;

    /**
     * Initialize.
     * @param coeffs [in] trapezoidal state variable filter coefficients
     * @returns Success or Failure
     */
    bool init(const trapezoidal_state_variable_filter_coeffs &coeffs) CXXPH_NOEXCEPT;

    /**
     * Update filter parameters.
     * @param params [in] general filter parameters
     * @returns Success or Failure
     */
    bool update(const filter_params_t &params) CXXPH_NOEXCEPT;

    /**
     * Update filter parameters.
     * @param coeffs [in] trapezoidal state variable filter coefficients
     * @returns Success or Failure
     */
    bool update(const trapezoidal_state_variable_filter_coeffs &coeffs) CXXPH_NOEXCEPT;

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

template <typename TFrame, class TTSVFCoreOperator>
inline trapezoidal_state_variable_filter<TFrame, TTSVFCoreOperator>::trapezoidal_state_variable_filter()
    : core_operator_()
{
}

template <typename TFrame, class TTSVFCoreOperator>
inline trapezoidal_state_variable_filter<TFrame, TTSVFCoreOperator>::~trapezoidal_state_variable_filter()
{
}

template <typename TFrame, class TTSVFCoreOperator>
inline bool trapezoidal_state_variable_filter<TFrame, TTSVFCoreOperator>::init(const filter_params_t &params)
    CXXPH_NOEXCEPT
{
    trapezoidal_state_variable_filter_coeffs coeffs;

    if (!coeffs.make(params)) {
        return false;
    }

    return init(coeffs);
}

template <typename TFrame, class TTSVFCoreOperator>
inline bool trapezoidal_state_variable_filter<TFrame, TTSVFCoreOperator>::init(
    const trapezoidal_state_variable_filter_coeffs &coeffs) CXXPH_NOEXCEPT
{
    core_operator_.set_params(coeffs.a1, coeffs.a2, coeffs.a3, coeffs.m0, coeffs.m1, coeffs.m2);
    core_operator_.reset();

    return true;
}

template <typename TFrame, class TTSVFCoreOperator>
inline bool trapezoidal_state_variable_filter<TFrame, TTSVFCoreOperator>::update(const filter_params_t &params)
    CXXPH_NOEXCEPT
{
    trapezoidal_state_variable_filter_coeffs coeffs;

    if (!coeffs.make(params)) {
        return false;
    }

    return update(coeffs);
}

template <typename TFrame, class TTSVFCoreOperator>
inline bool trapezoidal_state_variable_filter<TFrame, TTSVFCoreOperator>::update(
    const trapezoidal_state_variable_filter_coeffs &coeffs) CXXPH_NOEXCEPT
{
    core_operator_.set_params(coeffs.a1, coeffs.a2, coeffs.a3, coeffs.m0, coeffs.m1, coeffs.m2);

    return true;
}

template <typename TFrame, class TTSVFCoreOperator>
inline void trapezoidal_state_variable_filter<TFrame, TTSVFCoreOperator>::reset() CXXPH_NOEXCEPT
{
    core_operator_.reset();
}

template <typename TFrame, class TTSVFCoreOperator>
inline void trapezoidal_state_variable_filter<TFrame, TTSVFCoreOperator>::perform(frame_type *src_dest, int n)
    CXXPH_NOEXCEPT
{
    core_operator_.perform(src_dest, n);
}

template <typename TFrame, class TTSVFCoreOperator>
inline void trapezoidal_state_variable_filter<TFrame, TTSVFCoreOperator>::perform(const frame_type *CXXPH_RESTRICT src,
                                                                                  frame_type *CXXPH_RESTRICT dest,
                                                                                  int n) CXXPH_NOEXCEPT
{
    core_operator_.perform(src, dest, n);
}

} // namespace filter
} // namespace cxxdasp

#endif // CXXDASP_FILTER_TSVF_TSVF_HPP_
