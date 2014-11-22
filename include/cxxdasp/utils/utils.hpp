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

#ifndef CXXDASP_UTILS_UTILS_HPP_
#define CXXDASP_UTILS_UTILS_HPP_

#include <algorithm>
#include <cstring>
#include <cassert>

#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/type_traits>
#include <cxxporthelper/cstdint>

#include <cxxdasp/fft/types.hpp>
#include <cxxdasp/utils/impl/utils_impl_core.hpp>
#include <cxxdasp/utils/impl/utils_impl_general.hpp>
#include <cxxdasp/utils/impl/utils_impl_sse_optimized.hpp>
#include <cxxdasp/utils/impl/utils_impl_neon_optimized.hpp>

namespace cxxdasp {
namespace utils {

extern int gcd(int x, int y) CXXPH_NOEXCEPT;

inline int forward_fft_real_num_outputs(int n) CXXPH_NOEXCEPT
{
    if ((n % 2) == 0) {
        return (n / 2) + 1;
    } else {
        return (n + 1) / 2;
    }
}

template <typename T>
inline bool is_pow_of_two(T x) CXXPH_NOEXCEPT
{
    static_assert(std::is_integral<T>::value, "value requires integral type");
    return ((x > 0) && ((x & (x - 1)) == 0));
}

template <typename T>
inline T next_pow_of_two(T x) CXXPH_NOEXCEPT
{
    static_assert(std::is_integral<T>::value, "value requires integral type");
    T n;
    for (n = 1; n < x; n <<= 1)
        ;
    return n;
}

inline void multiply_scaler(float *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT
{
    impl_general::multiply_scaler(src_dest, x, n);
}

inline void multiply_scaler(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, float x, int n) CXXPH_NOEXCEPT
{
    impl_general::multiply_scaler(dest, src, x, n);
}

inline void multiply_scaler(double *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT
{
    impl_general::multiply_scaler(src_dest, x, n);
}

inline void multiply_scaler(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src, double x, int n)
    CXXPH_NOEXCEPT
{
    impl_general::multiply_scaler(dest, src, x, n);
}

inline void multiply(float *CXXPH_RESTRICT src_dest, const float *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    impl_general::multiply(src_dest, x, n);
}

inline void multiply(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, const float *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    impl_general::multiply(dest, src, x, n);
}

inline void multiply(double *CXXPH_RESTRICT src_dest, const double *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    impl_general::multiply(src_dest, x, n);
}

inline void multiply(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src, const double *CXXPH_RESTRICT x,
                     int n) CXXPH_NOEXCEPT
{
    impl_general::multiply(dest, src, x, n);
}

inline void multiply_scaler(std::complex<float> *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT
{
    impl_general::multiply_scaler(src_dest, x, n);
}

inline void multiply_scaler(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src,
                            float x, int n) CXXPH_NOEXCEPT
{
    impl_general::multiply_scaler(dest, src, x, n);
}

inline void multiply_scaler(std::complex<double> *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT
{
    impl_general::multiply_scaler(src_dest, x, n);
}

inline void multiply_scaler(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                            double x, int n) CXXPH_NOEXCEPT
{
    impl_general::multiply_scaler(dest, src, x, n);
}

inline void multiply(std::complex<float> *CXXPH_RESTRICT src_dest, const std::complex<float> *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    impl_general::multiply(src_dest, x, n);
}

inline void multiply(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src,
                     const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    impl_general::multiply(dest, src, x, n);
}

inline void multiply(std::complex<double> *CXXPH_RESTRICT src_dest, const std::complex<double> *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    impl_general::multiply(src_dest, x, n);
}

inline void multiply(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                     const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    impl_general::multiply(dest, src, x, n);
}

inline void multiply_scaler_aligned(float *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_scaler_aligned(src_dest, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_scaler_aligned(src_dest, x, n);
#else
    impl_general::multiply_scaler_aligned(src_dest, x, n);
#endif
}

inline void multiply_scaler_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, float x, int n)
    CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_scaler_aligned(dest, src, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_scaler_aligned(dest, src, x, n);
#else
    impl_general::multiply_scaler_aligned(dest, src, x, n);
#endif
}

inline void multiply_scaler_aligned(double *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_scaler_aligned(src_dest, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_scaler_aligned(src_dest, x, n);
#else
    impl_general::multiply_scaler_aligned(src_dest, x, n);
#endif
}

inline void multiply_scaler_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src, double x, int n)
    CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_scaler_aligned(dest, src, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_scaler_aligned(dest, src, x, n);
#else
    impl_general::multiply_scaler_aligned(dest, src, x, n);
#endif
}

inline void multiply_aligned(float *CXXPH_RESTRICT src_dest, const float *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_aligned(src_dest, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_aligned(src_dest, x, n);
#else
    impl_general::multiply_aligned(src_dest, x, n);
#endif
}

inline void multiply_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, const float *CXXPH_RESTRICT x,
                             int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_aligned(dest, src, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_aligned(dest, src, x, n);
#else
    impl_general::multiply_aligned(dest, src, x, n);
#endif
}

inline void multiply_aligned(double *CXXPH_RESTRICT src_dest, const double *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_aligned(src_dest, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_aligned(src_dest, x, n);
#else
    impl_general::multiply_aligned(src_dest, x, n);
#endif
}

inline void multiply_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src,
                             const double *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_aligned(dest, src, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_aligned(dest, src, x, n);
#else
    impl_general::multiply_aligned(dest, src, x, n);
#endif
}

inline void multiply_scaler_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_scaler_aligned(src_dest, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_scaler_aligned(src_dest, x, n);
#else
    impl_general::multiply_scaler_aligned(src_dest, x, n);
#endif
}

inline void multiply_scaler_aligned(std::complex<float> *CXXPH_RESTRICT dest,
                                    const std::complex<float> *CXXPH_RESTRICT src, float x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_scaler_aligned(dest, src, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_scaler_aligned(dest, src, x, n);
#else
    impl_general::multiply_scaler_aligned(dest, src, x, n);
#endif
}

inline void multiply_scaler_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_scaler_aligned(src_dest, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_scaler_aligned(src_dest, x, n);
#else
    impl_general::multiply_scaler_aligned(src_dest, x, n);
#endif
}

inline void multiply_scaler_aligned(std::complex<double> *CXXPH_RESTRICT dest,
                                    const std::complex<double> *CXXPH_RESTRICT src, double x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_scaler_aligned(dest, src, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_scaler_aligned(dest, src, x, n);
#else
    impl_general::multiply_scaler_aligned(dest, src, x, n);
#endif
}

inline void multiply_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, const std::complex<float> *CXXPH_RESTRICT x,
                             int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_aligned(src_dest, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_aligned(src_dest, x, n);
#else
    impl_general::multiply_aligned(src_dest, x, n);
#endif
}

inline void multiply_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src,
                             const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_aligned(dest, src, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_aligned(dest, src, x, n);
#else
    impl_general::multiply_aligned(dest, src, x, n);
#endif
}

inline void multiply_aligned(std::complex<double> *CXXPH_RESTRICT src_dest,
                             const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_aligned(src_dest, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_aligned(src_dest, x, n);
#else
    impl_general::multiply_aligned(src_dest, x, n);
#endif
}

inline void multiply_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                             const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::multiply_aligned(dest, src, x, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::multiply_aligned(dest, src, x, n);
#else
    impl_general::multiply_aligned(dest, src, x, n);
#endif
}

inline void conj(std::complex<float> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    impl_general::conj(src_dest, n);
}

inline void conj(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
    impl_general::conj(dest, src, n);
}

inline void conj(std::complex<double> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    impl_general::conj(src_dest, n);
}

inline void conj(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
    impl_general::conj(dest, src, n);
}

inline void conj_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::conj_aligned(src_dest, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::conj_aligned(src_dest, n);
#else
    impl_general::conj_aligned(src_dest, n);
#endif
}

inline void conj_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::conj_aligned(dest, src, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::conj_aligned(dest, src, n);
#else
    impl_general::conj_aligned(dest, src, n);
#endif
}

inline void conj_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::conj_aligned(src_dest, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::conj_aligned(src_dest, n);
#else
    impl_general::conj_aligned(src_dest, n);
#endif
}

inline void conj_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                         int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::conj_aligned(dest, src, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::conj_aligned(dest, src, n);
#else
    impl_general::conj_aligned(dest, src, n);
#endif
}

inline void mirror_conj(std::complex<float> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
    impl_general::mirror_conj(center, n);
}

inline void mirror_conj(std::complex<double> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
    impl_general::mirror_conj(center, n);
}

inline void mirror_conj_aligned(std::complex<float> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::mirror_conj_aligned(center, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::mirror_conj_aligned(center, n);
#else
    impl_general::mirror_conj_aligned(center, n);
#endif
}

inline void mirror_conj_aligned(std::complex<double> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::mirror_conj_aligned(center, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::mirror_conj_aligned(center, n);
#else
    impl_general::mirror_conj_aligned(center, n);
#endif
}

template <typename TDest, typename TSrc>
inline void interleave(TDest *CXXPH_RESTRICT dest, const TSrc *CXXPH_RESTRICT src1, const TSrc *CXXPH_RESTRICT src2,
                       int n) CXXPH_NOEXCEPT
{
    impl_general::interleave<TDest, TSrc>(dest, src1, src2, n);
}

template <>
inline void interleave<float, float>(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src1,
                                     const float *CXXPH_RESTRICT src2, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::interleave(dest, src1, src2, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::interleave(dest, src1, src2, n);
#else
    impl_general::interleave<float, float>(dest, src1, src2, n);
#endif
}

template <>
inline void interleave<double, double>(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src1,
                                       const double *CXXPH_RESTRICT src2, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::interleave(dest, src1, src2, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::interleave(dest, src1, src2, n);
#else
    impl_general::interleave<double, double>(dest, src1, src2, n);
#endif
}

template <typename TDest, typename TSrc>
inline void deinterleave(TDest *CXXPH_RESTRICT dest1, TDest *CXXPH_RESTRICT dest2, const TSrc *CXXPH_RESTRICT src,
                         int n) CXXPH_NOEXCEPT
{
    impl_general::deinterleave<TDest, TSrc>(dest1, dest2, src, n);
}

template <>
inline void deinterleave<float, float>(float *CXXPH_RESTRICT dest1, float *CXXPH_RESTRICT dest2,
                                       const float *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::deinterleave(dest1, dest2, src, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::deinterleave(dest1, dest2, src, n);
#else
    impl_general::deinterleave<float, float>(dest1, dest2, src, n);
#endif
}

template <>
inline void deinterleave<double, double>(double *CXXPH_RESTRICT dest1, double *CXXPH_RESTRICT dest2,
                                         const double *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
    impl_sse::deinterleave(dest1, dest2, src, n);
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON &&                                                                              \
    ((CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64))
    impl_neon::deinterleave(dest1, dest2, src, n);
#else
    impl_general::deinterleave<double, double>(dest1, dest2, src, n);
#endif
}

// if source and destination is same type, use memcpy
template <typename T1, typename T2>
inline void fast_pod_copy(T1 *CXXPH_RESTRICT dest, T2 *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{
    static_assert(std::is_pod<T1>::value, "T1 is not POD");
    static_assert(std::is_pod<T2>::value, "T2 is not POD");

    if (std::is_same<T1, T2>::value) {
        ::memcpy(dest, src, sizeof(T1) * n);
    } else {
        for (int i = 0; i < n; i++) {
            dest[i] = src[i];
        }
    }
}

template <typename TDest, typename TSrc>
inline void stride_pod_copy(TDest *CXXPH_RESTRICT dest, int dest_stride, const TSrc *CXXPH_RESTRICT src, int src_stride,
                            int n) CXXPH_NOEXCEPT
{
    static_assert(std::is_pod<TDest>::value, "TDest is not POD");
    static_assert(std::is_pod<TSrc>::value, "TSrc is not POD");

    if (std::is_same<TDest, TSrc>::value && (dest_stride == 1) && (src_stride == 1)) {
        ::memcpy(dest, src, sizeof(TSrc) * n);
    } else {
        for (int i = 0; i < n; i++) {
            dest[i * dest_stride] = static_cast<TDest>(src[i * src_stride]);
        }
    }
}

template <typename T>
inline T clamp(T v, T min_lim, T max_lim) CXXPH_NOEXCEPT
{
    return (std::min)((std::max)(v, min_lim), max_lim);
}

} // namespace utils
} // namespace cxxdasp

#endif // CXXDASP_UTILS_UTILS_HPP_
