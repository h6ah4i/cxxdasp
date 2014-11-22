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

#ifndef CXXDASP_UTILS_IMPL_UTILS_IMPL_NEON_OPTIMIZED_HPP_
#define CXXDASP_UTILS_IMPL_UTILS_IMPL_NEON_OPTIMIZED_HPP_

#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/complex>
#include <cxxporthelper/platform_info.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                                \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))

namespace cxxdasp {
namespace utils {
namespace impl_neon {

void multiply_scaler_aligned(float *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT;

void multiply_scaler_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, float x,
                             int n) CXXPH_NOEXCEPT;

void multiply_scaler_aligned(double *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT;

void multiply_scaler_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src, double x,
                             int n) CXXPH_NOEXCEPT;

void multiply_aligned(float *CXXPH_RESTRICT src_dest, const float *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT;

void multiply_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, const float *CXXPH_RESTRICT x,
                      int n) CXXPH_NOEXCEPT;

void multiply_aligned(double *CXXPH_RESTRICT src_dest, const double *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT;

void multiply_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src, const double *CXXPH_RESTRICT x,
                      int n) CXXPH_NOEXCEPT;

void multiply_scaler_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT;

void multiply_scaler_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src,
                             float x, int n) CXXPH_NOEXCEPT;

void multiply_scaler_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT;

void multiply_scaler_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                             double x, int n) CXXPH_NOEXCEPT;

void multiply_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, const std::complex<float> *CXXPH_RESTRICT x,
                      int n) CXXPH_NOEXCEPT;

void multiply_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src,
                      const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT;

void multiply_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, const std::complex<double> *CXXPH_RESTRICT x,
                      int n) CXXPH_NOEXCEPT;

void multiply_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                      const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT;

void conj_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT;

void conj_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src,
                  int n) CXXPH_NOEXCEPT;

void conj_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT;

void conj_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                  int n) CXXPH_NOEXCEPT;

void mirror_conj_aligned(std::complex<float> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT;

void mirror_conj_aligned(std::complex<double> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT;

void interleave(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src1, const float *CXXPH_RESTRICT src2,
                int n) CXXPH_NOEXCEPT;

void interleave(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src1, const double *CXXPH_RESTRICT src2,
                int n) CXXPH_NOEXCEPT;

void deinterleave(float *CXXPH_RESTRICT dest1, float *CXXPH_RESTRICT dest2, const float *CXXPH_RESTRICT src,
                  int n) CXXPH_NOEXCEPT;

void deinterleave(double *CXXPH_RESTRICT dest1, double *CXXPH_RESTRICT dest2, const double *CXXPH_RESTRICT src,
                  int n) CXXPH_NOEXCEPT;

} // namespace impl_neon
} // namespace utils
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON && ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH ==
       // CXXPH_ARCH_ARM64))
#endif // CXXDASP_UTILS_IMPL_UTILS_IMPL_NEON_OPTIMIZED_HPP_
