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

#ifndef CXXDASP_RESAMPLER_SMART_SMART_RESAMPLER_PARAMS_HPP_
#define CXXDASP_RESAMPLER_SMART_SMART_RESAMPLER_PARAMS_HPP_

namespace cxxdasp {
namespace resampler {

/**
 * Parameters for smart_resampler class
 */
struct smart_resampler_params {
    //
    // type definitions
    //

    /**
     * Stage 1 coefficients type
     */
    typedef float stage1_coeffs_t;

    /**
     * Stage 2 coefficients type
     */
    typedef float stage2_coeffs_t;

    /**
     * Stage 1 parameters type
     */
    struct stage1_x2_fir_info {
        bool is_static;                ///< indicates whether the coefficient table is statically allocated
        const stage1_coeffs_t *coeffs; ///< coefficients table
        int n_coeffs;                  ///< coefficients table size
        bool use_fft_resampler;        ///< true: fft_x2_resampler, false: halfband_x2_resampler

        /**
         * Constructor.
         */
        stage1_x2_fir_info() : is_static(false), coeffs(nullptr), n_coeffs(0), use_fft_resampler(false) {}

        /**
         * Constructor.
         *
         * @param is_static [in] "is_static" field value
         * @param coeffs [in] "coeffs" field value
         * @param n_coeffs [in] "n_coeffs" field value
         * @param use_fft_resampler [in] "use_fft_resampler" field value
         */
        stage1_x2_fir_info(bool is_static, const stage1_coeffs_t *coeffs, int n_coeffs, bool use_fft_resampler)
            : is_static(is_static), coeffs(coeffs), n_coeffs(n_coeffs), use_fft_resampler(use_fft_resampler)
        {
        }
    };

    /**
     * Stage 2 parameters type
     */
    struct stage2_poly_fir_info {
        bool is_static;                ///< indicates whether the coefficient table is statically allocated
        const stage2_coeffs_t *coeffs; ///< coefficients table
        int n_coeffs;                  ///< coefficients table size
        int m;                         ///< M: upsampling factor
        int l;                         ///< L: decimation factor

        /**
         * Constructor.
         */
        stage2_poly_fir_info() : is_static(false), coeffs(nullptr), n_coeffs(0), m(0), l(0) {}

        /**
         * Constructor.
         *
         * @param is_static [in] "is_static" field value
         * @param coeffs [in] "coeffs" field value
         * @param n_coeffs [in] "n_coeffs" field value
         * @param m [in] "m" field value
         * @param l [in] "l" field value
         */
        stage2_poly_fir_info(bool is_static, const stage2_coeffs_t *coeffs, int n_coeffs, int m, int l)
            : is_static(is_static), coeffs(coeffs), n_coeffs(n_coeffs), m(m), l(l)
        {
        }
    };

    //
    // methods
    //

    /**
     * Constructor.
     */
    smart_resampler_params() : have_stage1(false), have_stage2(false), stage1(), stage2() {}

    //
    // fields
    //
    bool have_stage1;            ///< have stage 1 (FFT based x2 oversampling)
    bool have_stage2;            ///< have stage 2 (Rational resampling with polyphase filter)
    stage1_x2_fir_info stage1;   ///< stage 1 parameters
    stage2_poly_fir_info stage2; ///< stage 2 parameters
};

} // namespace resampler
} // namespace cxxdasp

#endif // CXXDASP_RESAMPLER_SMART_SMART_RESAMPLER_PARAMS_HPP_
