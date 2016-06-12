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

#ifndef CXXDASP_RESAMPLER_POLYPHASE_F32_STEREO_NEON_POLYPHASE_CORE_OPERATOR_H_
#define CXXDASP_RESAMPLER_POLYPHASE_F32_STEREO_NEON_POLYPHASE_CORE_OPERATOR_H_

#include <cxxporthelper/compiler.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

#include <cxxporthelper/arm_neon.hpp>
#include <cxxporthelper/platform_info.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace resampler {

/**
 * Audio frame operator (NEON optimized)
 *
 * source & dest: float32, 2 ch
 */
class f32_stereo_neon_polyphase_core_operator {

    /// @cond INTERNAL_FIELD
    f32_stereo_neon_polyphase_core_operator(const f32_stereo_neon_polyphase_core_operator &) = delete;
    f32_stereo_neon_polyphase_core_operator &operator=(const f32_stereo_neon_polyphase_core_operator &) = delete;
    /// @endcond

public:
    /** Data type of source audio frame */
    typedef datatype::audio_frame<float, 2> src_frame_t;

    /** Data type of destination audio frame */
    typedef datatype::audio_frame<float, 2> dest_frame_t;

    /** Data type of FIR coefficients */
    typedef float coeffs_t;

/** Number of channels */
#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
    static constexpr int num_channels = 2;
#else
    enum { num_channels = 2 };
#endif

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    static bool is_supported() { return cxxporthelper::platform_info::support_arm_neon(); }

    /**
     * Constructor.
     */
    f32_stereo_neon_polyphase_core_operator() {}

    /**
     * Destructor.
     */
    ~f32_stereo_neon_polyphase_core_operator() {}

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM)

    /**
     * Copy multiple frames to dual destinations.
     *
     * @param [out] dest1 pointer of first destination frames
     * @param [out] dest2 pointer of second destination frames
     * @param [in] src pointer of source frames
     * @param [in] n copy length [frame]
     */
    void dual_copy(src_frame_t *CXXPH_RESTRICT dest1, src_frame_t *CXXPH_RESTRICT dest2,
                   const src_frame_t *CXXPH_RESTRICT src, int n) const CXXPH_NOEXCEPT
    {

        int32_t loop_cnt_1 = (n >> 4);  //(n / 16);
        int32_t loop_cnt_2 = (n & 0xf); // (n % 16);

        if (CXXPH_LIKELY(loop_cnt_1 > 0)) {
            asm volatile("1:\n\t"
                         "vldmia %[src]!, {d16-d31}\n\t"
                         "pld [%[src], #128]\n\t"
                         "subs %[loop_cnt], %[loop_cnt], #1\n\t"
                         "vstmia %[dest1]!, {d16-d31}\n\t"
                         "vstmia %[dest2]!, {d16-d31}\n\t"
                         "bne 1b\n\t"
                         : [src] "+r"(src), [dest1] "+r"(dest1), [dest2] "+r"(dest2), [loop_cnt] "+r"(loop_cnt_1)
                         :
                         : "memory", "d16", "d17", "d18", "d19", "d20", "d21", "d22", "d23", "d24", "d25", "d26", "d27",
                           "d28", "d29", "d30", "d31");
        }

        if (loop_cnt_2 > 0) {
            do {
                (*dest1) = (*src);
                (*dest2) = (*src);
                ++dest1;
                ++dest2;
                ++src;
            } while (--loop_cnt_2);
        }
    }

    /**
     * Calculate convolution.
     *
     * @param [out] dest pointer of destination frame
     * @param [in] samples pointer of souurce samples frames
     * @param [in] coeffs pointer of coefficients array frames
     * @param [in] n length [frame]
     */
    void convolve(dest_frame_t *CXXPH_RESTRICT dest, const src_frame_t *CXXPH_RESTRICT samples,
                  const coeffs_t *CXXPH_RESTRICT coeffs, int n) const CXXPH_NOEXCEPT
    {

        const float32_t *CXXPH_RESTRICT coeffs_f32 = reinterpret_cast<const float32_t *>(coeffs);
        const float32_t *CXXPH_RESTRICT samples_f32 = reinterpret_cast<const float32_t *>(samples);

        register float32x4_t sum1 asm("q0");
        register float32x4_t sum2 asm("q1");

        sum1 = vdupq_n_f32(0.0f);
        sum2 = vdupq_n_f32(0.0f);

        int32_t loop_cnt_1 = (n >> 3);   // (n / 8);
        int32_t loop_cnt_2 = (n & 0x07); // (n % 8);

        if (CXXPH_LIKELY(loop_cnt_1 != 0)) {
            asm volatile("1:\n\t"
                         "vld1.32 {q2, q3}, [%[coeffs]]!\n\t"
                         "\n\t"
                         "vld2.32 {q8, q9}, [%[samples]]!\n\t"
                         "vmla.f32 q0, q8, q2\n\t"
                         "vmla.f32 q1, q9, q2\n\t"
                         "\n\t"
                         "vld2.32 {q10, q11}, [%[samples]]!\n\t"
                         "vmla.f32 q0, q10, q3\n\t"
                         "vmla.f32 q1, q11, q3\n\t"
                         "\n\t"
                         "subs %[loop_cnt], %[loop_cnt], #1\n\t"
                         "bne 1b\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32),
                           [loop_cnt] "+r"(loop_cnt_1)
                         :
                         : "q2", "q3", "q8", "q9", "q10", "q11");
        }

        switch (loop_cnt_2) {
        case 7:
            asm volatile("// loop_cnt_2 == 7\n\t"
                         "vld1.32 {q2}, [%[coeffs]]!\n\t"
                         "vld2.32 {q8, q9}, [%[samples]]!\n\t"
                         "vmla.f32 q0, q8, q2\n\t"
                         "vmla.f32 q1, q9, q2\n\t"
                         "vld1.32 {d6}, [%[coeffs]]!\n\t"
                         "vld1.32 {d7[]}, [%[coeffs]]!\n\t"
                         "veor.32 q9, q9, q9\n\t"
                         "vld1.32 {d16, d17, d18}, [%[samples]]!\n\t"
                         "vuzp.32 q8, q9\n\t"
                         "vmla.f32 q0, q8, q3\n\t"
                         "vmla.f32 q1, q9, q3\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "q2", "q3", "q8", "q9");
            break;
        case 6:
            asm volatile("// loop_cnt_2 == 6\n\t"
                         "vld1.32 {q2}, [%[coeffs]]!\n\t"
                         "vld2.32 {q8, q9}, [%[samples]]!\n\t"
                         "vmla.f32 q0, q8, q2\n\t"
                         "vmla.f32 q1, q9, q2\n\t"
                         "vld1.32 {d6}, [%[coeffs]]!\n\t"
                         "vld2.32 {d16, d17}, [%[samples]]!\n\t"
                         "vmla.f32 d0, d16, d6\n\t"
                         "vmla.f32 d2, d17, d6\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "q2", "q3", "q8", "q9");
            break;
        case 5:
            asm volatile("// loop_cnt_2 == 5\n\t"
                         "vld1.32 {q2}, [%[coeffs]]!\n\t"
                         "vld2.32 {q8, q9}, [%[samples]]!\n\t"
                         "vmla.f32 q0, q8, q2\n\t"
                         "vmla.f32 q1, q9, q2\n\t"
                         "vld1.32 {d6[]}, [%[coeffs]]!\n\t"
                         "veor.32 q8, q8, q8\n\t"
                         "vld2.32 {d16[0], d17[0]}, [%[samples]]!\n\t"
                         "vmla.f32 d0, d16, d6\n\t"
                         "vmla.f32 d2, d17, d6\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "q2", "q3", "q8", "q9");
            break;
        case 4:
            asm volatile("// loop_cnt_2 == 4\n\t"
                         "vld1.32 {q2}, [%[coeffs]]!\n\t"
                         "vld2.32 {q8, q9}, [%[samples]]!\n\t"
                         "vmla.f32 q0, q8, q2\n\t"
                         "vmla.f32 q1, q9, q2\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "q2", "q8", "q9");
            break;
        case 3:
            asm volatile("// loop_cnt_2 == 3\n\t"
                         "vld1.32 {d6}, [%[coeffs]]!\n\t"
                         "vld1.32 {d7[]}, [%[coeffs]]!\n\t"
                         "veor.32 q9, q9, q9\n\t"
                         "vld1.32 {d16, d17, d18}, [%[samples]]!\n\t"
                         "vuzp.32 q8, q9\n\t"
                         "vmla.f32 q0, q8, q3\n\t"
                         "vmla.f32 q1, q9, q3\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "q3", "q8", "q9");
            break;
        case 2:
            asm volatile("// loop_cnt_2 == 2\n\t"
                         "vld1.32 {d6}, [%[coeffs]]!\n\t"
                         "vld2.32 {d16, d17}, [%[samples]]!\n\t"
                         "vmla.f32 d0, d16, d6\n\t"
                         "vmla.f32 d2, d17, d6\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "q3", "q8", "q9");
            break;
        case 1:
            asm volatile("// loop_cnt_2 == 1\n\t"
                         "vld1.32 {d6[]}, [%[coeffs]]!\n\t"
                         "veor.32 q8, q8, q8\n\t"
                         "vld2.32 {d16[0],d17[0]}, [%[samples]]!\n\t"
                         "vmla.f32 d0, d16, d6\n\t"
                         "vmla.f32 d2, d17, d6\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "q3", "q8", "q9");
            break;
        case 0:
        default:
            break;
        }

        asm volatile("vpadd.f32 d0, d0, d1\n\t"
                     "vpadd.f32 d2, d2, d3\n\t"
                     "vpadd.f32 d0, d0, d2\n\t"
                     "vstr.64 d0, [%[dest]]\n\t"
                     : [sum1] "+w"(sum1), [sum2] "+w"(sum2)
                     : [dest] "r"(dest)
                     : "memory");
    }

#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON && (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)

    /**
     * Copy multiple frames to dual destinations.
     *
     * @param [out] dest1 pointer of first destination frames
     * @param [out] dest2 pointer of second destination frames
     * @param [in] src pointer of source frames
     * @param [in] n copy length [frame]
     */
    void dual_copy(src_frame_t *CXXPH_RESTRICT dest1, src_frame_t *CXXPH_RESTRICT dest2,
                   const src_frame_t *CXXPH_RESTRICT src, int n) const CXXPH_NOEXCEPT
    {

        int32_t loop_cnt_1 = (n >> 4);  //(n / 16);
        int32_t loop_cnt_2 = (n & 0xf); // (n % 16);

        if (CXXPH_LIKELY(loop_cnt_1 > 0)) {
            asm volatile("prfm pldl1strm, [%[src], #128]\n\t"
                         "prfm pstl1strm, [%[dest1], #128]\n\t"
                         "prfm pstl1strm, [%[dest2], #128]\n\t"
                         "1:\n\t"
                         "ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[src]], #64\n\t"
                         "ld1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[src]], #64\n\t"
                         "prfm pldl1strm, [%[src], #128]\n\t"
                         "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[dest1]], #64\n\t"
                         "st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[dest1]], #64\n\t"
                         "prfm pstl1strm, [%[dest1], #128]\n\t"
                         "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [%[dest2]], #64\n\t"
                         "st1 {v4.2d, v5.2d, v6.2d, v7.2d}, [%[dest2]], #64\n\t"
                         "prfm pstl1strm, [%[dest2], #128]\n\t"
                         "subs %w[loop_cnt], %w[loop_cnt], #1\n\t"
                         "bne 1b\n\t"
                         : [src] "+r"(src), [dest1] "+r"(dest1), [dest2] "+r"(dest2), [loop_cnt] "+r"(loop_cnt_1)
                         :
                         : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7");
        }

        if (loop_cnt_2 > 0) {
            do {
                (*dest1) = (*src);
                (*dest2) = (*src);
                ++dest1;
                ++dest2;
                ++src;
            } while (--loop_cnt_2);
        }
    }

    /**
     * Calculate convolution.
     *
     * @param [out] dest pointer of destination frame
     * @param [in] samples pointer of souurce samples frames
     * @param [in] coeffs pointer of coefficients array frames
     * @param [in] n length [frame]
     */
    void convolve(dest_frame_t *CXXPH_RESTRICT dest, const src_frame_t *CXXPH_RESTRICT samples,
                  const coeffs_t *CXXPH_RESTRICT coeffs, int n) const CXXPH_NOEXCEPT
    {

        const float32_t *CXXPH_RESTRICT coeffs_f32 = reinterpret_cast<const float32_t *>(coeffs);
        const float32_t *CXXPH_RESTRICT samples_f32 = reinterpret_cast<const float32_t *>(samples);

        int32_t loop_cnt_1 = (n >> 3);   // (n / 8);
        int32_t loop_cnt_2 = (n & 0x07); // (n % 8);

        register float32x4_t sum1 asm("v0");
        register float32x4_t sum2 asm("v1");

        sum1 = vdupq_n_f32(0.0f);
        sum2 = vdupq_n_f32(0.0f);

        if (CXXPH_LIKELY(loop_cnt_1 != 0)) {
            asm volatile("1:\n\t"
                         "ld1 {v2.4s, v3.4s}, [%[coeffs]], #32\n\t"
                         "\n\t"
                         "subs %w[loop_cnt], %w[loop_cnt], #1\n\t"
                         "\n\t"
                         "ld2 {v4.4s, v5.4s}, [%[samples]], #32\n\t"
                         "fmla v0.4s, v4.4s, v2.4s\n\t"
                         "fmla v1.4s, v5.4s, v2.4s\n\t"
                         "\n\t"
                         "ld2 {v6.4s, v7.4s}, [%[samples]], #32\n\t"
                         "fmla v0.4s, v6.4s, v3.4s\n\t"
                         "fmla v1.4s, v7.4s, v3.4s\n\t"
                         "\n\t"
                         "bne 1b\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32),
                           [loop_cnt] "+r"(loop_cnt_1)
                         :
                         : "v2", "v3", "v4", "v5", "v6", "v7");
        }

        switch (loop_cnt_2) {
        case 7:
            asm volatile("// loop_cnt_2 == 7\n\t"
                         "ld1 {v2.4s}, [%[coeffs]], #16\n\t"
                         "\n\t"
                         "ld2 {v4.4s, v5.4s}, [%[samples]], #32\n\t"
                         "fmla v0.4s, v4.4s, v2.4s\n\t"
                         "fmla v1.4s, v5.4s, v2.4s\n\t"
                         "\n\t"
                         "eor v3.16b, v3.16b, v3.16b\n\t"
                         "ld1 {v3.2s}, [%[coeffs]], #8\n\t"
                         "ld1 {v3.s}[2], [%[coeffs]], #4\n\t"
                         "\n\t"
                         "eor v6.16b, v6.16b, v6.16b\n\t"
                         "eor v7.16b, v7.16b, v7.16b\n\t"
                         "ld2 {v6.2s, v7.2s}, [%[samples]], #16\n\t"
                         "ld2 {v6.s, v7.s}[2], [%[samples]], #8\n\t"
                         "fmla v0.4s, v6.4s, v3.4s\n\t"
                         "fmla v1.4s, v7.4s, v3.4s\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "v2", "v3", "v4", "v5", "v6", "v7");
            break;
        case 6:
            asm volatile("// loop_cnt_2 == 6\n\t"
                         "ld1 {v2.4s}, [%[coeffs]], #16\n\t"
                         "\n\t"
                         "ld2 {v4.4s, v5.4s}, [%[samples]], #32\n\t"
                         "fmla v0.4s, v4.4s, v2.4s\n\t"
                         "fmla v1.4s, v5.4s, v2.4s\n\t"
                         "\n\t"
                         "eor v3.16b, v3.16b, v3.16b\n\t"
                         "ld1 {v3.2s}, [%[coeffs]], #8\n\t"
                         "\n\t"
                         "eor v6.16b, v6.16b, v6.16b\n\t"
                         "eor v7.16b, v7.16b, v7.16b\n\t"
                         "ld2 {v6.2s, v7.2s}, [%[samples]], #16\n\t"
                         "fmla v0.4s, v6.4s, v3.4s\n\t"
                         "fmla v1.4s, v7.4s, v3.4s\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "v2", "v3", "v4", "v5", "v6", "v7");
            break;
        case 5:
            asm volatile("// loop_cnt_2 == 5\n\t"
                         "ld1 {v2.4s}, [%[coeffs]], #16\n\t"
                         "\n\t"
                         "ld2 {v4.4s, v5.4s}, [%[samples]], #32\n\t"
                         "fmla v0.4s, v4.4s, v2.4s\n\t"
                         "fmla v1.4s, v5.4s, v2.4s\n\t"
                         "\n\t"
                         "eor v3.16b, v3.16b, v3.16b\n\t"
                         "ld1 {v3.s}[0], [%[coeffs]], #4\n\t"
                         "\n\t"
                         "ld2r {v6.4s, v7.4s}, [%[samples]], #8\n\t"
                         "fmla v0.4s, v6.4s, v3.4s\n\t"
                         "fmla v1.4s, v7.4s, v3.4s\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "v2", "v3", "v4", "v5", "v6", "v7");
            break;
        case 4:
            asm volatile("// loop_cnt_2 == 4\n\t"
                         "ld1 {v2.4s}, [%[coeffs]], #16\n\t"
                         "\n\t"
                         "ld2 {v4.4s, v5.4s}, [%[samples]], #32\n\t"
                         "fmla v0.4s, v4.4s, v2.4s\n\t"
                         "fmla v1.4s, v5.4s, v2.4s\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "v2", "v4", "v5");
            break;
        case 3:
            asm volatile("// loop_cnt_2 == 3\n\t"
                         "eor v3.16b, v3.16b, v3.16b\n\t"
                         "ld1 {v3.2s}, [%[coeffs]], #8\n\t"
                         "ld1 {v3.s}[2], [%[coeffs]], #4\n\t"
                         "\n\t"
                         "eor v6.16b, v6.16b, v6.16b\n\t"
                         "eor v7.16b, v7.16b, v7.16b\n\t"
                         "ld2 {v6.2s, v7.2s}, [%[samples]], #16\n\t"
                         "ld2 {v6.s, v7.s}[2], [%[samples]], #8\n\t"
                         "fmla v0.4s, v6.4s, v3.4s\n\t"
                         "fmla v1.4s, v7.4s, v3.4s\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "v3", "v6", "v7");
            break;
        case 2:
            asm volatile("// loop_cnt_2 == 2\n\t"
                         "eor v3.16b, v3.16b, v3.16b\n\t"
                         "ld1 {v3.2s}, [%[coeffs]], #8\n\t"
                         "\n\t"
                         "eor v6.16b, v6.16b, v6.16b\n\t"
                         "eor v7.16b, v7.16b, v7.16b\n\t"
                         "ld2 {v6.2s, v7.2s}, [%[samples]], #16\n\t"
                         "fmla v0.4s, v6.4s, v3.4s\n\t"
                         "fmla v1.4s, v7.4s, v3.4s\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "v3", "v6", "v7");
            break;
        case 1:
            asm volatile("// loop_cnt_2 == 1\n\t"
                         "eor v3.16b, v3.16b, v3.16b\n\t"
                         "ld1 {v3.s}[0], [%[coeffs]], #4\n\t"
                         "\n\t"
                         "ld2r {v6.4s, v7.4s}, [%[samples]], #8\n\t"
                         "fmla v0.4s, v6.4s, v3.4s\n\t"
                         "fmla v1.4s, v7.4s, v3.4s\n\t"
                         : [sum1] "+w"(sum1), [sum2] "+w"(sum2), [coeffs] "+r"(coeffs_f32), [samples] "+r"(samples_f32)
                         :
                         : "v3", "v6", "v7");
            break;
        case 0:
        default:
            break;
        }

        asm volatile("faddp v0.4s, v0.4s, v1.4s\n\t"
                     "faddp v0.4s, v0.4s, v0.4s\n\t"
                     "str d0, [%[dest]]\n\t"
                     : [sum1] "+w"(sum1), [sum2] "+w"(sum2)
                     : [dest] "r"(dest)
                     : "memory");
    }

#endif
};

} // namespace resampler
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
#endif // CXXDASP_RESAMPLER_POLYPHASE_F32_STEREO_NEON_POLYPHASE_CORE_OPERATOR_H_
