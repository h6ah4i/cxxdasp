//
//    Copyright (C) 2016 Haruki Hasegawa
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

#ifndef CXXDASP_FFT_FFT_BACKEND_MUFFT_HPP_
#define CXXDASP_FFT_FFT_BACKEND_MUFFT_HPP_

#include <cxxdasp/cxxdasp_config.hpp>

#if CXXDASP_USE_FFT_BACKEND_MUFFT

#include <cxxdasp/fft/types.hpp>

#include <mufft/fft.h>

namespace cxxdasp {
namespace fft {
namespace backend {

namespace f {
/**
 * Single-precision FFT backend class
 * (using muFFT library)
 */
struct mufft {
    /**
     * Real value type
     */
    typedef fftf_real_t fft_real_t;

    /**
     * Complex value type
     */
    typedef fftf_complex_t fft_complex_t;

    /// @cond INTERNAL_FIELD
    mufft() = delete;
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
        forward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, 1), muplan_(nullptr)
        {
            muplan_ = ::mufft_create_plan_1d_c2c(n, MUFFT_FORWARD, MUFFT_FLAG_CPU_ANY);
        }

        /**
         * Destructor.
         */
        virtual ~forward()
        {
            ::mufft_free_plan_1d(muplan_);
            muplan_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::mufft_execute_plan_1d(muplan_, &out_[0], &in_[0]);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::mufft_plan_1d *muplan_;
        /// @endcond
    };

    /**
     * Inverse (Complex) FFT
     */
    class inverse : public fft_backend_base<fft_complex_t, fft_complex_t> {
        typedef fft_backend_base<fft_complex_t, fft_complex_t> base;

    public:
        /**
         * Constructor.
         *
         * @param n [in] FFT size
         * @param in [in] Input buffer
         * @param out [in] Output buffer
         */
        inverse(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, n), muplan_(nullptr)
        {
            muplan_ = ::mufft_create_plan_1d_c2c(n, MUFFT_INVERSE, MUFFT_FLAG_CPU_ANY);
        }

        /**
         * Destructor.
         */
        virtual ~inverse()
        {
            ::mufft_free_plan_1d(muplan_);
            muplan_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::mufft_execute_plan_1d(muplan_, &out_[0], &in_[0]);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::mufft_plan_1d *muplan_;
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
        forward_real(int n, fft_real_t *in, fft_complex_t *out) : base(n, in, out, 1), muplan_(nullptr)
        {
            muplan_ = ::mufft_create_plan_1d_r2c(n, MUFFT_FLAG_CPU_ANY);
        }

        /**
         * Destructor.
         */
        virtual ~forward_real()
        {
            ::mufft_free_plan_1d(muplan_);
            muplan_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::mufft_execute_plan_1d(muplan_, &out_[0], &in_[0]);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::mufft_plan_1d *muplan_;
        /// @endcond
    };

    /**
     * Inverse (Real) FFT
     */
    class inverse_real : public fft_backend_base<fft_complex_t, fft_real_t> {
        typedef fft_backend_base<fft_complex_t, fft_real_t> base;

    public:
        /**
         * Constructor.
         *
         * @param n [in] FFT size
         * @param in [in] Input buffer
         * @param out [in] Output buffer
         */
        inverse_real(int n, fft_complex_t *in, fft_real_t *out) : base(n, in, out, n), muplan_(nullptr)
        {
            muplan_ = ::mufft_create_plan_1d_c2r(n, MUFFT_FLAG_CPU_ANY);
        }

        /**
         * Destructor.
         */
        virtual ~inverse_real()
        {
            ::mufft_free_plan_1d(muplan_);
            muplan_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::mufft_execute_plan_1d(muplan_, &out_[0], &in_[0]);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::mufft_plan_1d *muplan_;
        /// @endcond
    };
};

} // namespace f

} // namespace backend
} // namespace fft
} // namespace cxxdasp

#endif // CXXDASP_USE_FFT_BACKEND_MUFFT
#endif // CXXDASP_FFT_FFT_BACKEND_MUFFT_HPP_
