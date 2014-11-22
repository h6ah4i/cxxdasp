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

#ifndef CXXDASP_UTILS_IMPL_UTILS_IMPL_GENERAL_HPP_
#define CXXDASP_UTILS_IMPL_UTILS_IMPL_GENERAL_HPP_

#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/complex>
#include <cxxporthelper/platform_info.hpp>

namespace cxxdasp {
namespace utils {
namespace impl_general {

template <typename T>
inline void multiply_scaler(T *CXXPH_RESTRICT src_dest, T x, int n) CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        src_dest[i] *= x;
    }
}

template <typename T>
inline void multiply_scaler(T *CXXPH_RESTRICT dest, const T *CXXPH_RESTRICT src, T x, int n) CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * x;
    }
}

template <typename T>
inline void multiply_scaler_aligned(T *CXXPH_RESTRICT src_dest, T x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);

    multiply_scaler(src_dest, x, n);
}

template <typename T>
inline void multiply_scaler_aligned(T *CXXPH_RESTRICT dest, const T *CXXPH_RESTRICT src, T x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, CXXPH_PLATFORM_SIMD_ALIGNMENT);

    multiply_scaler(dest, src, x, n);
}

template <typename T>
inline void multiply(T *CXXPH_RESTRICT src_dest, const T *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        src_dest[i] *= x[i];
    }
}

template <typename T>
inline void multiply(T *CXXPH_RESTRICT dest, const T *CXXPH_RESTRICT src, const T *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * x[i];
    }
}

template <typename T>
inline void multiply_aligned(T *CXXPH_RESTRICT src_dest, const T *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);

    multiply(src_dest, x, n);
}

template <typename T>
inline void multiply_aligned(T *CXXPH_RESTRICT dest, const T *CXXPH_RESTRICT src, const T *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, CXXPH_PLATFORM_SIMD_ALIGNMENT);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, CXXPH_PLATFORM_SIMD_ALIGNMENT);

    multiply(dest, src, x, n);
}

template <typename T>
inline void multiply_scaler(std::complex<T> *CXXPH_RESTRICT src_dest, T x, int n) CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        src_dest[i] *= x;
    }
}

template <typename T>
inline void multiply_scaler(std::complex<T> *CXXPH_RESTRICT dest, const std::complex<T> *CXXPH_RESTRICT src, T x, int n)
    CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * x;
    }
}

template <typename T>
inline void multiply(std::complex<T> *CXXPH_RESTRICT src_dest, const std::complex<T> *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        src_dest[i] *= x[i];
    }
}

template <typename T>
inline void multiply(std::complex<T> *CXXPH_RESTRICT dest, const std::complex<T> *CXXPH_RESTRICT src,
                     const std::complex<T> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * x[i];
    }
}

template <typename T>
inline void multiply_scaler_aligned(std::complex<T> *CXXPH_RESTRICT src_dest, T x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);

    multiply_scaler(src_dest, x, n);
}

template <typename T>
inline void multiply_scaler_aligned(std::complex<T> *CXXPH_RESTRICT dest, const std::complex<T> *CXXPH_RESTRICT src,
                                    T x, int n) CXXPH_NOEXCEPT
{

    CXXDASP_UTIL_ASSUME_ALIGNED(dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, CXXPH_PLATFORM_SIMD_ALIGNMENT);

    multiply_scaler(dest, src, x, n);
}

template <typename T>
inline void multiply_aligned(std::complex<T> *CXXPH_RESTRICT src_dest, const std::complex<T> *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, CXXPH_PLATFORM_SIMD_ALIGNMENT);

    multiply(src_dest, x, n);
}

template <typename T>
inline void multiply_aligned(std::complex<T> *CXXPH_RESTRICT dest, const std::complex<T> *CXXPH_RESTRICT src,
                             const std::complex<T> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, CXXPH_PLATFORM_SIMD_ALIGNMENT);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, CXXPH_PLATFORM_SIMD_ALIGNMENT);

    multiply(dest, src, x, n);
}

template <typename T>
inline void conj(std::complex<T> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{

    for (int i = 0; i < n; ++i) {
        src_dest[i] = std::conj(src_dest[i]);
    }
}

template <typename T>
inline void conj(std::complex<T> *CXXPH_RESTRICT dest, const std::complex<T> *CXXPH_RESTRICT src, int n) CXXPH_NOEXCEPT
{

    for (int i = 0; i < n; ++i) {
        dest[i] = std::conj(src[i]);
    }
}

template <typename T>
inline void conj_aligned(std::complex<T> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);

    conj(src_dest, n);
}

template <typename T>
inline void conj_aligned(std::complex<T> *CXXPH_RESTRICT dest, const std::complex<T> *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, CXXPH_PLATFORM_SIMD_ALIGNMENT);

    conj(dest, src, n);
}

template <typename T>
inline void mirror_conj(std::complex<T> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
    const std::complex<T> *CXXPH_RESTRICT src = &center[-1];
    std::complex<T> *CXXPH_RESTRICT dest = &center[+1];

    for (int i = 0; i < n; ++i) {
        (*dest) = std::conj(*src);
        --src;
        ++dest;
    }
}

template <typename T>
inline void mirror_conj_aligned(std::complex<T> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(center, CXXPH_PLATFORM_SIMD_ALIGNMENT);

    mirror_conj(center, n);
}

template <typename TDest, typename TSrc>
inline void interleave(TDest *CXXPH_RESTRICT dest, const TSrc *CXXPH_RESTRICT src1, const TSrc *CXXPH_RESTRICT src2,
                       int n) CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest[2 * i + 0] = src1[i];
        dest[2 * i + 1] = src2[i];
    }
}

template <typename TDest, typename TSrc>
inline void deinterleave(TDest *CXXPH_RESTRICT dest1, TDest *CXXPH_RESTRICT dest2, const TSrc *CXXPH_RESTRICT src,
                         int n) CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest1[i] = src[2 * i + 0];
        dest2[i] = src[2 * i + 1];
    }
}

} // namespace impl_general
} // namespace utils
} // namespace cxxdasp

#endif // CXXDASP_UTILS_IMPL_UTILS_IMPL_GENERAL_HPP_
