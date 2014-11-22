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

/**
 * Bluestein's FFT classes
 *
 * This FFT algorithm can handle non-power-of-two size FFT.
 *
 * Wikipedia - Bluestein's FFT algorithm
 *   http://en.wikipedia.org/wiki/Bluestein%27s_FFT_algorithm
 */
#ifndef CXXDASP_FFT_BLUESTEIN_FFT_HPP_
#define CXXDASP_FFT_BLUESTEIN_FFT_HPP_

#include <cstring>

#include <cxxporthelper/cmath>
#include <cxxporthelper/utility>
#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/fft/types.hpp>
#include <cxxdasp/utils/utils.hpp>

namespace cxxdasp {
namespace fft {
namespace bluestein {

/// @cond INTERNAL_FIELD
template <typename Tin, typename Tout, class Tbackend>
struct bluestein_impl_helper {
    typedef typename Tbackend::fft_real_t fft_real_t;
    typedef typename Tbackend::fft_complex_t fft_complex_t;
    typedef fft<fft_complex_t, fft_complex_t, typename Tbackend::forward> backend_forward_fft;
    typedef fft<fft_complex_t, fft_complex_t, typename Tbackend::backward> backend_backward_fft;

    bluestein_impl_helper() = delete;

    //
    // TODO: These utility functions can be optimized by SIMD
    //

    // make the 'A' and 'B' arrays
    //
    //  A_{n} = a_{n}  --- (0 ≤ n < N)
    //  A_{n} = 0      --- otherwise
    //
    //  a_{n} = x_{n} e^{ -\frac{\pi i}{N} n^2 } = x_{n} conj(b_{n})
    //
    //  B_{0} = b_{0}
    //  B_{n} = B_{M–n} = b_{n}  --- (0 < n < N)
    //  B_{n} = 0                --- otherwise
    //
    //  b_{n} = e^{ \frac{\pi i}{N} n^2 }
    //
    static void make_a_array_c2c(fft_complex_t *CXXPH_RESTRICT a, const fft_complex_t *CXXPH_RESTRICT x,
                                 const fft_complex_t *CXXPH_RESTRICT b_conj, int n, int m) CXXPH_NOEXCEPT
    {

        for (int i = 0; i < n; ++i) {
            a[i] = x[i] * b_conj[i];
        }
        for (int i = n; i < m; ++i) {
            a[i] = fft_complex_t(0);
        }
    }

    static void make_a_array_r2c(fft_complex_t *CXXPH_RESTRICT a, const fft_real_t *CXXPH_RESTRICT x,
                                 const fft_complex_t *CXXPH_RESTRICT b_conj, int n, int m) CXXPH_NOEXCEPT
    {

        for (int i = 0; i < n; ++i) {
            a[i] = fft_complex_t(x[i], 0) * b_conj[i];
        }
        for (int i = n; i < m; ++i) {
            a[i] = fft_complex_t(0);
        }
    }

    static void make_a_array_c2r(fft_complex_t *CXXPH_RESTRICT a, const fft_complex_t *CXXPH_RESTRICT x,
                                 const fft_complex_t *CXXPH_RESTRICT b_conj, int n, int m, int nx) CXXPH_NOEXCEPT
    {

        for (int i = 0; i < nx; ++i) {
            a[i] = x[i] * b_conj[i];
        }
        if ((n % 2) == 0) {
            for (int i = nx; i < n; ++i) {
                a[i] = std::conj(x[nx + (nx - i) - 2]) * b_conj[i];
            }
        } else {
            for (int i = nx; i < n; ++i) {
                a[i] = std::conj(x[nx + (nx - i) - 1]) * b_conj[i];
            }
        }
        for (int i = n; i < m; ++i) {
            a[i] = fft_complex_t(0);
        }
    }

    static void make_b_array(fft_complex_t *CXXPH_RESTRICT b, int n, int m, int sign) CXXPH_NOEXCEPT
    {

        const double theta_coeff = -((M_PI / n) * sign);

        b[0] = fft_complex_t(1.0, 0.0);
        for (int i = 1; i < n; ++i) {
            const double theta = theta_coeff * (i * i);
            b[i] = b[m - i] = fft_complex_t(static_cast<fft_real_t>(cos(theta)), static_cast<fft_real_t>(sin(theta)));
        }
        for (int i = n; i < (m - n); ++i) {
            b[i] = fft_complex_t(0);
        }
    }

    static void calc_output_c2c(fft_complex_t *CXXPH_RESTRICT dest, const fft_complex_t *CXXPH_RESTRICT a,
                                const fft_complex_t *CXXPH_RESTRICT b, fft_real_t scale, int n) CXXPH_NOEXCEPT
    {

        for (int i = 0; i < n; i++) {
            dest[i] = scale * a[i] * b[i];
        }
    }

    static void calc_output_r2c(fft_complex_t *CXXPH_RESTRICT dest, const fft_complex_t *CXXPH_RESTRICT a,
                                const fft_complex_t *CXXPH_RESTRICT b, fft_real_t scale, int n) CXXPH_NOEXCEPT
    {

        for (int i = 0; i < n; i++) {
            dest[i] = std::conj(scale * a[i] * b[i]);
        }
    }

    static void calc_output_c2r(fft_real_t *CXXPH_RESTRICT dest, const fft_complex_t *CXXPH_RESTRICT a,
                                const fft_complex_t *CXXPH_RESTRICT b, fft_real_t scale, int n) CXXPH_NOEXCEPT
    {

        for (int i = 0; i < n; i++) {
            dest[i] = scale * (a[i] * b[i]).real();
        }
    }
};
/// @endcond

/**
 * Forward (Complex) FFT
 *
 * @tparam Tin input data type
 * @tparam Tout output data type
 * @tparam Tbackend base FFT backend class
 */
template <typename Tin, typename Tout, class Tbackend>
class forward : public fft_backend_base<Tin, Tout> {
    typedef fft_backend_base<Tin, Tout> base;
    typedef bluestein_impl_helper<Tin, Tout, Tbackend> helper;
    typedef typename helper::fft_real_t fft_real_t;
    typedef typename helper::fft_complex_t fft_complex_t;
    typedef typename helper::backend_forward_fft backend_forward_fft;
    typedef typename helper::backend_backward_fft backend_backward_fft;

    forward(const forward &) = delete;
    forward &operator=(const forward &) = delete;

public:
    /**
     * Constructor.
     *
     * @param n [in] FFT size
     * @param in [in] Input buffer
     * @param out [in] Output buffer
     */
    forward(int n, Tin *in, Tout *out)
        : base(n, in, out, 1), m_(utils::next_pow_of_two(2 * n - 1)), normalize_coeff_(1), b_conj_(), f_b_(),
          fft_b_src_(), fft_b_dest_f_src_(), fft_f_dest_(), fft_b_(), fft_f_()
    {
        // allocate memory blocks
        cxxporthelper::aligned_memory<fft_complex_t> b_conj(m_);
        cxxporthelper::aligned_memory<fft_complex_t> f_b(m_);
        cxxporthelper::aligned_memory<fft_complex_t> fft_b_src(m_);
        cxxporthelper::aligned_memory<fft_complex_t> fft_b_dest_f_src(m_);
        cxxporthelper::aligned_memory<fft_complex_t> fft_f_dest(m_);

        backend_backward_fft fft_b(m_, &fft_b_src[0], &fft_b_dest_f_src[0]);
        backend_forward_fft fft_f(m_, &fft_b_dest_f_src[0], &fft_f_dest[0]);

        // make the 'B' array and perform FFT
        helper::make_b_array(&fft_b_src[0], base::n_, m_, -1);
        fft_b.execute();
        ::memcpy(&f_b[0], &fft_b_dest_f_src[0], sizeof(fft_complex_t) * m_);

        // conjugate B
        utils::conj_aligned(&b_conj[0], &fft_b_src[0], m_);

        // update fields
        b_conj_ = std::move(b_conj);
        f_b_ = std::move(f_b);
        fft_b_src_ = std::move(fft_b_src);
        fft_b_dest_f_src_ = std::move(fft_b_dest_f_src);
        fft_f_dest_ = std::move(fft_f_dest);
        fft_f_ = std::move(fft_f);
        fft_b_ = std::move(fft_b);

        normalize_coeff_ = static_cast<fft_real_t>(1) / fft_b_.scale();
    }

    /**
     * Destructor.
     */
    virtual ~forward() {}

    /**
     * Execute FFT.
     */
    void execute() CXXPH_NOEXCEPT
    {
        const int N = base::n_;
        const int M = m_;

        fft_complex_t *x = base::in_;
        fft_complex_t *y = base::out_;

        cxxporthelper::aligned_memory<fft_complex_t> &A = fft_b_src_;
        const cxxporthelper::aligned_memory<fft_complex_t> &B_CONJ = b_conj_;

        // make the 'A' array and perform FFT
        helper::make_a_array_c2c(&A[0], &x[0], &B_CONJ[0], N, M);

        cxxporthelper::aligned_memory<fft_complex_t> &F_A = fft_b_dest_f_src_; // and used for F_A x F_B
        const cxxporthelper::aligned_memory<fft_complex_t> &F_B = f_b_;

        fft_b_.execute();

        // multply
        utils::multiply_aligned(&F_A[0], &F_B[0], M);

        fft_f_.execute();

        const cxxporthelper::aligned_memory<fft_complex_t> &AB = fft_f_dest_;

        // multiply phase factors conj(b_{k})
        helper::calc_output_c2c(&y[0], &B_CONJ[0], &AB[0], normalize_coeff_, N);
    }

private:
    /// @cond INTERNAL_FIELD
    const int m_;
    fft_real_t normalize_coeff_;

    cxxporthelper::aligned_memory<fft_complex_t> b_conj_;
    cxxporthelper::aligned_memory<fft_complex_t> f_b_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_b_src_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_b_dest_f_src_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_f_dest_;
    backend_backward_fft fft_b_;
    backend_forward_fft fft_f_;
    /// @endcond
};

/**
 * Backward (Complex) FFT
 *
 * @tparam Tin input data type
 * @tparam Tout output data type
 * @tparam Tbackend base FFT backend class
 */
template <typename Tin, typename Tout, class Tbackend>
class backward : public fft_backend_base<Tin, Tout> {
    typedef fft_backend_base<Tin, Tout> base;
    typedef bluestein_impl_helper<Tin, Tout, Tbackend> helper;
    typedef typename helper::fft_real_t fft_real_t;
    typedef typename helper::fft_complex_t fft_complex_t;
    typedef typename helper::backend_forward_fft backend_forward_fft;
    typedef typename helper::backend_backward_fft backend_backward_fft;

    backward(const backward &) = delete;
    backward &operator=(const backward &) = delete;

public:
    /**
     * Constructor.
     *
     * @param n [in] FFT size
     * @param in [in] Input buffer
     * @param out [in] Output buffer
     */
    backward(int n, Tin *in, Tout *out)
        : base(n, in, out, n), m_(utils::next_pow_of_two(2 * n - 1)), normalize_coeff_(1), b_conj_(), f_b_(),
          fft_f_src_(), fft_f_dest_b_src_(), fft_b_dest_(), fft_f_(), fft_b_()
    {
        // allocate memory blocks
        cxxporthelper::aligned_memory<fft_complex_t> b_conj(m_, FFT_MEMORY_ALIGNMENT);
        cxxporthelper::aligned_memory<fft_complex_t> f_b(m_, FFT_MEMORY_ALIGNMENT);
        cxxporthelper::aligned_memory<fft_complex_t> fft_f_src(m_, FFT_MEMORY_ALIGNMENT);
        cxxporthelper::aligned_memory<fft_complex_t> fft_f_dest_b_src(m_, FFT_MEMORY_ALIGNMENT);
        cxxporthelper::aligned_memory<fft_complex_t> fft_b_dest(m_, FFT_MEMORY_ALIGNMENT);

        backend_forward_fft fft_f(m_, &fft_f_src[0], &fft_f_dest_b_src[0]);
        backend_backward_fft fft_b(m_, &fft_f_dest_b_src[0], &fft_b_dest[0]);

        // make the 'B' array and perform FFT
        helper::make_b_array(&fft_f_src[0], base::n_, m_, 1);
        fft_f.execute();
        ::memcpy(&f_b[0], &fft_f_dest_b_src[0], sizeof(fft_complex_t) * m_);

        // conjugate B
        utils::conj_aligned(&b_conj[0], &fft_f_src[0], m_);

        // update fields
        b_conj_ = std::move(b_conj);
        f_b_ = std::move(f_b);
        fft_f_src_ = std::move(fft_f_src);
        fft_f_dest_b_src_ = std::move(fft_f_dest_b_src);
        fft_b_dest_ = std::move(fft_b_dest);
        fft_f_ = std::move(fft_f);
        fft_b_ = std::move(fft_b);

        normalize_coeff_ = static_cast<fft_real_t>(1) / fft_b_.scale();
    }

    /**
     * Destructor.
     */
    virtual ~backward() {}

    /**
     * Execute FFT.
     */
    void execute() CXXPH_NOEXCEPT
    {
        const int N = base::n_;
        const int M = m_;

        fft_complex_t *x = base::in_;
        fft_complex_t *y = base::out_;

        cxxporthelper::aligned_memory<fft_complex_t> &A = fft_f_src_;
        const cxxporthelper::aligned_memory<fft_complex_t> &B_CONJ = b_conj_;

        // make the 'A' array and perform FFT
        helper::make_a_array_c2c(&A[0], &x[0], &B_CONJ[0], N, M);

        cxxporthelper::aligned_memory<fft_complex_t> &F_A = fft_f_dest_b_src_; // and used for F_A x F_B
        const cxxporthelper::aligned_memory<fft_complex_t> &F_B = f_b_;

        fft_f_.execute();

        // multply
        utils::multiply_aligned(&F_A[0], &F_B[0], M);

        fft_b_.execute();

        const cxxporthelper::aligned_memory<fft_complex_t> &AB = fft_b_dest_;

        // multiply phase factors conj(b_{k})
        helper::calc_output_c2c(&y[0], &B_CONJ[0], &AB[0], normalize_coeff_, N);
    }

private:
    /// @cond INTERNAL_FIELD
    const int m_;
    fft_real_t normalize_coeff_;

    cxxporthelper::aligned_memory<fft_complex_t> b_conj_;
    cxxporthelper::aligned_memory<fft_complex_t> f_b_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_f_src_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_f_dest_b_src_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_b_dest_;
    backend_forward_fft fft_f_;
    backend_backward_fft fft_b_;
    /// @endcond
};

/**
 * Forward (Real) FFT
 *
 * @tparam Tin input data type
 * @tparam Tout output data type
 * @tparam Tbackend base FFT backend class
 */
template <typename Tin, typename Tout, class Tbackend>
class forward_real : public fft_backend_base<Tin, Tout> {
    typedef fft_backend_base<Tin, Tout> base;
    typedef bluestein_impl_helper<Tin, Tout, Tbackend> helper;
    typedef typename helper::fft_real_t fft_real_t;
    typedef typename helper::fft_complex_t fft_complex_t;
    typedef typename helper::backend_forward_fft backend_forward_fft;
    typedef typename helper::backend_backward_fft backend_backward_fft;

    forward_real(const forward_real &) = delete;
    forward_real &operator=(const forward_real &) = delete;

public:
    /**
     * Constructor.
     *
     * @param n [in] FFT size
     * @param in [in] Input buffer
     * @param out [in] Output buffer
     */
    forward_real(int n, Tin *in, Tout *out)
        : base(n, in, out, 1), m_(utils::next_pow_of_two(2 * n - 1)), normalize_coeff_(1), b_conj_(), f_b_(),
          fft_f_src_(), fft_f_dest_b_src_(), fft_b_dest_(), fft_f_(), fft_b_()
    {
        // allocate memory blocks
        cxxporthelper::aligned_memory<fft_complex_t> b_conj(m_);
        cxxporthelper::aligned_memory<fft_complex_t> f_b(m_);
        cxxporthelper::aligned_memory<fft_complex_t> fft_f_src(m_);
        cxxporthelper::aligned_memory<fft_complex_t> fft_f_dest_b_src(m_);
        cxxporthelper::aligned_memory<fft_complex_t> fft_b_dest(m_);

        backend_forward_fft fft_f(m_, &fft_f_src[0], &fft_f_dest_b_src[0]);
        backend_backward_fft fft_b(m_, &fft_f_dest_b_src[0], &fft_b_dest[0]);

        // make the 'B' array and perform FFT
        helper::make_b_array(&fft_f_src[0], base::n_, m_, 1);
        fft_f.execute();
        ::memcpy(&f_b[0], &fft_f_dest_b_src[0], sizeof(fft_complex_t) * m_);

        // conjugate B
        utils::conj_aligned(&b_conj[0], &fft_f_src[0], m_);

        // update fields
        b_conj_ = std::move(b_conj);
        f_b_ = std::move(f_b);
        fft_f_src_ = std::move(fft_f_src);
        fft_f_dest_b_src_ = std::move(fft_f_dest_b_src);
        fft_b_dest_ = std::move(fft_b_dest);
        fft_f_ = std::move(fft_f);
        fft_b_ = std::move(fft_b);

        normalize_coeff_ = static_cast<fft_real_t>(1) / fft_b_.scale();
    }

    /**
     * Destructor.
     */
    virtual ~forward_real() {}

    /**
     * Execute FFT.
     */
    void execute() CXXPH_NOEXCEPT
    {
        const int N = base::n_;
        const int Ny = utils::forward_fft_real_num_outputs(N);
        const int M = m_;

        fft_real_t *x = base::in_;
        fft_complex_t *y = base::out_;

        cxxporthelper::aligned_memory<fft_complex_t> &A = fft_f_src_;
        const cxxporthelper::aligned_memory<fft_complex_t> &B_CONJ = b_conj_;

        // make the 'A' array and perform FFT
        helper::make_a_array_r2c(&A[0], &x[0], &B_CONJ[0], N, M);

        cxxporthelper::aligned_memory<fft_complex_t> &F_A = fft_f_dest_b_src_; // and used for F_A x F_B
        const cxxporthelper::aligned_memory<fft_complex_t> &F_B = f_b_;

        fft_f_.execute();

        // multply
        utils::multiply_aligned(&F_A[0], &F_B[0], M);

        fft_b_.execute();

        const cxxporthelper::aligned_memory<fft_complex_t> &AB = fft_b_dest_;

        // multiply phase factors conj(b_{k})
        helper::calc_output_r2c(&y[0], &B_CONJ[0], &AB[0], normalize_coeff_, Ny);
    }

private:
    /// @cond INTERNAL_FIELD
    const int m_;
    fft_real_t normalize_coeff_;

    cxxporthelper::aligned_memory<fft_complex_t> b_conj_;
    cxxporthelper::aligned_memory<fft_complex_t> f_b_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_f_src_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_f_dest_b_src_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_b_dest_;
    backend_forward_fft fft_f_;
    backend_backward_fft fft_b_;
    /// @endcond
};

/**
 * Backward (Real) FFT
 *
 * @tparam Tin input data type
 * @tparam Tout output data type
 * @tparam Tbackend base FFT backend class
 */
template <typename Tin, typename Tout, class Tbackend>
class backward_real : public fft_backend_base<Tin, Tout> {
    typedef fft_backend_base<Tin, Tout> base;
    typedef bluestein_impl_helper<Tin, Tout, Tbackend> helper;
    typedef typename helper::fft_real_t fft_real_t;
    typedef typename helper::fft_complex_t fft_complex_t;
    typedef typename helper::backend_forward_fft backend_forward_fft;
    typedef typename helper::backend_backward_fft backend_backward_fft;

    backward_real(const backward_real &) = delete;
    backward_real &operator=(const backward_real &) = delete;

public:
    /**
     * Constructor.
     *
     * @param n [in] FFT size
     * @param in [in] Input buffer
     * @param out [in] Output buffer
     */
    backward_real(int n, Tin *in, Tout *out)
        : base(n, in, out, n), m_(utils::next_pow_of_two(2 * n - 1)), normalize_coeff_(1), b_conj_(), f_b_(),
          fft_f_src_(), fft_f_dest_b_src_(), fft_b_dest_(), fft_f_(), fft_b_()
    {
        // allocate memory blocks
        cxxporthelper::aligned_memory<fft_complex_t> b_conj(m_, FFT_MEMORY_ALIGNMENT);
        cxxporthelper::aligned_memory<fft_complex_t> f_b(m_, FFT_MEMORY_ALIGNMENT);
        cxxporthelper::aligned_memory<fft_complex_t> fft_f_src(m_, FFT_MEMORY_ALIGNMENT);
        cxxporthelper::aligned_memory<fft_complex_t> fft_f_dest_b_src(m_, FFT_MEMORY_ALIGNMENT);
        cxxporthelper::aligned_memory<fft_complex_t> fft_b_dest(m_, FFT_MEMORY_ALIGNMENT);

        backend_forward_fft fft_f(m_, &fft_f_src[0], &fft_f_dest_b_src[0]);
        backend_backward_fft fft_b(m_, &fft_f_dest_b_src[0], &fft_b_dest[0]);

        // make the 'B' array and perform FFT
        helper::make_b_array(&fft_f_src[0], base::n_, m_, 1);
        fft_f.execute();
        ::memcpy(&f_b[0], &fft_f_dest_b_src[0], sizeof(fft_complex_t) * m_);

        // conjugate B
        utils::conj_aligned(&b_conj[0], &fft_f_src[0], m_);

        // update fields
        b_conj_ = std::move(b_conj);
        f_b_ = std::move(f_b);
        fft_f_src_ = std::move(fft_f_src);
        fft_f_dest_b_src_ = std::move(fft_f_dest_b_src);
        fft_b_dest_ = std::move(fft_b_dest);
        fft_f_ = std::move(fft_f);
        fft_b_ = std::move(fft_b);

        normalize_coeff_ = static_cast<fft_real_t>(1) / fft_b_.scale();
    }

    /**
     * Destructor.
     */
    virtual ~backward_real() {}

    /**
     * Execute FFT.
     */
    void execute() CXXPH_NOEXCEPT
    {
        const int N = base::n_;
        const int Nx = utils::forward_fft_real_num_outputs(N);
        const int M = m_;

        fft_complex_t *x = base::in_;
        fft_real_t *y = base::out_;

        cxxporthelper::aligned_memory<fft_complex_t> &A = fft_f_src_;
        const cxxporthelper::aligned_memory<fft_complex_t> &B_CONJ = b_conj_;

        // make the 'A' array and perform FFT
        helper::make_a_array_c2r(&A[0], &x[0], &B_CONJ[0], N, M, Nx);

        cxxporthelper::aligned_memory<fft_complex_t> &F_A = fft_f_dest_b_src_; // and used for F_A x F_B
        const cxxporthelper::aligned_memory<fft_complex_t> &F_B = f_b_;

        fft_f_.execute();

        // multply
        utils::multiply_aligned(&F_A[0], &F_B[0], M);

        fft_b_.execute();

        const cxxporthelper::aligned_memory<fft_complex_t> &AB = fft_b_dest_;

        // multiply phase factors conj(b_{k})
        helper::calc_output_c2r(&y[0], &B_CONJ[0], &AB[0], normalize_coeff_, N);
    }

private:
    /// @cond INTERNAL_FIELD
    const int m_;
    fft_real_t normalize_coeff_;

    cxxporthelper::aligned_memory<fft_complex_t> b_conj_;
    cxxporthelper::aligned_memory<fft_complex_t> f_b_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_f_src_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_f_dest_b_src_;
    cxxporthelper::aligned_memory<fft_complex_t> fft_b_dest_;
    backend_forward_fft fft_f_;
    backend_backward_fft fft_b_;
    /// @endcond
};

} // namespace bluestein
} // namespace fft
} // namespace cxxdasp

#endif // CXXDASP_FFT_BLUESTEIN_FFT_HPP_
