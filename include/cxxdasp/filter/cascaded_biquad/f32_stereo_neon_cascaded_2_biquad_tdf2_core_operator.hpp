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

#ifndef CXXDASP_FILTER_BIQUAD_F32_STEREO_NEON_CASCADED_2_BIQUAD_TDF2_CORE_OPERATOR_HPP_
#define CXXDASP_FILTER_BIQUAD_F32_STEREO_NEON_CASCADED_2_BIQUAD_TDF2_CORE_OPERATOR_HPP_

#include <cxxporthelper/compiler.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

#include <cxxporthelper/type_traits>
#include <cxxporthelper/arm_neon.hpp>
#include <cxxporthelper/platform_info.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace filter {

/// @cond INTERNAL_FIELD
namespace impl {

class f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator_impl {
public:
    static void perform(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                        const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT;

    static void perform(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                        const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                        float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT;
};

} // namespace impl
/// @endcond

/**
 * NEON optimized biquad filter core operator (stereo, Transposed Direct Form 2)
 */
class f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator {

    /// @cond INTERNAL_FIELD
    f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator(
        const f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator &) = delete;
    f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator &
    operator=(const f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator &) = delete;
    /// @endcond

public:
    /**
     * Audio frame type.
     */
    typedef datatype::audio_frame<float, 2> frame_type;

    /**
     * Value type.
     */
    typedef float value_type;

/**
     * Number of cascaded filters.
     */
#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
    static constexpr int num_cascaded = 2;
#else
    enum { num_cascaded = 2 };
#endif

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    static bool is_supported() CXXPH_NOEXCEPT { return cxxporthelper::platform_info::support_arm_neon(); }

    /**
     * Constructor.
     */
    f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator();

    /**
     * Destructor.
     */
    ~f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator();

    /**
     * Set parameters.
     * @param filter_no [in] filter no.
     * @param b0 [in] b0 filter parameter
     * @param b1 [in] b1 filter parameter
     * @param b2 [in] b2 filter parameter
     * @param a1 [in] a1 filter parameter
     * @param a2 [in] a2 filter parameter
     */
    void set_params(int filter_no, double b0, double b1, double b2, double a1, double a2) CXXPH_NOEXCEPT;

    /**
     * Reset state.
     * @param filter_no [in] filter no.
     */
    void reset(int filter_no) CXXPH_NOEXCEPT;

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
    typedef impl::f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator_impl impl_class;

    float32_t b_[3][2];
    float32_t ia_[2][2];

    float32_t s_[2][2 * 2];
    /// @endcond
};

inline f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator::
    f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator()
{
    const value_type z = static_cast<value_type>(0);

    b_[0][0] = z;
    b_[1][0] = z;
    b_[2][0] = z;
    b_[0][1] = z;
    b_[1][1] = z;
    b_[2][1] = z;

    ia_[0][0] = z;
    ia_[1][0] = z;
    ia_[0][1] = z;
    ia_[1][1] = z;

    s_[0][0] = z;
    s_[0][1] = z;
    s_[0][2] = z;
    s_[0][3] = z;
    s_[1][0] = z;
    s_[1][1] = z;
    s_[1][2] = z;
    s_[1][3] = z;
}

inline f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator::
    ~f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator()
{
}

inline void f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator::set_params(
    int filter_no, double b0, double b1, double b2, double a1, double a2) CXXPH_NOEXCEPT
{

    b_[0][filter_no] = static_cast<value_type>(b0);
    b_[1][filter_no] = static_cast<value_type>(b1);
    b_[2][filter_no] = static_cast<value_type>(b2);

    ia_[0][filter_no] = static_cast<value_type>(-a1);
    ia_[1][filter_no] = static_cast<value_type>(-a2);
}

inline void f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator::reset(int filter_no)
    CXXPH_NOEXCEPT
{
    for (int i = 0; i < (2 * 2); ++i) {
        s_[filter_no][i] = static_cast<value_type>(0);
    }
}

inline void f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator::perform(frame_type *src_dest,
                                                                                              int n) CXXPH_NOEXCEPT
{
    impl_class::perform(reinterpret_cast<float32_t *>(src_dest), n, &(b_[0][0]), &(ia_[0][0]), &(s_[0][0]));
}

inline void f32_stereo_neon_cascaded_2_biquad_transposed_direct_form_2_core_operator::perform(
    const frame_type *CXXPH_RESTRICT src, frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    impl_class::perform(reinterpret_cast<const float32_t *>(src), reinterpret_cast<float32_t *>(dest), n, &(b_[0][0]),
                        &(ia_[0][0]), &(s_[0][0]));
}

} // namespace filter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
#endif // CXXDASP_FILTER_BIQUAD_F32_STEREO_NEON_CASCADED_2_BIQUAD_TDF2_CORE_OPERATOR_HPP_
