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

#include <cxxdasp/resampler/polyphase/polyphase_resampler_utils.hpp>

namespace cxxdasp {
namespace resampler {

int polyphase_resampler_utils::calc_delay_line_size(int num_coeffs, int m, int l, int base_block_size)
{
    int t = 0;

    t = (m >= t) ? m : t;
    t = (l >= t) ? l : t;
    t = (num_coeffs >= t) ? num_coeffs : t;

    return (base_block_size + num_coeffs + t);
}

int polyphase_resampler_utils::calc_interleaved_coeffs_subtable_size(int num_coeffs, int m)
{
    return (num_coeffs + (m - 1)) / m;
}

template <typename T>
void template_func_make_interleaved_coeffs_table(const T *src_coeffs, int num_src_coeffs, int m,
                                                 T *dest_interleaved_coeffs)
{
    const int sub_table_size = polyphase_resampler_utils::calc_interleaved_coeffs_subtable_size(num_src_coeffs, m);

    for (int i = 0; i < m; ++i) {
        T *dest_sub_table = &dest_interleaved_coeffs[sub_table_size * i];

        for (int j = 0; j < sub_table_size; ++j) {
            const int k = (j * m) + i;
            dest_sub_table[j] = (k < num_src_coeffs) ? (src_coeffs[k] * m) : 0.0f;
        }
    }
}

void polyphase_resampler_utils::make_interleaved_coeffs_table(const float *src_coeffs, int num_src_coeffs, int m,
                                                              float *dest_interleaved_coeffs)
{
    template_func_make_interleaved_coeffs_table(src_coeffs, num_src_coeffs, m, dest_interleaved_coeffs);
}

void polyphase_resampler_utils::make_interleaved_coeffs_table(const double *src_coeffs, int num_src_coeffs, int m,
                                                              double *dest_interleaved_coeffs)
{
    template_func_make_interleaved_coeffs_table(src_coeffs, num_src_coeffs, m, dest_interleaved_coeffs);
}

bool polyphase_resampler_utils::check_is_pass_through(const float *src_coeffs, int num_src_coeffs, int m, int l)
{
    return (m == 1 && l == 1 && num_src_coeffs == 1 && src_coeffs[0] == 1.0f);
}

bool polyphase_resampler_utils::check_is_pass_through(const double *src_coeffs, int num_src_coeffs, int m, int l)
{
    return (m == 1 && l == 1 && num_src_coeffs == 1 && src_coeffs[0] == 1.0);
}

bool polyphase_resampler_utils::check_is_sparse_copy(const float *src_coeffs, int num_src_coeffs, int m, int l)
{
    return (m == 1 && l > 1 && num_src_coeffs == 1 && src_coeffs[0] == 1.0f);
}

bool polyphase_resampler_utils::check_is_sparse_copy(const double *src_coeffs, int num_src_coeffs, int m, int l)
{
    return (m == 1 && l > 1 && num_src_coeffs == 1 && src_coeffs[0] == 1.0);
}

} // namespace resampler
} // namespace cxxdasp
