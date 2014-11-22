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

#ifndef CXXDASP_CONVERTER_S16_TO_F32_MONO_NEON_SAMPLE_FORMAT_CONVERTER_CORE_OPERATOR_HPP_
#define CXXDASP_CONVERTER_S16_TO_F32_MONO_NEON_SAMPLE_FORMAT_CONVERTER_CORE_OPERATOR_HPP_

#include <cxxporthelper/compiler.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

#include <cxxporthelper/cstdint>
#include <cxxporthelper/platform_info.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace converter {

/**
 * NEON optimized
 * Signed integer 16 (monaural) -> Float32 (monaural)
 */
class s16_to_f32_mono_neon_sample_format_converter_core_operator {

    /// @cond INTERNAL_FIELD
    s16_to_f32_mono_neon_sample_format_converter_core_operator(
        const s16_to_f32_mono_neon_sample_format_converter_core_operator &) = delete;
    s16_to_f32_mono_neon_sample_format_converter_core_operator &
    operator=(const s16_to_f32_mono_neon_sample_format_converter_core_operator &) = delete;
    /// @endcond

public:
    /**
     * Source audio frame type.
     */
    typedef datatype::audio_frame<int16_t, 1> src_frame_type;

    /**
     * Destination audio frame type.
     */
    typedef datatype::audio_frame<float, 1> dest_frame_type;

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    static bool is_supported() CXXPH_NOEXCEPT { return cxxporthelper::platform_info::support_arm_neon(); }

    /**
     * Constructor.
     */
    s16_to_f32_mono_neon_sample_format_converter_core_operator() {}

    /**
     * Destructor.
     */
    ~s16_to_f32_mono_neon_sample_format_converter_core_operator() {}

    /**
     * Perform sample format conversion.
     *
     * @param src [in] source buffer
     * @param dest [out] destination buffer
     * @param n [in] count of samples
     */
    void perform(const src_frame_type *CXXPH_RESTRICT src, dest_frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT;

private:
    /// @cond INTERNAL_FIELD
    static void perform_impl(const int16_t *CXXPH_RESTRICT src, float *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT;
    /// @endcond
};

inline void s16_to_f32_mono_neon_sample_format_converter_core_operator::perform(
    const src_frame_type *CXXPH_RESTRICT src, dest_frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    perform_impl(reinterpret_cast<const int16_t *>(src), reinterpret_cast<float *>(dest), n);
}

} // namespace converter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
#endif // CXXDASP_CONVERTER_S16_TO_F32_MONO_NEON_SAMPLE_FORMAT_CONVERTER_CORE_OPERATOR_HPP_
