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

#ifndef CXXDASP_FILTER_TSVF_GENERAL_TSVF_CORE_OPERATOR_HPP_
#define CXXDASP_FILTER_TSVF_GENERAL_TSVF_CORE_OPERATOR_HPP_

#include <cxxporthelper/type_traits>

#include <cxxporthelper/compiler.hpp>
#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace filter {

/**
 * General Linear Trapezoidal Integrated State Variable Filter core operator
 *
 * @tparam TFrame audio frame type
 */
template <typename TFrame>
class general_tsvf_core_operator {

    /// @cond INTERNAL_FIELD
    general_tsvf_core_operator(const general_tsvf_core_operator &) = delete;
    general_tsvf_core_operator &operator=(const general_tsvf_core_operator &) = delete;
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
    general_tsvf_core_operator();

    /**
     * Destructor.
     */
    ~general_tsvf_core_operator();

    /**
     * Set parameters.
     * @param a1 [in] a1 filter parameter
     * @param a2 [in] a2 filter parameter
     * @param a3 [in] a3 filter parameter
     * @param m0 [in] m0 filter parameter
     * @param m1 [in] m1 filter parameter
     * @param m2 [in] m2 filter parameter
     */
    void set_params(double a1, double a2, double a3, double m0, double m1, double m2) CXXPH_NOEXCEPT;

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
    static_assert(std::is_floating_point<value_type>::value, "floating point type is required");

    /// @cond INTERNAL_FIELD
    value_type a1_;
    value_type a2_;
    value_type a3_;
    value_type m0_;
    value_type m1_;
    value_type m2_;

    frame_type ic1eq_;
    frame_type ic2eq_;
    /// @endcond
};

template <typename TFrame>
inline general_tsvf_core_operator<TFrame>::general_tsvf_core_operator()
{
}

template <typename TFrame>
inline general_tsvf_core_operator<TFrame>::~general_tsvf_core_operator()
{
}

template <typename TFrame>
inline void general_tsvf_core_operator<TFrame>::set_params(double a1, double a2, double a3, double m0, double m1,
                                                           double m2) CXXPH_NOEXCEPT
{
    a1_ = static_cast<value_type>(a1);
    a2_ = static_cast<value_type>(a2);
    a3_ = static_cast<value_type>(a3);
    m0_ = static_cast<value_type>(m0);
    m1_ = static_cast<value_type>(m1);
    m2_ = static_cast<value_type>(m2);
}

template <typename TFrame>
inline void general_tsvf_core_operator<TFrame>::reset() CXXPH_NOEXCEPT
{
    ic1eq_ = 0;
    ic2eq_ = 0;
}

template <typename TFrame>
inline void general_tsvf_core_operator<TFrame>::perform(frame_type *src_dest, int n) CXXPH_NOEXCEPT
{

    const value_type a1 = a1_;
    const value_type a2 = a2_;
    const value_type a3 = a3_;
    const value_type m0 = m0_;
    const value_type m1 = m1_;
    const value_type m2 = m2_;

    frame_type ic1eq = ic1eq_;
    frame_type ic2eq = ic2eq_;

    for (int i = 0; i < n; ++i) {
        const frame_type v0 = src_dest[i];
        const frame_type v3 = v0 - ic2eq;
        const frame_type v1 = a1 * ic1eq + a2 * v3;
        const frame_type v2 = ic2eq + a2 * ic1eq + a3 * v3;
        ic1eq = 2 * v1 - ic1eq;
        ic2eq = 2 * v2 - ic2eq;
        src_dest[i] = m0 * v0 + m1 * v1 + m2 * v2;
    }

    ic1eq_ = ic1eq;
    ic2eq_ = ic2eq;
}

template <typename TFrame>
inline void general_tsvf_core_operator<TFrame>::perform(const frame_type *CXXPH_RESTRICT src,
                                                        frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{

    const value_type a1 = a1_;
    const value_type a2 = a2_;
    const value_type a3 = a3_;
    const value_type m0 = m0_;
    const value_type m1 = m1_;
    const value_type m2 = m2_;

    frame_type ic1eq = ic1eq_;
    frame_type ic2eq = ic2eq_;

    for (int i = 0; i < n; ++i) {
        const frame_type v0 = src[i];
        const frame_type v3 = v0 - ic2eq;
        const frame_type v1 = a1 * ic1eq + a2 * v3;
        const frame_type v2 = ic2eq + a2 * ic1eq + a3 * v3;
        ic1eq = 2 * v1 - ic1eq;
        ic2eq = 2 * v2 - ic2eq;
        dest[i] = m0 * v0 + m1 * v1 + m2 * v2;
    }

    ic1eq_ = ic1eq;
    ic2eq_ = ic2eq;
}

} // namespace filter
} // namespace cxxdasp

#endif // CXXDASP_FILTER_TSVF_GENERAL_TSVF_CORE_OPERATOR_HPP_
