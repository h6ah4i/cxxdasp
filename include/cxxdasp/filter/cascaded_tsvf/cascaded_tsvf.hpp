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

#ifndef CXXDASP_FILTER_CASCADED_TSVF_CASCADED_CASCADED_TSVF_HPP_
#define CXXDASP_FILTER_CASCADED_TSVF_CASCADED_CASCADED_TSVF_HPP_

#include <cassert>

#include <cxxporthelper/compiler.hpp>

#include <cxxdasp/filter/digital_filter.hpp>
#include <cxxdasp/filter/tsvf/tsvf.hpp>

namespace cxxdasp {
namespace filter {

/**
 * Biquad filter
 *
 * @tparam TFrame audio frame type
 * @tparam TCascadedTSVFCoreOperator core operator class
 *
 * @sa "Cookbook formulae for audio EQ biquad filter coefficients"
 *     by Robert Bristow-Johnson  <rbj@audioimagination.com>
 *     http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
 */
template <typename TFrame, class TCascadedTSVFCoreOperator>
class cascaded_trapezoidal_state_variable_filter {

    /// @cond INTERNAL_FIELD
    cascaded_trapezoidal_state_variable_filter(const cascaded_trapezoidal_state_variable_filter &) = delete;
    cascaded_trapezoidal_state_variable_filter &operator=(const cascaded_trapezoidal_state_variable_filter &) = delete;
    /// @endcond

public:
    /**
     * Audio frame type.
     */
    typedef TFrame frame_type;

    /**
     * Core operator class.
     */
    typedef TCascadedTSVFCoreOperator core_operator_type;

/**
     * Number of cascaded filters.
     */
#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
    static constexpr int num_cascaded = core_operator_type::num_cascaded;
#else
    enum { num_cascaded = core_operator_type::num_cascaded };
#endif

    /**
     * Constructor.
     */
    cascaded_trapezoidal_state_variable_filter();

    /**
     * Destructor.
     */
    ~cascaded_trapezoidal_state_variable_filter();

    /**
     * Initialize.
     * @param params [in] general filter parameters
     * @returns Success or Failure
     */
    bool init_all(const filter_params_t *params) CXXPH_NOEXCEPT;

    /**
     * Initialize.
     * @param coeffs [in] biquad filter coefficients
     * @returns Success or Failure
     */
    bool init_all(const trapezoidal_state_variable_filter_coeffs *coeffs) CXXPH_NOEXCEPT;

    /**
     * Initialize.
     * @param filter_no [in] specify filter no.  (0 - (num_cascaded - 1))
     * @param params [in] general filter parameters
     * @returns Success or Failure
     */
    bool init_partial(int filter_no, const filter_params_t *params) CXXPH_NOEXCEPT;

    /**
     * Initialize.
     * @param filter_no [in] specify filter no.  (0 - (num_cascaded - 1))
     * @param coeffs [in] biquad filter coefficients
     * @returns Success or Failure
     */
    bool init_partial(int filter_no, const trapezoidal_state_variable_filter_coeffs *coeffs) CXXPH_NOEXCEPT;

    /**
     * Update all filter parameters.
     * @param params [in] general filter parameters
     * @returns Success or Failure
     */
    bool update_all(const filter_params_t *params) CXXPH_NOEXCEPT;

    /**
     * Update all filter parameters.
     * @param coeffs [in] biquad filter coefficients
     * @returns Success or Failure
     */
    bool update_all(const trapezoidal_state_variable_filter_coeffs *coeffs) CXXPH_NOEXCEPT;

    /**
     * Update filter parameters.
     * @param filter_no [in] specify filter no.  (0 - (num_cascaded - 1))
     * @param params [in] general filter parameters
     * @returns Success or Failure
     */
    bool update_partial(int filter_no, const filter_params_t *params) CXXPH_NOEXCEPT;

    /**
     * Update filter parameters.
     * @param filter_no [in] specify filter no.  (0 - (num_cascaded - 1))
     * @param coeffs [in] biquad filter coefficients
     * @returns Success or Failure
     */
    bool update_partial(int filter_no, const trapezoidal_state_variable_filter_coeffs *coeffs) CXXPH_NOEXCEPT;

    /**
     * Reset all filters state.
     */
    void reset_all() CXXPH_NOEXCEPT;

    /**
     * Reset partial.
     * @param filter_no [in] specify filter no.  (0 - (num_cascaded - 1))
     */
    void reset_partial(int filter_no) CXXPH_NOEXCEPT;

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

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline cascaded_trapezoidal_state_variable_filter<
    TFrame, TCascadedTSVFCoreOperator>::cascaded_trapezoidal_state_variable_filter()
    : core_operator_()
{
    for (int no = 0; no < num_cascaded; ++no) {
        core_operator_.reset(no);
    }
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline cascaded_trapezoidal_state_variable_filter<
    TFrame, TCascadedTSVFCoreOperator>::~cascaded_trapezoidal_state_variable_filter()
{
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline bool
cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::init_all(const filter_params_t *params)
    CXXPH_NOEXCEPT
{

    assert(params);

    trapezoidal_state_variable_filter_coeffs coeffs[num_cascaded];

    for (int no = 0; no < num_cascaded; ++no) {
        if (!coeffs[no].make(params[no])) {
            return false;
        }
    }

    return init_all(coeffs);
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline bool cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::init_all(
    const trapezoidal_state_variable_filter_coeffs *coeffs) CXXPH_NOEXCEPT
{

    assert(coeffs);

    for (int no = 0; no < num_cascaded; ++no) {
        const trapezoidal_state_variable_filter_coeffs &c = coeffs[no];
        core_operator_.set_params(no, c.a1, c.a2, c.a3, c.m0, c.m1, c.m2);
        core_operator_.reset(no);
    }

    return true;
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline bool cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::init_partial(
    int filter_no, const filter_params_t *params) CXXPH_NOEXCEPT
{

    assert(filter_no >= 0 && filter_no < num_cascaded);
    assert(params);

    trapezoidal_state_variable_filter_coeffs coeffs;

    if (!coeffs.make(*params)) {
        return false;
    }

    return init_partial(filter_no, &coeffs);
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline bool cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::init_partial(
    int filter_no, const trapezoidal_state_variable_filter_coeffs *coeffs) CXXPH_NOEXCEPT
{

    assert(filter_no >= 0 && filter_no < num_cascaded);
    assert(coeffs);

    const trapezoidal_state_variable_filter_coeffs &c = (*coeffs);
    core_operator_.set_params(filter_no, c.a1, c.a2, c.a3, c.m0, c.m1, c.m2);
    core_operator_.reset(filter_no);

    return true;
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline bool
cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::update_all(const filter_params_t *params)
    CXXPH_NOEXCEPT
{

    assert(params);

    trapezoidal_state_variable_filter_coeffs coeffs[num_cascaded];

    for (int no = 0; no < num_cascaded; ++no) {
        if (!coeffs[no].make(params[no])) {
            return false;
        }
    }

    return update_all(coeffs);
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline bool cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::update_all(
    const trapezoidal_state_variable_filter_coeffs *coeffs) CXXPH_NOEXCEPT
{

    assert(coeffs);

    for (int no = 0; no < num_cascaded; ++no) {
        const trapezoidal_state_variable_filter_coeffs &c = coeffs[no];
        core_operator_.set_params(no, c.a1, c.a2, c.a3, c.m0, c.m1, c.m2);
    }

    return true;
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline bool cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::update_partial(
    int filter_no, const filter_params_t *params) CXXPH_NOEXCEPT
{

    assert(filter_no >= 0 && filter_no < num_cascaded);
    assert(params);

    trapezoidal_state_variable_filter_coeffs coeffs;

    if (!coeffs.make(*params)) {
        return false;
    }

    return update_partial(filter_no, &coeffs);
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline bool cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::update_partial(
    int filter_no, const trapezoidal_state_variable_filter_coeffs *coeffs) CXXPH_NOEXCEPT
{

    assert(filter_no >= 0 && filter_no < num_cascaded);
    assert(coeffs);

    const trapezoidal_state_variable_filter_coeffs &c = (*coeffs);
    core_operator_.set_params(filter_no, c.a1, c.a2, c.a3, c.m0, c.m1, c.m2);

    return true;
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline void cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::reset_all() CXXPH_NOEXCEPT
{
    for (int no = 0; no < num_cascaded; ++no) {
        core_operator_.reset(no);
    }
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline void cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::reset_partial(int filter_no)
    CXXPH_NOEXCEPT
{
    assert(filter_no >= 0 && filter_no < num_cascaded);

    core_operator_.reset(filter_no);
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline void cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::perform(frame_type *src_dest,
                                                                                                   int n) CXXPH_NOEXCEPT
{
    core_operator_.perform(src_dest, n);
}

template <typename TFrame, class TCascadedTSVFCoreOperator>
inline void cascaded_trapezoidal_state_variable_filter<TFrame, TCascadedTSVFCoreOperator>::perform(
    const frame_type *CXXPH_RESTRICT src, frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    core_operator_.perform(src, dest, n);
}

} // namespace filter
} // namespace cxxdasp

#endif // CXXDASP_FILTER_CASCADED_TSVF_CASCADED_CASCADED_TSVF_HPP_
