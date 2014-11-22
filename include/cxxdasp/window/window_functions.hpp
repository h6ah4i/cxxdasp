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

#ifndef CXXDASP_WINDOW_WINDOW_FUNCTIONS_HPP_
#define CXXDASP_WINDOW_WINDOW_FUNCTIONS_HPP_

#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/cstddef>

namespace cxxdasp {
namespace window {

void generate_rectangular_window(float *dest, size_t n) CXXPH_NOEXCEPT;
void generate_hann_window(float *dest, size_t n) CXXPH_NOEXCEPT;
void generate_hamming_window(float *dest, size_t n) CXXPH_NOEXCEPT;
void generate_blackman_window(float *dest, size_t n, double alpha = 0.16) CXXPH_NOEXCEPT;
void generate_flat_top_window(float *dest, size_t n) CXXPH_NOEXCEPT;

} // namespace window
} // namespace cxxdasp

#endif // CXXDASP_WINDOW_WINDOW_FUNCTIONS_HPP_
