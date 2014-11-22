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

#ifndef CXXDASP_FFT_FFT_BACKEND_GP_FFT_HPP_
#define CXXDASP_FFT_FFT_BACKEND_GP_FFT_HPP_

#include <cxxdasp/cxxdasp_config.hpp>

#if CXXDASP_USE_FFT_BACKEND_GP_FFT

#include <cstring>

#include <cxxporthelper/cmath>
#include <cxxporthelper/utility>
#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/fft/types.hpp>
#include <cxxdasp/utils/utils.hpp>

extern "C" void cdft(int n, int isgn, double *a, int *ip, double *w);
extern "C" void rdft(int n, int isgn, double *a, int *ip, double *w);

namespace cxxdasp {
namespace fft {
namespace backend {

namespace d {

/**
 * Double-precision FFT backend class
 * (using General Purpose FFT Package)
 */
struct gp_fft {
    /**
     * Real value type
     */
    typedef fftd_real_t fft_real_t;

    /**
     * Complex value type
     */
    typedef fftd_complex_t fft_complex_t;

    /// @cond INTERNAL_FIELD
    gp_fft() = delete;
    /// @endcond

    /**
     * Forward (Complex) FFT
     */
    class forward : public fft_backend_base<fft_complex_t, fft_complex_t> {
        typedef fft_backend_base<fft_complex_t, fft_complex_t> base;

    public:
        /**
         * Constructor.
         *
         * @param n [in] FFT size
         * @param in [in] Input buffer
         * @param out [in] Output buffer
         */
        forward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, 1), ip_(), w_()
        {
            // verify parameters
            assert(n >= 1 && utils::is_pow_of_two(n));

            const size_t ip_len = 2 + static_cast<size_t>(std::sqrt(static_cast<double>(n)));
            const size_t w_len = n / 2;

            // allocate memory blocks
            cxxporthelper::aligned_memory<int> ip(ip_len);
            cxxporthelper::aligned_memory<double> w(w_len);

            // initialize
            ip[0] = 0;
            ::cdft(2 * n, -1, reinterpret_cast<double *>(out), &ip[0], &w[0]);

            // update fields
            ip_ = std::move(ip);
            w_ = std::move(w);
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
            // copy input data to the output buffer
            ::memcpy(out_, in_, n_ * sizeof(double) * 2);

            // perform RDFT
            ::cdft(2 * n_, -1, reinterpret_cast<double *>(out_), &ip_[0], &w_[0]);
        }

    private:
        /// @cond INTERNAL_FIELD
        cxxporthelper::aligned_memory<int> ip_;
        cxxporthelper::aligned_memory<double> w_;
        /// @endcond
    };

    /**
     * Backward (Complex) FFT
     */
    class backward : public fft_backend_base<fft_complex_t, fft_complex_t> {
        typedef fft_backend_base<fft_complex_t, fft_complex_t> base;

    public:
        /**
         * Constructor.
         *
         * @param n [in] FFT size
         * @param in [in] Input buffer
         * @param out [in] Output buffer
         */
        backward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, n), ip_(), w_()
        {
            // verify parameters
            assert(n >= 1 && utils::is_pow_of_two(n));

            const size_t ip_len = 2 + static_cast<size_t>(std::sqrt(static_cast<double>(n)));
            const size_t w_len = n / 2;

            // allocate memory blocks
            cxxporthelper::aligned_memory<int> ip(ip_len);
            cxxporthelper::aligned_memory<double> w(w_len);

            // initialize
            ip[0] = 0;
            ::cdft(2 * n, 1, reinterpret_cast<double *>(out), &ip[0], &w[0]);

            // update fields
            ip_ = std::move(ip);
            w_ = std::move(w);
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
            // copy input data to the output buffer
            ::memcpy(out_, in_, n_ * sizeof(double) * 2);

            // perform RDFT
            ::cdft(2 * n_, 1, reinterpret_cast<double *>(out_), &ip_[0], &w_[0]);
        }

    private:
        /// @cond INTERNAL_FIELD
        cxxporthelper::aligned_memory<int> ip_;
        cxxporthelper::aligned_memory<double> w_;
        /// @endcond
    };

    /**
     * Forward (Real) FFT
     */
    class forward_real : public fft_backend_base<fft_real_t, fft_complex_t> {
        typedef fft_backend_base<fft_real_t, fft_complex_t> base;

    public:
        /**
         * Constructor.
         *
         * @param n [in] FFT size
         * @param in [in] Input buffer
         * @param out [in] Output buffer
         */
        forward_real(int n, fft_real_t *in, fft_complex_t *out) : base(n, in, out, 1), ip_(), w_()
        {
            // verify parameters
            assert(n >= 2 && utils::is_pow_of_two(n));

            const size_t ip_len = 2 + static_cast<size_t>(std::sqrt(static_cast<double>(n / 2)));
            const size_t w_len = n / 2;

            // allocate memory blocks
            cxxporthelper::aligned_memory<int> ip(ip_len);
            cxxporthelper::aligned_memory<double> w(w_len);

            // initialize
            ip[0] = 0;
            ::rdft(n, 1, reinterpret_cast<double *>(out), &ip[0], &w[0]);

            // update fields
            ip_ = std::move(ip);
            w_ = std::move(w);
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
            namespace compat = cxxporthelper::complex;

            // copy input data to the output buffer
            ::memcpy(out_, in_, n_ * sizeof(double));

            // perform RDFT
            ::rdft(n_, 1, reinterpret_cast<double *>(out_), &ip_[0], &w_[0]);

            // move N/2 th result to the corresponding position
            compat::set_real(out_[n_ / 2], out_[0].imag());
            compat::set_imag(out_[n_ / 2], 0.0);
            compat::set_imag(out_[0], 0.0);

            // conjugate
            utils::conj_aligned(out_, n_ / 2);
        }

    private:
        /// @cond INTERNAL_FIELD
        cxxporthelper::aligned_memory<int> ip_;
        cxxporthelper::aligned_memory<double> w_;
        /// @endcond
    };

    /**
     * Backward (Real) FFT
     */
    class backward_real : public fft_backend_base<fft_complex_t, fft_real_t> {
        typedef fft_backend_base<fft_complex_t, fft_real_t> base;

    public:
        /**
         * Constructor.
         *
         * @param n [in] FFT size
         * @param in [in] Input buffer
         * @param out [in] Output buffer
         */
        backward_real(int n, fft_complex_t *in, fft_real_t *out) : base(n, in, out, (n / 2)), ip_(), w_()
        {
            // verify parameters
            assert(n >= 2 && utils::is_pow_of_two(n));

            const size_t ip_len = 2 + static_cast<size_t>(std::sqrt(static_cast<double>(n / 2)));
            const size_t w_len = n / 2;

            // allocate memory blocks
            cxxporthelper::aligned_memory<int> ip(ip_len);
            cxxporthelper::aligned_memory<double> w(w_len);

            // initialize
            ip[0] = 0;
            ::rdft(n, -1, reinterpret_cast<double *>(out), &ip[0], &w[0]);

            // update fields
            ip_ = std::move(ip);
            w_ = std::move(w);
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
            // conjugate
            utils::conj_aligned(reinterpret_cast<std::complex<double> *>(out_), in_, n_ / 2);
            out_[1] = (in_[n_ / 2].real());

            // perform RDFT
            ::rdft(n_, -1, reinterpret_cast<double *>(out_), &ip_[0], &w_[0]);
        }

    private:
        /// @cond INTERNAL_FIELD
        cxxporthelper::aligned_memory<int> ip_;
        cxxporthelper::aligned_memory<double> w_;
        /// @endcond
    };
};

} // namespace d

} // namespace backend
} // namespace fft
} // namespace cxxdasp

#endif // CXXDASP_USE_FFT_BACKEND_GP_FFT
#endif // CXXDASP_FFT_FFT_BACKEND_GP_FFT_HPP_
