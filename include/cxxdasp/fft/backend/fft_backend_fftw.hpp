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

#ifndef CXXDASP_FFT_FFT_BACKEND_FFTW_HPP_
#define CXXDASP_FFT_FFT_BACKEND_FFTW_HPP_

#include <cxxdasp/cxxdasp_config.hpp>

#if CXXDASP_USE_FFT_BACKEND_FFTW || CXXDASP_USE_FFT_BACKEND_FFTWF

#include <cxxdasp/fft/types.hpp>

#include <fftw3.h>

namespace cxxdasp {
namespace fft {
namespace backend {

#if CXXDASP_USE_FFT_BACKEND_FFTWF
namespace f {

/**
 * Single-precision FFT backend class
 * (using FFTW library)
 */
struct fftw {
    /**
     * Real value type
     */
    typedef fftf_real_t fft_real_t;

    /**
     * Complex value type
     */
    typedef fftf_complex_t fft_complex_t;

    /// @cond INTERNAL_FIELD
    fftw() = delete;
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
        forward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, 1), plan_(0)
        {
            plan_ = ::fftwf_plan_dft_1d(n, reinterpret_cast<::fftwf_complex *>(in),
                                        reinterpret_cast<::fftwf_complex *>(out), FFTW_FORWARD,
                                        FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
        }

        /**
         * Destructor.
         */
        virtual ~forward()
        {
            ::fftwf_destroy_plan(plan_);
            plan_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT { ::fftwf_execute(plan_); }

    private:
        ::fftwf_plan plan_;
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
        backward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, n), plan_(0)
        {
            plan_ = ::fftwf_plan_dft_1d(n, reinterpret_cast<::fftwf_complex *>(in),
                                        reinterpret_cast<::fftwf_complex *>(out), FFTW_BACKWARD,
                                        FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
        }

        /**
         * Destructor.
         */
        virtual ~backward()
        {
            ::fftwf_destroy_plan(plan_);
            plan_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT { ::fftwf_execute(plan_); }

    private:
        /// @cond INTERNAL_FIELD
        ::fftwf_plan plan_;
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
        forward_real(int n, fft_real_t *in, fft_complex_t *out) : base(n, in, out, 1), plan_(0)
        {
            plan_ = ::fftwf_plan_dft_r2c_1d(n, reinterpret_cast<float *>(in), reinterpret_cast<::fftwf_complex *>(out),
                                            FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
        }

        /**
         * Destructor.
         */
        virtual ~forward_real()
        {
            ::fftwf_destroy_plan(plan_);
            plan_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT { ::fftwf_execute(plan_); }

    private:
        /// @cond INTERNAL_FIELD
        ::fftwf_plan plan_;
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
        backward_real(int n, fft_complex_t *in, fft_real_t *out) : base(n, in, out, n), plan_(0)
        {
            plan_ = ::fftwf_plan_dft_c2r_1d(n, reinterpret_cast<::fftwf_complex *>(in), reinterpret_cast<float *>(out),
                                            FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
        }

        /**
         * Destructor.
         */
        virtual ~backward_real()
        {
            ::fftwf_destroy_plan(plan_);
            plan_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT { ::fftwf_execute(plan_); }

    private:
        /// @cond INTERNAL_FIELD
        ::fftwf_plan plan_;
        /// @endcond
    };
};

} // namespace f

#endif // CXXDASP_USE_FFT_BACKEND_FFTWF

#if CXXDASP_USE_FFT_BACKEND_FFTW
namespace d {

/**
 * Double-precision FFT backend class
 * (using FFTW library)
 */
struct fftw {
    /**
     * Real value type
     */
    typedef fftd_real_t fft_real_t;

    /**
     * Complex value type
     */
    typedef fftd_complex_t fft_complex_t;

    /// @cond INTERNAL_FIELD
    fftw() = delete;
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
        forward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, 1), plan_(0)
        {
            plan_ =
                ::fftw_plan_dft_1d(n, reinterpret_cast<::fftw_complex *>(in), reinterpret_cast<::fftw_complex *>(out),
                                   FFTW_FORWARD, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
        }

        /**
         * Destructor.
         */
        virtual ~forward()
        {
            ::fftw_destroy_plan(plan_);
            plan_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT { ::fftw_execute(plan_); }

    private:
        /// @cond INTERNAL_FIELD
        ::fftw_plan plan_;
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
        backward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, n), plan_(0)
        {
            plan_ =
                ::fftw_plan_dft_1d(n, reinterpret_cast<::fftw_complex *>(in), reinterpret_cast<::fftw_complex *>(out),
                                   FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
        }

        /**
         * Destructor.
         */
        virtual ~backward()
        {
            ::fftw_destroy_plan(plan_);
            plan_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT { ::fftw_execute(plan_); }

    private:
        /// @cond INTERNAL_FIELD
        ::fftw_plan plan_;
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
        forward_real(int n, fft_real_t *in, fft_complex_t *out) : base(n, in, out, 1), plan_(0)
        {
            plan_ = ::fftw_plan_dft_r2c_1d(n, reinterpret_cast<double *>(in), reinterpret_cast<::fftw_complex *>(out),
                                           FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
        }

        /**
         * Destructor.
         */
        virtual ~forward_real()
        {
            ::fftw_destroy_plan(plan_);
            plan_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT { ::fftw_execute(plan_); }

    private:
        /// @cond INTERNAL_FIELD
        ::fftw_plan plan_;
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
        backward_real(int n, fft_complex_t *in, fft_real_t *out) : base(n, in, out, n), plan_(0)
        {
            plan_ = ::fftw_plan_dft_c2r_1d(n, reinterpret_cast<::fftw_complex *>(in), reinterpret_cast<double *>(out),
                                           FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
        }

        /**
         * Destructor.
         */
        virtual ~backward_real()
        {
            ::fftw_destroy_plan(plan_);
            plan_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT { ::fftw_execute(plan_); }

    private:
        /// @cond INTERNAL_FIELD
        ::fftw_plan plan_;
        /// @endcond
    };
};

} // namespace d

#endif // CXXDASP_USE_FFT_BACKEND_FFTW
} // namespace backend
} // namespace fft
} // namespace cxxdasp

#endif // CXXDASP_USE_FFT_BACKEND_FFTW || CXXDASP_USE_FFT_BACKEND_FFTWF
#endif // CXXDASP_FFT_FFT_BACKEND_FFTW_HPP_
