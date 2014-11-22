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

#ifndef CXXDASP_FFT_FFT_BACKEND_PFFFT_HPP_
#define CXXDASP_FFT_FFT_BACKEND_PFFFT_HPP_

#include <cxxdasp/cxxdasp_config.hpp>

#if CXXDASP_USE_FFT_BACKEND_PFFFT

#include <cxxporthelper/utility>
#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/fft/types.hpp>

#include <pffft.h>

namespace cxxdasp {
namespace fft {
namespace backend {

namespace f {

/**
 * Single-precision FFT backend cla*
 * (using PFFFT:"a pretty fast FFT" library)
 */
struct pffft {
    /**
     * Real value type
     */
    typedef fftf_real_t fft_real_t;

    /**
     * Complex value type
     */
    typedef fftf_complex_t fft_complex_t;

    /// @cond INTERNAL_FIELD
    pffft() = delete;
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
        forward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, 1), setup_(0), work_()
        {
            cxxporthelper::aligned_memory<fft_complex_t> work(n);
            setup_ = ::pffft_new_setup(n, PFFFT_COMPLEX);
            work_ = std::move(work);
        }

        /**
         * Destructor.
         */
        virtual ~forward()
        {
            ::pffft_destroy_setup(setup_);
            setup_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::pffft_transform_ordered(setup_, reinterpret_cast<const float *>(in_), reinterpret_cast<float *>(out_),
                                      reinterpret_cast<float *>(&work_[0]), PFFFT_FORWARD);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::PFFFT_Setup *setup_;
        cxxporthelper::aligned_memory<fft_complex_t> work_;
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
        backward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, n), setup_(0), work_()
        {
            cxxporthelper::aligned_memory<fft_complex_t> work(n);
            setup_ = ::pffft_new_setup(n, PFFFT_COMPLEX);
            work_ = std::move(work);
        }

        /**
         * Destructor.
         */
        virtual ~backward()
        {
            ::pffft_destroy_setup(setup_);
            setup_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::pffft_transform_ordered(setup_, reinterpret_cast<const float *>(in_), reinterpret_cast<float *>(out_),
                                      reinterpret_cast<float *>(&work_[0]), PFFFT_BACKWARD);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::PFFFT_Setup *setup_;
        cxxporthelper::aligned_memory<fft_complex_t> work_;
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
        forward_real(int n, fft_real_t *in, fft_complex_t *out) : base(n, in, out, 1), setup_(0), work_()
        {
            cxxporthelper::aligned_memory<fft_real_t> work(n);
            setup_ = ::pffft_new_setup(n, PFFFT_REAL);
            work_ = std::move(work);
        }

        /**
         * Destructor.
         */
        virtual ~forward_real()
        {
            ::pffft_destroy_setup(setup_);
            setup_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            namespace compat = cxxporthelper::complex;

            ::pffft_transform_ordered(setup_, reinterpret_cast<const float *>(in_), reinterpret_cast<float *>(out_),
                                      reinterpret_cast<float *>(&work_[0]), PFFFT_FORWARD);

            // correct 0 th and (N/2-1) th element
            compat::set_real(out_[n_ / 2], out_[0].imag());
            compat::set_imag(out_[n_ / 2], 0.0f);
            compat::set_imag(out_[0], 0.0f);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::PFFFT_Setup *setup_;
        cxxporthelper::aligned_memory<fft_real_t> work_;
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
        backward_real(int n, fft_complex_t *in, fft_real_t *out) : base(n, in, out, n), setup_(0), work_()
        {
            cxxporthelper::aligned_memory<fft_real_t> work(n);
            setup_ = ::pffft_new_setup(n, PFFFT_REAL);
            work_ = std::move(work);
        }

        /**
         * Destructor.
         */
        virtual ~backward_real()
        {
            ::pffft_destroy_setup(setup_);
            setup_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            namespace compat = cxxporthelper::complex;

            const fft_complex_t::value_type orig_in_0_imag = in_[0].imag();

            // pack to 0 th element
            compat::set_imag(in_[0], in_[n_ / 2].real());

            ::pffft_transform_ordered(setup_, reinterpret_cast<const float *>(in_), reinterpret_cast<float *>(out_),
                                      reinterpret_cast<float *>(&work_[0]), PFFFT_BACKWARD);

            // restore 0 th element
            compat::set_imag(in_[0], orig_in_0_imag);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::PFFFT_Setup *setup_;
        cxxporthelper::aligned_memory<fft_real_t> work_;
        /// @endcond
    };
};

} // namespace f

} // namespace backend
} // namespace fft
} // namespace cxxdasp

#endif // CXXDASP_USE_FFT_BACKEND_PFFFT
#endif // CXXDASP_FFT_FFT_BACKEND_PFFFT_HPP_
