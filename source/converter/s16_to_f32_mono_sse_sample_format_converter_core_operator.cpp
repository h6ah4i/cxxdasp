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

#include <cxxdasp/converter/s16_to_f32_mono_sse_sample_format_converter_core_operator.hpp>

#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)

#include <limits>
#include <cassert>
#include <algorithm>

#include <cxxporthelper/x86_intrinsics.hpp>

#include <cxxdasp/utils/utils.hpp>

namespace cxxdasp {
namespace converter {

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
template <typename LoadOp, typename StoreOp>
static inline void perform_s16_to_f32_mono_sse(const int16_t *CXXPH_RESTRICT src, float *CXXPH_RESTRICT dest, int n)
    CXXPH_NOEXCEPT
{

    const __m128 norm_coeff = _mm_set1_ps(1.0f / std::numeric_limits<int16_t>::max());

    for (int i = 0; i < n; i += 8) {
        const __m128i s16x8 = LoadOp::load(reinterpret_cast<const __m128i *>(&src[i]));
        const __m128i s32x4l = _mm_srai_epi32(_mm_unpacklo_epi16(_mm_setzero_si128(), s16x8), 16);
        const __m128i s32x4h = _mm_srai_epi32(_mm_unpackhi_epi16(_mm_setzero_si128(), s16x8), 16);
        const __m128 f32x4l = _mm_mul_ps(_mm_cvtepi32_ps(s32x4l), norm_coeff);
        const __m128 f32x4h = _mm_mul_ps(_mm_cvtepi32_ps(s32x4h), norm_coeff);
        StoreOp::store(&dest[i + 0], f32x4l);
        StoreOp::store(&dest[i + 4], f32x4h);
    }
}
#endif

void s16_to_f32_mono_sse_sample_format_converter_core_operator::perform_impl(const int16_t *CXXPH_RESTRICT src,
                                                                             float *CXXPH_RESTRICT dest, int n)
    CXXPH_NOEXCEPT
{

    using namespace cxxporthelper::x86intrinsics;

    assert(utils::is_aligned(src, 2));
    assert(utils::is_aligned(dest, 4));
    assert(n >= 0);

    const float norm_coeff = (1.0f / std::numeric_limits<int16_t>::max());

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int n1 = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 2, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] = src[i] * norm_coeff;
        }

        src += n1;
        dest += n1;
        n -= n1;
    }

    const int n2 = (n >> 3) << 3;

    if (n2 > 0) {
        if (utils::is_aligned(dest, 16)) {
            perform_s16_to_f32_mono_sse<sse2_m128i_load_aligned, sse_m128_store_aligned>(src, dest, n2);
        } else {
            perform_s16_to_f32_mono_sse<sse2_m128i_load_aligned, sse_m128_store_unaligned>(src, dest, n2);
        }

        src += n2;
        dest += n2;
        n -= n2;
    }
#endif

    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * norm_coeff;
    }
}

} // namespace converter
} // namespace cxxdasp

#endif // (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
