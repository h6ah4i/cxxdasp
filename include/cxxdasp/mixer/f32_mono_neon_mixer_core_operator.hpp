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

#ifndef CXXDASP_MIXER_F32_MONO_NEON_MIXER_CORE_OPERATOR_HPP_
#define CXXDASP_MIXER_F32_MONO_NEON_MIXER_CORE_OPERATOR_HPP_

#include <cxxporthelper/compiler.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

#include <cxxporthelper/platform_info.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace mixer {

/**
 * General mixer core operator
 *
 * @tparam TFrame audio frame type
 *
 */
class f32_mono_neon_mixer_core_operator {

    /// @cond INTERNAL_FIELD
    f32_mono_neon_mixer_core_operator(const f32_mono_neon_mixer_core_operator &) = delete;
    f32_mono_neon_mixer_core_operator &operator=(const f32_mono_neon_mixer_core_operator &) = delete;
    /// @endcond

public:
    /**
     * Audio frame type.
     */
    typedef datatype::audio_frame<float, 1> frame_type;

    /**
     * Coefficient table type.
     */
    typedef float table_coeff_type;

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    static bool is_supported() CXXPH_NOEXCEPT { return cxxporthelper::platform_info::support_arm_neon(); }

    /**
     * Constructor.
     */
    f32_mono_neon_mixer_core_operator() {}

    /**
     * Destructor.
     */
    ~f32_mono_neon_mixer_core_operator() {}

    /**
     * Multiply (scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     */
    void mul_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const frame_type &scale,
                   int n) const CXXPH_NOEXCEPT;

    /**
     * Multiply and Accumulate (scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     */
    void mac_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const frame_type &scale,
                   int n) const CXXPH_NOEXCEPT;

    /**
     * Multiply (forward table, scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param table [in] scalar coefficient table
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     *
     * @note The table is reffered forward direction. (table[0], table[1], ... table[n-1])
     */
    void mul_forward_table_and_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                     const table_coeff_type *CXXPH_RESTRICT table, const frame_type &scale,
                                     int n) const CXXPH_NOEXCEPT;

    /**
     * Multiply and Accumulate (forward table, scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param table [in] scalar coefficient table
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     *
     * @note The table is reffered forward direction. (table[0], table[1], ... table[n-1])
     */
    void mac_forward_table_and_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                     const table_coeff_type *CXXPH_RESTRICT table, const frame_type &scale,
                                     int n) const CXXPH_NOEXCEPT;

    /**
     * Multiply (backward table, scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param table [in] scalar coefficient table
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     *
     * @note The table is reffered backward direction. (table[0], table[-1], ... table[-(n-1)])
     */
    void mul_backward_table_and_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                      const table_coeff_type *CXXPH_RESTRICT table, const frame_type &scale,
                                      int n) const CXXPH_NOEXCEPT;

    /**
     * Multiply and Accumulate (backward table, scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param table [in] scalar coefficient table
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     *
     * @note The table is reffered backward direction. (table[0], table[-1], ... table[-(n-1)])
     */
    void mac_backward_table_and_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                      const table_coeff_type *CXXPH_RESTRICT table, const frame_type &scale,
                                      int n) const CXXPH_NOEXCEPT;
};

} // namespace filter
} // namespace cxxdasp

#endif // (CXXPH_COMPILER_SUPPORTS_ARM_NEON

#endif // CXXDASP_MIXER_F32_MONO_NEON_MIXER_CORE_OPERATOR_HPP_
