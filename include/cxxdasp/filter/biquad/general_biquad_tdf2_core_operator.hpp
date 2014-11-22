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

#ifndef CXXDASP_FILTER_BIQUAD_GENERAL_BIQUAD_TDF2_CORE_OPERATOR_HPP_
#define CXXDASP_FILTER_BIQUAD_GENERAL_BIQUAD_TDF2_CORE_OPERATOR_HPP_

#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/type_traits>

#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace filter {

/**
 * General biquad filter core operator (Transposed Direct Form 2)
 *
 * @tparam TFrame audio frame type
 */
template <typename TFrame>
class general_biquad_transposed_direct_form_2_core_operator {

    /// @cond INTERNAL_FIELD
    general_biquad_transposed_direct_form_2_core_operator(
        const general_biquad_transposed_direct_form_2_core_operator &) = delete;
    general_biquad_transposed_direct_form_2_core_operator &
    operator=(const general_biquad_transposed_direct_form_2_core_operator &) = delete;
    /// @endcond

public:
    /**
     * Audio frame type.
     */
    typedef TFrame frame_type;

    /**
     * Value type.
     */
    typedef typename frame_type::data_type value_type;

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    CXXPH_OPTIONAL_CONSTEXPR static bool is_supported() CXXPH_NOEXCEPT { return true; }

    /**
     * Constructor.
     */
    general_biquad_transposed_direct_form_2_core_operator();

    /**
     * Destructor.
     */
    ~general_biquad_transposed_direct_form_2_core_operator();

    /**
     * Set parameters.
     * @param b0 [in] b0 filter parameter
     * @param b1 [in] b1 filter parameter
     * @param b2 [in] b2 filter parameter
     * @param a1 [in] a1 filter parameter
     * @param a2 [in] a2 filter parameter
     */
    void set_params(double b0, double b1, double b2, double a1, double a2) CXXPH_NOEXCEPT;

    /**
     * Reset state.
     */
    void reset() CXXPH_NOEXCEPT;

    /**
     * Perform filtering.
     * @param src_dest [in/out] data buffer
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
    static_assert(std::is_floating_point<value_type>::value, "floating point type is required");

    /// @cond INTERNAL_FIELD
    value_type b_[3];
    value_type a_[2];

    frame_type s_[2];
    /// @endcond
};

template <typename TFrame>
inline general_biquad_transposed_direct_form_2_core_operator<
    TFrame>::general_biquad_transposed_direct_form_2_core_operator()
{
    for (auto &r : b_) {
        r = 0;
    }
    for (auto &r : a_) {
        r = 0;
    }
    for (auto &r : s_) {
        r = 0;
    }
}

template <typename TFrame>
inline general_biquad_transposed_direct_form_2_core_operator<
    TFrame>::~general_biquad_transposed_direct_form_2_core_operator()
{
}

template <typename TFrame>
inline void general_biquad_transposed_direct_form_2_core_operator<TFrame>::set_params(double b0, double b1, double b2,
                                                                                      double a1, double a2)
    CXXPH_NOEXCEPT
{

    b_[0] = static_cast<value_type>(b0);
    b_[1] = static_cast<value_type>(b1);
    b_[2] = static_cast<value_type>(b2);
    a_[0] = static_cast<value_type>(a1);
    a_[1] = static_cast<value_type>(a2);
}

template <typename TFrame>
inline void general_biquad_transposed_direct_form_2_core_operator<TFrame>::reset() CXXPH_NOEXCEPT
{
    for (auto &r : s_) {
        r = 0;
    }
}

template <typename TFrame>
inline void general_biquad_transposed_direct_form_2_core_operator<TFrame>::perform(frame_type *CXXPH_RESTRICT src_dest,
                                                                                   int n) CXXPH_NOEXCEPT
{

#define CXXDASP_BIQUAD_TDF2_STEP(offset)                                                                               \
    do {                                                                                                               \
        x0 = src_dest[(offset)];                                                                                       \
        y0 = s1 + (b0 * x0);                                                                                           \
        s1 = s2 + (ia1 * y0) + (b1 * x0);                                                                              \
        s2 = (ia2 * y0) + (b2 * x0);                                                                                   \
        src_dest[(offset)] = y0;                                                                                       \
    } while (0)

    const value_type b0 = b_[0];
    const value_type b1 = b_[1];
    const value_type b2 = b_[2];
    const value_type ia1 = -(a_[0]);
    const value_type ia2 = -(a_[1]);

    frame_type s1 = s_[0];
    frame_type s2 = s_[1];
    frame_type x0;
    frame_type y0;

#if 1 // loop-unrolling
    const int n1 = (n / 4) * 4;
    for (int i = 0; i < n1; i += 4) {
        CXXDASP_BIQUAD_TDF2_STEP(i + 0);
        CXXDASP_BIQUAD_TDF2_STEP(i + 1);
        CXXDASP_BIQUAD_TDF2_STEP(i + 2);
        CXXDASP_BIQUAD_TDF2_STEP(i + 3);
    }

    src_dest += n1;
    n -= n1;
#endif

    for (int i = 0; i < n; ++i) {
        CXXDASP_BIQUAD_TDF2_STEP(i);
    }

    s_[0] = s1;
    s_[1] = s2;

#undef CXXDASP_BIQUAD_TDF2_STEP
}

template <typename TFrame>
inline void general_biquad_transposed_direct_form_2_core_operator<TFrame>::perform(const frame_type *CXXPH_RESTRICT src,
                                                                                   frame_type *CXXPH_RESTRICT dest,
                                                                                   int n) CXXPH_NOEXCEPT
{

#define CXXDASP_BIQUAD_TDF2_STEP(offset)                                                                               \
    do {                                                                                                               \
        x0 = src[(offset)];                                                                                            \
        y0 = s1 + (b0 * x0);                                                                                           \
        s1 = s2 + (ia1 * y0) + (b1 * x0);                                                                              \
        s2 = (ia2 * y0) + (b2 * x0);                                                                                   \
        dest[(offset)] = y0;                                                                                           \
    } while (0)

    const value_type b0 = b_[0];
    const value_type b1 = b_[1];
    const value_type b2 = b_[2];
    const value_type ia1 = -(a_[0]);
    const value_type ia2 = -(a_[1]);

    frame_type s1 = s_[0];
    frame_type s2 = s_[1];
    frame_type x0;
    frame_type y0;

#if 1 // loop-unrolling
    const int n1 = (n / 4) * 4;
    for (int i = 0; i < n1; i += 4) {
        CXXDASP_BIQUAD_TDF2_STEP(i + 0);
        CXXDASP_BIQUAD_TDF2_STEP(i + 1);
        CXXDASP_BIQUAD_TDF2_STEP(i + 2);
        CXXDASP_BIQUAD_TDF2_STEP(i + 3);
    }

    src += n1;
    dest += n1;
    n -= n1;
#endif

    for (int i = 0; i < n; ++i) {
        CXXDASP_BIQUAD_TDF2_STEP(i);
    }

    s_[0] = s1;
    s_[1] = s2;

#undef CXXDASP_BIQUAD_TDF2_STEP
}

} // namespace filter
} // namespace cxxdasp

#endif // CXXDASP_FILTER_BIQUAD_GENERAL_BIQUAD_TDF2_CORE_OPERATOR_HPP_
