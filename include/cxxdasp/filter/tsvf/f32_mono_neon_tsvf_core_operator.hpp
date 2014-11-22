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

#ifndef CXXDASP_FILTER_TSVF_F32_MONO_NEON_TSVF_CORE_OPERATOR_HPP_
#define CXXDASP_FILTER_TSVF_F32_MONO_NEON_TSVF_CORE_OPERATOR_HPP_

#include <cxxporthelper/compiler.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

#include <cxxporthelper/arm_neon.hpp>
#include <cxxporthelper/platform_info.hpp>

#include <cxxdasp/filter/digital_filter.hpp>
#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace filter {

/// @cond INTERNAL_FIELD
namespace impl {

class f32_mono_neon_tsvf_core_operator_impl {
public:
    static void perform(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, float32x2_t *CXXPH_RESTRICT pic12eq,
                        const float32x4x3_t *CXXPH_RESTRICT pcoeffs) CXXPH_NOEXCEPT;

    static void perform(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                        float32x2_t *CXXPH_RESTRICT pic12eq,
                        const float32x4x3_t *CXXPH_RESTRICT pcoeffs) CXXPH_NOEXCEPT;
};

} // namespace impl
/// @endcond

/**
 * NEON optimized Linear Trapezoidal Integrated State Variable Filter core operator (monaural)
 */
class f32_mono_neon_tsvf_core_operator {

    /// @cond INTERNAL_FIELD
    f32_mono_neon_tsvf_core_operator(const f32_mono_neon_tsvf_core_operator &) = delete;
    f32_mono_neon_tsvf_core_operator &operator=(const f32_mono_neon_tsvf_core_operator &) = delete;
    /// @endcond

public:
    /**
     * Audio frame type.
     */
    typedef datatype::audio_frame<float, 1> frame_type;

    /**
     * Value type.
     */
    typedef float value_type;

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    static bool is_supported() CXXPH_NOEXCEPT { return cxxporthelper::platform_info::support_arm_neon(); }

    /**
     * Constructor.
     */
    f32_mono_neon_tsvf_core_operator();

    /**
     * Destructor.
     */
    ~f32_mono_neon_tsvf_core_operator();

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
    /// @cond INTERNAL_FIELD
    typedef impl::f32_mono_neon_tsvf_core_operator_impl impl_class;

    float32x4x3_t cx_;
    float32x2_t ic12eq_;
    /// @endcond
};

inline f32_mono_neon_tsvf_core_operator::f32_mono_neon_tsvf_core_operator() {}

inline f32_mono_neon_tsvf_core_operator::~f32_mono_neon_tsvf_core_operator() {}

inline void f32_mono_neon_tsvf_core_operator::set_params(double a1, double a2, double a3, double m0, double m1,
                                                         double m2) CXXPH_NOEXCEPT
{
#define CXXDASP_D2F(x) static_cast<float>(x)
#define CXXDASP_D2F_X4(v1, v2, v3, v4)                                                                                 \
    {                                                                                                                  \
        CXXDASP_D2F(v1), CXXDASP_D2F(v2), CXXDASP_D2F(v3), CXXDASP_D2F(v4)                                             \
    }

    const double b0 = (m0 + (m1 * a2) + (m2 * a3));
    const double b1 = ((m1 * a1) + (m2 * a2));
    const double b2 = ((m1 * (-a2)) + (m2 * (1 - a3)));

    const float32x4_t c0 = CXXDASP_D2F_X4((2 * a2), (2 * a3), b0, b0);
    const float32x4_t c1 = CXXDASP_D2F_X4((2 * a1), (2 * a2), b1, b1);
    const float32x4_t c2 = CXXDASP_D2F_X4((2 * (0 - a2)), (2 * (1 - a3)), b2, b2);

    cx_.val[0] = c0;
    cx_.val[1] = c1;
    cx_.val[2] = c2;
#undef CXXDASP_D2F
#undef CXXDASP_D2F_X4
}

inline void f32_mono_neon_tsvf_core_operator::reset() CXXPH_NOEXCEPT { ic12eq_ = vmov_n_f32(0.0f); }

inline void f32_mono_neon_tsvf_core_operator::perform(frame_type *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    impl_class::perform(reinterpret_cast<float32_t *>(src_dest), n, &ic12eq_, &cx_);
}

inline void f32_mono_neon_tsvf_core_operator::perform(const frame_type *CXXPH_RESTRICT src,
                                                      frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    impl_class::perform(reinterpret_cast<const float32_t *>(src), reinterpret_cast<float32_t *>(dest), n, &ic12eq_,
                        &cx_);
}

} // namespace filter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
#endif // CXXDASP_FILTER_TSVF_F32_MONO_NEON_TSVF_CORE_OPERATOR_HPP_
