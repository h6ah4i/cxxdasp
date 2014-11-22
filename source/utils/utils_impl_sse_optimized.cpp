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

#include <cxxdasp/utils/impl/utils_impl_sse_optimized.hpp>

#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)

#include <cxxporthelper/x86_intrinsics.hpp>
#include <cxxporthelper/platform_info.hpp>

#include <cxxdasp/utils/impl/utils_impl_core.hpp>
#include <cxxdasp/utils/impl/utils_impl_general.hpp>

namespace cxxdasp {
namespace utils {
namespace impl_sse {

//
// internal functions
//

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
static void sse_multiply_scaler_aligned(float *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT
{
    CXXPH_ALIGNAS(16) const float aligned_x[1] = { x };
    const __m128 mx = _mm_load_ps1(&aligned_x[0]);

    const int n1 = (n >> 3);
    const int n2 = (n & 0x7);

    for (int i = 0; i < n1; ++i) {
        const __m128 a = _mm_load_ps(&src_dest[8 * i + 0]);
        const __m128 b = _mm_load_ps(&src_dest[8 * i + 4]);

        const __m128 ax = _mm_mul_ps(a, mx);
        const __m128 bx = _mm_mul_ps(b, mx);

        _mm_store_ps(&src_dest[8 * i + 0], ax);
        _mm_store_ps(&src_dest[8 * i + 4], bx);
    }

    for (int i = n1 * 8; i < n; ++i) {
        src_dest[i] *= x;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
static void sse_multiply_scaler_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, float x, int n)
    CXXPH_NOEXCEPT
{
    CXXPH_ALIGNAS(16) const float aligned_x[1] = { x };
    const __m128 mx = _mm_load_ps1(&aligned_x[0]);

    const int n1 = (n >> 3);
    const int n2 = (n & 0x7);

    for (int i = 0; i < n1; ++i) {
        const __m128 a = _mm_load_ps(&src[8 * i + 0]);
        const __m128 b = _mm_load_ps(&src[8 * i + 4]);

        const __m128 ax = _mm_mul_ps(a, mx);
        const __m128 bx = _mm_mul_ps(b, mx);

        _mm_store_ps(&dest[8 * i + 0], ax);
        _mm_store_ps(&dest[8 * i + 4], bx);
    }

    for (int i = n1 * 8; i < n; ++i) {
        dest[i] = src[i] * x;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
static void sse2_multiply_scaler_aligned(double *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    CXXPH_ALIGNAS(16) const double aligned_x[1] = { x };
    const __m128d mx = _mm_load_pd1(&aligned_x[0]);

    const int n1 = (n >> 2);
    const int n2 = (n & 0x3);

    for (int i = 0; i < n1; ++i) {
        const __m128d a = _mm_load_pd(&src_dest[4 * i + 0]);
        const __m128d b = _mm_load_pd(&src_dest[4 * i + 2]);

        const __m128d ax = _mm_mul_pd(a, mx);
        const __m128d bx = _mm_mul_pd(b, mx);

        _mm_store_pd(&src_dest[4 * i + 0], ax);
        _mm_store_pd(&src_dest[4 * i + 2], bx);
    }

    for (int i = n1 * 4; i < n; ++i) {
        src_dest[i] *= x;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
static void sse2_multiply_scaler_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src, double x, int n)
    CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    CXXPH_ALIGNAS(16) const double aligned_x[1] = { x };
    const __m128d mx = _mm_load_pd1(&aligned_x[0]);

    const int n1 = (n >> 2);
    const int n2 = (n & 0x3);

    for (int i = 0; i < n1; ++i) {
        const __m128d a = _mm_load_pd(&src[4 * i + 0]);
        const __m128d b = _mm_load_pd(&src[4 * i + 2]);

        const __m128d ax = _mm_mul_pd(a, mx);
        const __m128d bx = _mm_mul_pd(b, mx);

        _mm_store_pd(&dest[4 * i + 0], ax);
        _mm_store_pd(&dest[4 * i + 2], bx);
    }

    for (int i = n1 * 4; i < n; ++i) {
        dest[i] = src[i] * x;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
void sse_multiply_aligned(float *CXXPH_RESTRICT src_dest, const float *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    const int n1 = (n >> 3);
    const int n2 = (n & 0x7);

    for (int i = 0; i < n1; ++i) {
        const __m128 a = _mm_load_ps(&src_dest[8 * i + 0]);
        const __m128 b = _mm_load_ps(&src_dest[8 * i + 4]);
        const __m128 x1 = _mm_load_ps(&x[8 * i + 0]);
        const __m128 x2 = _mm_load_ps(&x[8 * i + 4]);

        const __m128 ax = _mm_mul_ps(a, x1);
        const __m128 bx = _mm_mul_ps(b, x2);

        _mm_store_ps(&src_dest[8 * i + 0], ax);
        _mm_store_ps(&src_dest[8 * i + 4], bx);
    }

    for (int i = n1 * 8; i < n; ++i) {
        src_dest[i] *= x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
void sse_multiply_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, const float *CXXPH_RESTRICT x,
                          int n) CXXPH_NOEXCEPT
{
    const int n1 = (n >> 3);
    const int n2 = (n & 0x7);

    for (int i = 0; i < n1; ++i) {
        const __m128 a = _mm_load_ps(&src[8 * i + 0]);
        const __m128 b = _mm_load_ps(&src[8 * i + 4]);
        const __m128 x1 = _mm_load_ps(&x[8 * i + 0]);
        const __m128 x2 = _mm_load_ps(&x[8 * i + 4]);

        const __m128 ax = _mm_mul_ps(a, x1);
        const __m128 bx = _mm_mul_ps(b, x2);

        _mm_store_ps(&dest[8 * i + 0], ax);
        _mm_store_ps(&dest[8 * i + 4], bx);
    }

    for (int i = n1 * 8; i < n; ++i) {
        dest[i] = src[i] * x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
void sse2_multiply_aligned(double *CXXPH_RESTRICT src_dest, const double *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    const int n1 = (n >> 2);
    const int n2 = (n & 0x3);

    for (int i = 0; i < n1; ++i) {
        const __m128d a = _mm_load_pd(&src_dest[4 * i + 0]);
        const __m128d b = _mm_load_pd(&src_dest[4 * i + 2]);
        const __m128d x1 = _mm_load_pd(&x[4 * i + 0]);
        const __m128d x2 = _mm_load_pd(&x[4 * i + 2]);

        const __m128d ax = _mm_mul_pd(a, x1);
        const __m128d bx = _mm_mul_pd(b, x2);

        _mm_store_pd(&src_dest[4 * i + 0], ax);
        _mm_store_pd(&src_dest[4 * i + 2], bx);
    }

    for (int i = n1 * 4; i < n; ++i) {
        src_dest[i] *= x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
void sse2_multiply_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src,
                           const double *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    const int n1 = (n >> 2);
    const int n2 = (n & 0x3);

    for (int i = 0; i < n1; ++i) {
        const __m128d a = _mm_load_pd(&src[4 * i + 0]);
        const __m128d b = _mm_load_pd(&src[4 * i + 2]);
        const __m128d x1 = _mm_load_pd(&x[4 * i + 0]);
        const __m128d x2 = _mm_load_pd(&x[4 * i + 2]);

        const __m128d ax = _mm_mul_pd(a, x1);
        const __m128d bx = _mm_mul_pd(b, x2);

        _mm_store_pd(&dest[4 * i + 0], ax);
        _mm_store_pd(&dest[4 * i + 2], bx);
    }

    for (int i = n1 * 8; i < n; ++i) {
        dest[i] = src[i] * x[i];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
static void sse_multiply_aligned(std::complex<float> *CXXPH_RESTRICT src_dest,
                                 const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    float *CXXPH_RESTRICT fsd = reinterpret_cast<float *>(const_cast<std::complex<float> *>(src_dest));
    float *CXXPH_RESTRICT fx = reinterpret_cast<float *>(const_cast<std::complex<float> *>(x));

    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    CXXPH_ALIGNAS(16) static const float add_sub_sign_array[4] = { -1.0f, 1.0f, -1.0f, 1.0f };
    const __m128 add_sub_sign = _mm_load_ps(add_sub_sign_array);

    for (int i = 0; i < (n / 2); ++i) {
        const __m128 a01 = _mm_load_ps(&fsd[4 * i]);
        const __m128 b01 = _mm_load_ps(&fx[4 * i]);

        const __m128 a01r = _mm_shuffle_ps(a01, a01, _MM_SHUFFLE(2, 2, 0, 0));
        const __m128 a01i = _mm_shuffle_ps(a01, a01, _MM_SHUFFLE(3, 3, 1, 1));
        const __m128 b01s = _mm_shuffle_ps(b01, b01, _MM_SHUFFLE(2, 3, 0, 1));

        const __m128 t0 = _mm_mul_ps(a01r, b01);
        const __m128 t1 = _mm_mul_ps(a01i, b01s);

        const __m128 ans = _mm_add_ps(t0, _mm_mul_ps(add_sub_sign, t1));

        _mm_store_ps(&fsd[4 * i], ans);
    }

    if (n & 1) {
        src_dest[n - 1] *= x[n - 1];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE && CXXPH_COMPILER_SUPPORTS_X86_SSE3
static void sse3_multiply_aligned(std::complex<float> *CXXPH_RESTRICT src_dest,
                                  const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    float *CXXPH_RESTRICT fsd = reinterpret_cast<float *>(const_cast<std::complex<float> *>(src_dest));
    float *CXXPH_RESTRICT fx = reinterpret_cast<float *>(const_cast<std::complex<float> *>(x));

    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    for (int i = 0; i < (n / 2); ++i) {
        const __m128 a01 = _mm_load_ps(&fsd[4 * i]);
        const __m128 b01 = _mm_load_ps(&fx[4 * i]);

        const __m128 a01r = _mm_moveldup_ps(a01);
        const __m128 a01i = _mm_movehdup_ps(a01);
        const __m128 b01s = _mm_shuffle_ps(b01, b01, _MM_SHUFFLE(2, 3, 0, 1));

        const __m128 t0 = _mm_mul_ps(a01r, b01);
        const __m128 t1 = _mm_mul_ps(a01i, b01s);

        const __m128 ans = _mm_addsub_ps(t0, t1);

        _mm_store_ps(&fsd[4 * i], ans);
    }

    if (n & 1) {
        src_dest[n - 1] *= x[n - 1];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
static void sse_multiply_aligned(std::complex<float> *CXXPH_RESTRICT dest,
                                 const std::complex<float> *CXXPH_RESTRICT src,
                                 const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    float *CXXPH_RESTRICT fd = reinterpret_cast<float *>(dest);
    float *CXXPH_RESTRICT fs0 = reinterpret_cast<float *>(const_cast<std::complex<float> *>(src));
    float *CXXPH_RESTRICT fs1 = reinterpret_cast<float *>(const_cast<std::complex<float> *>(x));

    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    CXXPH_ALIGNAS(16) static const float add_sub_sign_array[4] = { -1.0f, 1.0f, -1.0f, 1.0f };
    const __m128 add_sub_sign = _mm_load_ps(add_sub_sign_array);

    for (int i = 0; i < (n / 2); ++i) {
        const __m128 a01 = _mm_load_ps(&fs0[4 * i]);
        const __m128 b01 = _mm_load_ps(&fs1[4 * i]);

        const __m128 a01r = _mm_shuffle_ps(a01, a01, _MM_SHUFFLE(2, 2, 0, 0));
        const __m128 a01i = _mm_shuffle_ps(a01, a01, _MM_SHUFFLE(3, 3, 1, 1));
        const __m128 b01s = _mm_shuffle_ps(b01, b01, _MM_SHUFFLE(2, 3, 0, 1));

        const __m128 t0 = _mm_mul_ps(a01r, b01);
        const __m128 t1 = _mm_mul_ps(add_sub_sign, _mm_mul_ps(a01i, b01s));

        const __m128 ans = _mm_add_ps(t0, t1);

        _mm_store_ps(&fd[4 * i], ans);
    }

    if (n & 1) {
        dest[n - 1] = src[n - 1] * x[n - 1];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE && CXXPH_COMPILER_SUPPORTS_X86_SSE3
static void sse3_multiply_aligned(std::complex<float> *CXXPH_RESTRICT dest,
                                  const std::complex<float> *CXXPH_RESTRICT src,
                                  const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    float *CXXPH_RESTRICT fd = reinterpret_cast<float *>(dest);
    float *CXXPH_RESTRICT fs0 = reinterpret_cast<float *>(const_cast<std::complex<float> *>(src));
    float *CXXPH_RESTRICT fs1 = reinterpret_cast<float *>(const_cast<std::complex<float> *>(x));

    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    for (int i = 0; i < (n / 2); ++i) {
        const __m128 a01 = _mm_load_ps(&fs0[4 * i]);
        const __m128 b01 = _mm_load_ps(&fs1[4 * i]);

        const __m128 a01r = _mm_moveldup_ps(a01);
        const __m128 a01i = _mm_movehdup_ps(a01);
        const __m128 b01s = _mm_shuffle_ps(b01, b01, _MM_SHUFFLE(2, 3, 0, 1));

        const __m128 t0 = _mm_mul_ps(a01r, b01);
        const __m128 t1 = _mm_mul_ps(a01i, b01s);

        const __m128 ans = _mm_addsub_ps(t0, t1);

        _mm_store_ps(&fd[4 * i], ans);
    }

    if (n & 1) {
        dest[n - 1] = src[n - 1] * x[n - 1];
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
static void sse2_multiply_aligned(std::complex<double> *CXXPH_RESTRICT src_dest,
                                  const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    double *CXXPH_RESTRICT dsd = reinterpret_cast<double *>(const_cast<std::complex<double> *>(src_dest));
    double *CXXPH_RESTRICT dx = reinterpret_cast<double *>(const_cast<std::complex<double> *>(x));

    CXXPH_ALIGNAS(16) static const double add_sub_sign_array[2] = { -1.0, 1.0 };
    const __m128d add_sub_sign = _mm_load_pd(add_sub_sign_array);

    for (int i = 0; i < n; ++i) {
        const __m128d a = _mm_load_pd(&dsd[2 * i]);
        const __m128d b = _mm_load_pd(&dx[2 * i]);

        const __m128d ar = _mm_unpacklo_pd(a, a);
        const __m128d ai = _mm_unpackhi_pd(a, a);
        const __m128d bs = _mm_shuffle_pd(b, b, _MM_SHUFFLE2(0, 1));

        const __m128d t0 = _mm_mul_pd(ar, b);
        const __m128d t1 = _mm_mul_pd(ai, bs);

        const __m128d ans = _mm_add_pd(t0, _mm_mul_pd(add_sub_sign, t1));

        _mm_store_pd(&dsd[2 * i], ans);
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2 && CXXPH_COMPILER_SUPPORTS_X86_SSE3
static void sse3_multiply_aligned(std::complex<double> *CXXPH_RESTRICT src_dest,
                                  const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    double *CXXPH_RESTRICT dsd = reinterpret_cast<double *>(const_cast<std::complex<double> *>(src_dest));
    double *CXXPH_RESTRICT dx = reinterpret_cast<double *>(const_cast<std::complex<double> *>(x));

    for (int i = 0; i < n; ++i) {
        const __m128d a = _mm_load_pd(&dsd[2 * i]);
        const __m128d b = _mm_load_pd(&dx[2 * i]);

        const __m128d ar = _mm_unpacklo_pd(a, a);
        const __m128d ai = _mm_unpackhi_pd(a, a);
        const __m128d bs = _mm_shuffle_pd(b, b, _MM_SHUFFLE2(0, 1));

        const __m128d t0 = _mm_mul_pd(ar, b);
        const __m128d t1 = _mm_mul_pd(ai, bs);

        const __m128d ans = _mm_addsub_pd(t0, t1);

        _mm_store_pd(&dsd[2 * i], ans);
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
static void sse2_multiply_aligned(std::complex<double> *CXXPH_RESTRICT dest,
                                  const std::complex<double> *CXXPH_RESTRICT src,
                                  const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    double *CXXPH_RESTRICT dd = reinterpret_cast<double *>(const_cast<std::complex<double> *>(dest));
    double *CXXPH_RESTRICT ds = reinterpret_cast<double *>(const_cast<std::complex<double> *>(src));
    double *CXXPH_RESTRICT dx = reinterpret_cast<double *>(const_cast<std::complex<double> *>(x));

    CXXPH_ALIGNAS(16) static const double add_sub_sign_array[2] = { -1.0, 1.0 };
    const __m128d add_sub_sign = _mm_load_pd(add_sub_sign_array);

    for (int i = 0; i < n; ++i) {
        const __m128d a = _mm_load_pd(&ds[2 * i]);
        const __m128d b = _mm_load_pd(&dx[2 * i]);

        const __m128d ar = _mm_unpacklo_pd(a, a);
        const __m128d ai = _mm_unpackhi_pd(a, a);
        const __m128d bs = _mm_shuffle_pd(b, b, _MM_SHUFFLE2(0, 1));

        const __m128d t0 = _mm_mul_pd(ar, b);
        const __m128d t1 = _mm_mul_pd(ai, bs);

        const __m128d ans = _mm_add_pd(t0, _mm_mul_pd(add_sub_sign, t1));

        _mm_store_pd(&dd[2 * i], ans);
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2 && CXXPH_COMPILER_SUPPORTS_X86_SSE3
static void sse3_multiply_aligned(std::complex<double> *CXXPH_RESTRICT dest,
                                  const std::complex<double> *CXXPH_RESTRICT src,
                                  const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    // A = a0 + {a1}i
    // B = b0 + {b1}i
    // (A * B) = (a0*b0 - a1*b1) + {(a0*b1 + a1*b0)}i
    double *CXXPH_RESTRICT dd = reinterpret_cast<double *>(const_cast<std::complex<double> *>(dest));
    double *CXXPH_RESTRICT ds = reinterpret_cast<double *>(const_cast<std::complex<double> *>(src));
    double *CXXPH_RESTRICT dx = reinterpret_cast<double *>(const_cast<std::complex<double> *>(x));

    for (int i = 0; i < n; ++i) {
        const __m128d a = _mm_load_pd(&ds[2 * i]);
        const __m128d b = _mm_load_pd(&dx[2 * i]);

        const __m128d ar = _mm_unpacklo_pd(a, a);
        const __m128d ai = _mm_unpackhi_pd(a, a);
        const __m128d bs = _mm_shuffle_pd(b, b, _MM_SHUFFLE2(0, 1));

        const __m128d t0 = _mm_mul_pd(ar, b);
        const __m128d t1 = _mm_mul_pd(ai, bs);

        const __m128d ans = _mm_addsub_pd(t0, t1);

        _mm_store_pd(&dd[2 * i], ans);
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
static void sse_conj_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{

    float *f_src_dest = reinterpret_cast<float *>(src_dest);
    CXXPH_ALIGNAS(16) static const float conj_coeffs[4] = { 1.0f, -1.0f, 1.0f, -1.0f };

    if (n == 0) {
        return;
    }

    const int n1 = n >> 3;
    const int n2 = n & 0x7;

    const __m128 m_conj_coeffs = _mm_load_ps(&conj_coeffs[0]);

    for (int i = 0; i < n1; ++i) {
        const __m128 a = _mm_load_ps(&f_src_dest[0]);
        const __m128 b = _mm_load_ps(&f_src_dest[4]);
        const __m128 c = _mm_load_ps(&f_src_dest[8]);
        const __m128 d = _mm_load_ps(&f_src_dest[12]);

        const __m128 xa = _mm_mul_ps(a, m_conj_coeffs);
        const __m128 xb = _mm_mul_ps(b, m_conj_coeffs);
        const __m128 xc = _mm_mul_ps(c, m_conj_coeffs);
        const __m128 xd = _mm_mul_ps(d, m_conj_coeffs);

        _mm_store_ps(&f_src_dest[0], xa);
        _mm_store_ps(&f_src_dest[4], xb);
        _mm_store_ps(&f_src_dest[8], xc);
        _mm_store_ps(&f_src_dest[12], xd);

        f_src_dest += (4 * 4);
    }

    for (int i = 0; i < n2; ++i) {
        f_src_dest[1] = -f_src_dest[1];
        f_src_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
static void sse_conj_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src,
                             int n) CXXPH_NOEXCEPT
{

    float *f_dest = reinterpret_cast<float *>(dest);
    const float *f_src = reinterpret_cast<const float *>(src);
    CXXPH_ALIGNAS(16) static const float conj_coeffs[4] = { 1.0f, -1.0f, 1.0f, -1.0f };

    if (n == 0) {
        return;
    }

    const int n1 = n >> 3;
    const int n2 = n & 0x7;

    const __m128 m_conj_coeffs = _mm_load_ps(&conj_coeffs[0]);

    for (int i = 0; i < n1; ++i) {
        const __m128 a = _mm_load_ps(&f_src[0]);
        const __m128 b = _mm_load_ps(&f_src[4]);
        const __m128 c = _mm_load_ps(&f_src[8]);
        const __m128 d = _mm_load_ps(&f_src[12]);

        const __m128 xa = _mm_mul_ps(a, m_conj_coeffs);
        const __m128 xb = _mm_mul_ps(b, m_conj_coeffs);
        const __m128 xc = _mm_mul_ps(c, m_conj_coeffs);
        const __m128 xd = _mm_mul_ps(d, m_conj_coeffs);

        _mm_store_ps(&f_dest[0], xa);
        _mm_store_ps(&f_dest[4], xb);
        _mm_store_ps(&f_dest[8], xc);
        _mm_store_ps(&f_dest[12], xd);

        f_src += (4 * 4);
        f_dest += (4 * 4);
    }

    for (int i = 0; i < n2; ++i) {
        f_dest[0] = f_src[0];
        f_dest[1] = -f_src[1];
        f_src += 2;
        f_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
static void sse2_conj_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{

    double *d_src_dest = reinterpret_cast<double *>(src_dest);
    CXXPH_ALIGNAS(16) static const double conj_coeffs[4] = { 1.0, -1.0f };

    const int n1 = n >> 2;
    const int n2 = n & 0x3;

    const __m128d m_conj_coeffs = _mm_load_pd(&conj_coeffs[0]);

    for (int i = 0; i < n1; ++i) {
        const __m128d a = _mm_load_pd(&d_src_dest[0]);
        const __m128d b = _mm_load_pd(&d_src_dest[2]);
        const __m128d c = _mm_load_pd(&d_src_dest[4]);
        const __m128d d = _mm_load_pd(&d_src_dest[6]);

        const __m128d xa = _mm_mul_pd(a, m_conj_coeffs);
        const __m128d xb = _mm_mul_pd(b, m_conj_coeffs);
        const __m128d xc = _mm_mul_pd(c, m_conj_coeffs);
        const __m128d xd = _mm_mul_pd(d, m_conj_coeffs);

        _mm_store_pd(&d_src_dest[0], xa);
        _mm_store_pd(&d_src_dest[2], xb);
        _mm_store_pd(&d_src_dest[4], xc);
        _mm_store_pd(&d_src_dest[6], xd);

        d_src_dest += (2 * 4);
    }

    for (int i = 0; i < n2; ++i) {
        d_src_dest[1] = -d_src_dest[1];
        d_src_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
static void sse2_conj_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                              int n) CXXPH_NOEXCEPT
{

    double *d_dest = reinterpret_cast<double *>(dest);
    const double *d_src = reinterpret_cast<const double *>(src);
    CXXPH_ALIGNAS(16) static const double conj_coeffs[4] = { 1.0, -1.0f };

    const int n1 = n >> 2;
    const int n2 = n & 0x3;

    const __m128d m_conj_coeffs = _mm_load_pd(&conj_coeffs[0]);

    for (int i = 0; i < n1; ++i) {
        const __m128d a = _mm_load_pd(&d_src[0]);
        const __m128d b = _mm_load_pd(&d_src[2]);
        const __m128d c = _mm_load_pd(&d_src[4]);
        const __m128d d = _mm_load_pd(&d_src[6]);

        const __m128d xa = _mm_mul_pd(a, m_conj_coeffs);
        const __m128d xb = _mm_mul_pd(b, m_conj_coeffs);
        const __m128d xc = _mm_mul_pd(c, m_conj_coeffs);
        const __m128d xd = _mm_mul_pd(d, m_conj_coeffs);

        _mm_store_pd(&d_dest[0], xa);
        _mm_store_pd(&d_dest[2], xb);
        _mm_store_pd(&d_dest[4], xc);
        _mm_store_pd(&d_dest[6], xd);

        d_src += (2 * 4);
        d_dest += (2 * 4);
    }

    for (int i = 0; i < n2; ++i) {
        d_dest[0] = d_src[0];
        d_dest[1] = -d_src[1];
        d_src += 2;
        d_dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
static void sse_mirror_conj_aligned(std::complex<float> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{

    CXXPH_ALIGNAS(16) static const float conj_coeffs[4] = { 1.0f, -1.0f, 1.0f, -1.0f };
    const float *src = reinterpret_cast<const float *>(&center[-1]);
    float *dest = reinterpret_cast<float *>(&center[+1]);

    if (n == 0) {
        return;
    }

    const int n1 = 1;
    const int n2 = (n - n1) >> 3;
    const int n3 = (n - n1) & 0x7;

    // adjust alignment
    for (int i = 0; i < n1; ++i) {
        dest[0] = src[0];
        dest[1] = -src[1];
        src -= 2;
        dest += 2;
    }

    const __m128 m_conj_coeffs = _mm_load_ps(&conj_coeffs[0]);

    if (n2 > 0) {
        __m128 s4 = _mm_load_ps(&src[0]);

        for (int i = 0; i < n2; ++i) {
            src -= (4 * 4);

            const __m128 s0 = _mm_load_ps(&src[0]);
            const __m128 s1 = _mm_load_ps(&src[4]);
            const __m128 s2 = _mm_load_ps(&src[8]);
            const __m128 s3 = _mm_load_ps(&src[12]);

            const __m128 t0 = _mm_shuffle_ps(s1, s0, _MM_SHUFFLE(3, 2, 1, 0));
            const __m128 t1 = _mm_shuffle_ps(s2, s1, _MM_SHUFFLE(3, 2, 1, 0));
            const __m128 t2 = _mm_shuffle_ps(s3, s2, _MM_SHUFFLE(3, 2, 1, 0));
            const __m128 t3 = _mm_shuffle_ps(s4, s3, _MM_SHUFFLE(3, 2, 1, 0));

            const __m128 c0 = _mm_mul_ps(t0, m_conj_coeffs);
            const __m128 c1 = _mm_mul_ps(t1, m_conj_coeffs);
            const __m128 c2 = _mm_mul_ps(t2, m_conj_coeffs);
            const __m128 c3 = _mm_mul_ps(t3, m_conj_coeffs);

            _mm_store_ps(&dest[0], c3);
            _mm_store_ps(&dest[4], c2);
            _mm_store_ps(&dest[8], c1);
            _mm_store_ps(&dest[12], c0);

            s4 = s0;
            dest += (4 * 4);
        }
    }

    for (int i = 0; i < n3; ++i) {
        dest[0] = src[0];
        dest[1] = -src[1];
        src -= 2;
        dest += 2;
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
static void sse2_mirror_conj_aligned(std::complex<double> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{

    CXXPH_ALIGNAS(16) static const double conj_coeffs[4] = { 1.0, -1.0f };

    const double *src = reinterpret_cast<const double *>(&center[-1]);
    double *dest = reinterpret_cast<double *>(&center[+1]);

    const int n1 = n >> 2;
    const int n2 = n & 0x3;

    const __m128d m_conj_coeffs = _mm_load_pd(&conj_coeffs[0]);

    for (int i = 0; i < n1; ++i) {
        src -= (2 * 4);

        const __m128d a = _mm_load_pd(&src[2]);
        const __m128d b = _mm_load_pd(&src[4]);
        const __m128d c = _mm_load_pd(&src[6]);
        const __m128d d = _mm_load_pd(&src[8]);

        const __m128d xa = _mm_mul_pd(a, m_conj_coeffs);
        const __m128d xb = _mm_mul_pd(b, m_conj_coeffs);
        const __m128d xc = _mm_mul_pd(c, m_conj_coeffs);
        const __m128d xd = _mm_mul_pd(d, m_conj_coeffs);

        _mm_store_pd(&dest[0], xd);
        _mm_store_pd(&dest[2], xc);
        _mm_store_pd(&dest[4], xb);
        _mm_store_pd(&dest[6], xa);

        dest += (2 * 4);
    }

    for (int i = 0; i < n2; ++i) {
        dest[0] = src[0];
        dest[1] = -src[1];
        src -= 2;
        dest += 2;
    }
}
#endif

//
// exposed functions
//

void multiply_scaler_aligned(float *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse()) {
        sse_multiply_scaler_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(src_dest, x, n);
}

void multiply_scaler_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, float x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, CXXPH_PLATFORM_SIMD_ALIGNMENT);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse()) {
        sse_multiply_scaler_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(dest, src, x, n);
}

void multiply_scaler_aligned(double *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
    if (cxxporthelper::platform_info::support_sse2()) {
        sse2_multiply_scaler_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(src_dest, x, n);
}

void multiply_scaler_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src, double x, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, CXXPH_PLATFORM_SIMD_ALIGNMENT);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, CXXPH_PLATFORM_SIMD_ALIGNMENT);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
    if (cxxporthelper::platform_info::support_sse2()) {
        sse2_multiply_scaler_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(dest, src, x, n);
}

void multiply_aligned(float *CXXPH_RESTRICT src_dest, const float *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse()) {
        sse_multiply_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_aligned(src_dest, x, n);
}

void multiply_aligned(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, const float *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse()) {
        sse_multiply_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_aligned(dest, src, x, n);
}

void multiply_aligned(double *CXXPH_RESTRICT src_dest, const double *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
    if (cxxporthelper::platform_info::support_sse2()) {
        sse2_multiply_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_aligned(src_dest, x, n);
}

void multiply_aligned(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src, const double *CXXPH_RESTRICT x,
                      int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse2()) {
        sse2_multiply_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_aligned(dest, src, x, n);
}

void multiply_scaler_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, float x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse()) {
        sse_multiply_scaler_aligned(reinterpret_cast<float *>(src_dest), x, (2 * n));
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(src_dest, x, n);
}

void multiply_scaler_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src,
                             float x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse()) {
        sse_multiply_scaler_aligned(reinterpret_cast<float *>(dest), reinterpret_cast<const float *>(src), x, (2 * n));
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(dest, src, x, n);
}

void multiply_scaler_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, double x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
    if (cxxporthelper::platform_info::support_sse2()) {
        sse2_multiply_scaler_aligned(reinterpret_cast<double *>(src_dest), x, (2 * n));
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(src_dest, x, n);
}

void multiply_scaler_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                             double x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
    if (cxxporthelper::platform_info::support_sse2()) {
        sse2_multiply_scaler_aligned(reinterpret_cast<double *>(dest), reinterpret_cast<const double *>(src), x,
                                     (2 * n));
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_scaler_aligned(dest, src, x, n);
}

void multiply_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, const std::complex<float> *CXXPH_RESTRICT x, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE && CXXPH_COMPILER_SUPPORTS_X86_SSE3
    if (cxxporthelper::platform_info::support_sse() && cxxporthelper::platform_info::support_sse3()) {
        sse3_multiply_aligned(src_dest, x, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse()) {
        sse_multiply_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_aligned(src_dest, x, n);
}

void multiply_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src,
                      const std::complex<float> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{

    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE && CXXPH_COMPILER_SUPPORTS_X86_SSE3
    if (cxxporthelper::platform_info::support_sse() && cxxporthelper::platform_info::support_sse3()) {
        sse3_multiply_aligned(dest, src, x, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse()) {
        sse_multiply_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_aligned(dest, src, x, n);
}

void multiply_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, const std::complex<double> *CXXPH_RESTRICT x,
                      int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2 && CXXPH_COMPILER_SUPPORTS_X86_SSE3
    if (cxxporthelper::platform_info::support_sse2() && cxxporthelper::platform_info::support_sse3()) {
        sse3_multiply_aligned(src_dest, x, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
    if (cxxporthelper::platform_info::support_sse2()) {
        sse2_multiply_aligned(src_dest, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_aligned(src_dest, x, n);
}

void multiply_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src,
                      const std::complex<double> *CXXPH_RESTRICT x, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(x, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2 && CXXPH_COMPILER_SUPPORTS_X86_SSE3
    if (cxxporthelper::platform_info::support_sse2() && cxxporthelper::platform_info::support_sse3()) {
        sse3_multiply_aligned(dest, src, x, n);
        return;
    }
#endif
#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
    if (cxxporthelper::platform_info::support_sse2()) {
        sse2_multiply_aligned(dest, src, x, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::multiply_aligned(dest, src, x, n);
}

void conj_aligned(std::complex<float> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse()) {
        sse_conj_aligned(src_dest, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::conj_aligned(src_dest, n);
}

void conj_aligned(std::complex<float> *CXXPH_RESTRICT dest, const std::complex<float> *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse()) {
        sse_conj_aligned(dest, src, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::conj_aligned(dest, src, n);
}

void conj_aligned(std::complex<double> *CXXPH_RESTRICT src_dest, int n) CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(src_dest, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
    if (cxxporthelper::platform_info::support_sse2()) {
        sse2_conj_aligned(src_dest, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::conj_aligned(src_dest, n);
}

void conj_aligned(std::complex<double> *CXXPH_RESTRICT dest, const std::complex<double> *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
    CXXDASP_UTIL_ASSUME_ALIGNED(dest, 16);
    CXXDASP_UTIL_ASSUME_ALIGNED(src, 16);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
    if (cxxporthelper::platform_info::support_sse2()) {
        sse2_conj_aligned(dest, src, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::conj_aligned(dest, src, n);
}

void mirror_conj_aligned(std::complex<float> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (cxxporthelper::platform_info::support_sse()) {
        sse_mirror_conj_aligned(center, n);
        return;
    }
#endif

    // TODO auto vectorization may generates SSE instructions
    cxxdasp::utils::impl_general::mirror_conj_aligned(center, n);
}

void mirror_conj_aligned(std::complex<double> *CXXPH_RESTRICT center, int n) CXXPH_NOEXCEPT
{
#if CXXPH_COMPILER_SUPPORTS_X86_SSE2
    if (cxxporthelper::platform_info::support_sse2()) {
        sse2_mirror_conj_aligned(center, n);
        return;
    }
#endif

    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::mirror_conj_aligned(center, n);
}

void interleave(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src1, const float *CXXPH_RESTRICT src2, int n)
    CXXPH_NOEXCEPT
{
    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::interleave(dest, src1, src2, n);
}

void interleave(double *CXXPH_RESTRICT dest, const double *CXXPH_RESTRICT src1, const double *CXXPH_RESTRICT src2,
                int n) CXXPH_NOEXCEPT
{
    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::interleave(dest, src1, src2, n);
}

void deinterleave(float *CXXPH_RESTRICT dest1, float *CXXPH_RESTRICT dest2, const float *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::deinterleave(dest1, dest2, src, n);
}

void deinterleave(double *CXXPH_RESTRICT dest1, double *CXXPH_RESTRICT dest2, const double *CXXPH_RESTRICT src, int n)
    CXXPH_NOEXCEPT
{
    // TODO auto vectorization may generates NEON instructions
    cxxdasp::utils::impl_general::deinterleave(dest1, dest2, src, n);
}

} // namespace impl_sse
} // namespace utils
} // namespace cxxdasp

#endif // (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
