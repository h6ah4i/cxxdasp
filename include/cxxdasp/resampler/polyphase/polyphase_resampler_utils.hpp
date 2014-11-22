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

#ifndef CXXDASP_RESAMPLER_POLYPHASE_POLYPHASE_RESAMPER_UTILS_HPP_
#define CXXDASP_RESAMPLER_POLYPHASE_POLYPHASE_RESAMPER_UTILS_HPP_

#include <cstddef>

namespace cxxdasp {
namespace resampler {

/**
 * Utility class for polyphase_resampler
 */
class polyphase_resampler_utils {
public:
    /// @cond INTERNAL_FIELD
    polyphase_resampler_utils() = delete;

    static int calc_delay_line_size(int num_coeffs, int m, int l, int base_block_size);
    static int calc_interleaved_coeffs_subtable_size(int num_coeffs, int m);

    static void make_interleaved_coeffs_table(const float *src_coeffs, int num_src_coeffs, int m, float *dest_coeffs);
    static void make_interleaved_coeffs_table(const double *src_coeffs, int num_src_coeffs, int m, double *dest_coeffs);

    static bool check_is_pass_through(const float *src_coeffs, int num_src_coeffs, int m, int l);
    static bool check_is_pass_through(const double *src_coeffs, int num_src_coeffs, int m, int l);

    static bool check_is_sparse_copy(const float *src_coeffs, int num_src_coeffs, int m, int l);
    static bool check_is_sparse_copy(const double *src_coeffs, int num_src_coeffs, int m, int l);
    /// @check_is_sparse_copy
};

} // namespace resampler
} // namespace cxxdasp

#endif // CXXDASP_RESAMPLER_POLYPHASE_POLYPHASE_RESAMPER_UTILS_HPP_
