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

#ifndef CXXDASP_FFT_FFT_BACKEND_CKFFT_HPP_
#define CXXDASP_FFT_FFT_BACKEND_CKFFT_HPP_

#include <cxxdasp/cxxdasp_config.hpp>

#if CXXDASP_USE_FFT_BACKEND_CKFFT

#include <cxxporthelper/utility>
#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/fft/types.hpp>

#include <ckfft/ckfft.h>

namespace cxxdasp {
namespace fft {
namespace backend {

namespace f {
/**
 * Single-precision FFT backend class
 * (using Cricket FFT library)
 */
struct ckfft {
    /**
     * Real value type
     */
    typedef fftf_real_t fft_real_t;

    /**
     * Complex value type
     */
    typedef fftf_complex_t fft_complex_t;

    /// @cond INTERNAL_FIELD
    ckfft() = delete;
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
        forward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, 1), context_(nullptr)
        {
            context_ = ::CkFftInit(n, kCkFftDirection_Forward, nullptr, nullptr);
        }

        /**
         * Destructor.
         */
        virtual ~forward()
        {
            ::CkFftShutdown(context_);
            context_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::CkFftComplexForward(context_, n_, reinterpret_cast<const CkFftComplex*>(in_), reinterpret_cast<CkFftComplex *>(out_));
        }

    private:
        /// @cond INTERNAL_FIELD
        ::CkFftContext *context_;
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
        inverse(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, n), context_(nullptr)
        {
            context_ = ::CkFftInit(n, kCkFftDirection_Inverse, nullptr, nullptr);
        }

        /**
         * Destructor.
         */
        virtual ~inverse()
        {
            ::CkFftShutdown(context_);
            context_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::CkFftComplexInverse(context_, n_, reinterpret_cast<const CkFftComplex*>(in_), reinterpret_cast<CkFftComplex *>(out_));
        }

    private:
        /// @cond INTERNAL_FIELD
        ::CkFftContext *context_;
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
        forward_real(int n, fft_real_t *in, fft_complex_t *out) : base(n, in, out, 2), context_(nullptr)
        {
            context_ = ::CkFftInit(n, kCkFftDirection_Forward, nullptr, nullptr);
        }

        /**
         * Destructor.
         */
        virtual ~forward_real()
        {
            ::CkFftShutdown(context_);
            context_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::CkFftRealForward(context_, n_, reinterpret_cast<const float *>(in_), reinterpret_cast<CkFftComplex *>(out_));
        }

    private:
        /// @cond INTERNAL_FIELD
        ::CkFftContext *context_;
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
        inverse_real(int n, fft_complex_t *in, fft_real_t *out) : base(n, in, out, n), context_(nullptr), tmp_()
        {
            cxxporthelper::aligned_memory<CkFftComplex> tmp(n / 2 + 1);
            context_ = ::CkFftInit(n, kCkFftDirection_Inverse, nullptr, nullptr);
            tmp_ = std::move(tmp);
        }

        /**
         * Destructor.
         */
        virtual ~inverse_real()
        {
            ::CkFftShutdown(context_);
            context_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::CkFftRealInverse(context_, n_, reinterpret_cast<const CkFftComplex *>(in_), reinterpret_cast<float *>(out_), &tmp_[0]);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::CkFftContext *context_;
        cxxporthelper::aligned_memory<CkFftComplex> tmp_;
        /// @endcond
    };
};

} // namespace f

} // namespace backend
} // namespace fft
} // namespace cxxdasp

#endif // CXXDASP_USE_FFT_BACKEND_CKFFT
#endif // CXXDASP_FFT_FFT_BACKEND_CKFFT_HPP_
