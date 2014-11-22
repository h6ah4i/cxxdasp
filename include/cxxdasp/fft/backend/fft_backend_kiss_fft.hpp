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

#ifndef CXXDASP_FFT_FFT_BACKEND_KISS_FFT_HPP_
#define CXXDASP_FFT_FFT_BACKEND_KISS_FFT_HPP_

#include <cxxdasp/cxxdasp_config.hpp>

#if CXXDASP_USE_FFT_BACKEND_KISS_FFT

#include <cxxdasp/fft/types.hpp>

#include <kiss_fft.h>
#include <tools/kiss_fftr.h>

namespace cxxdasp {
namespace fft {
namespace backend {

namespace f {

/**
 * Single-precision FFT backend class
 * (using Kiss FFT:"Keep It Simple, Stupid" FFT library)
 */
struct kiss_fft {
    /**
     * Real value type
     */
    typedef fftf_real_t fft_real_t;

    /**
     * Complex value type
     */
    typedef fftf_complex_t fft_complex_t;

    /// @cond INTERNAL_FIELD
    kiss_fft() = delete;
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
        forward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, 1), cfg_(0)
        {
            cfg_ = ::kiss_fft_alloc(n, 0, nullptr, nullptr);
        }

        /**
         * Destructor.
         */
        virtual ~forward()
        {
            ::kiss_fft_free(cfg_);
            cfg_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::kiss_fft(cfg_, reinterpret_cast<const kiss_fft_cpx *>(in_), reinterpret_cast<kiss_fft_cpx *>(out_));
        }

    private:
        /// @cond INTERNAL_FIELD
        ::kiss_fft_cfg cfg_;
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
        backward(int n, fft_complex_t *in, fft_complex_t *out) : base(n, in, out, n), cfg_(0)
        {
            cfg_ = ::kiss_fft_alloc(n, 1, nullptr, nullptr);
        }

        /**
         * Destructor.
         */
        virtual ~backward()
        {
            ::kiss_fft_free(cfg_);
            cfg_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::kiss_fft(cfg_, reinterpret_cast<const kiss_fft_cpx *>(in_), reinterpret_cast<kiss_fft_cpx *>(out_));
        }

    private:
        /// @cond INTERNAL_FIELD
        ::kiss_fft_cfg cfg_;
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
        forward_real(int n, fft_real_t *in, fft_complex_t *out) : base(n, in, out, 1), cfg_(0)
        {
            cfg_ = ::kiss_fftr_alloc(n, 0, nullptr, nullptr);
        }

        /**
         * Destructor.
         */
        virtual ~forward_real()
        {
            ::kiss_fftr_free(cfg_);
            cfg_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::kiss_fftr(cfg_, reinterpret_cast<const kiss_fft_scalar *>(in_), reinterpret_cast<kiss_fft_cpx *>(out_));
        }

    private:
        /// @cond INTERNAL_FIELD
        ::kiss_fftr_cfg cfg_;
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
        backward_real(int n, fft_complex_t *in, fft_real_t *out) : base(n, in, out, n), cfg_(0)
        {
            cfg_ = ::kiss_fftr_alloc(n, 1, nullptr, nullptr);
        }

        /**
         * Destructor.
         */
        virtual ~backward_real()
        {
            ::kiss_fftr_free(cfg_);
            cfg_ = 0;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            ::kiss_fftri(cfg_, reinterpret_cast<const kiss_fft_cpx *>(in_), reinterpret_cast<kiss_fft_scalar *>(out_));
        }

    private:
        /// @cond INTERNAL_FIELD
        ::kiss_fftr_cfg cfg_;
        /// @endcond
    };
};

} // namespace f

} // namespace backend
} // namespace fft
} // namespace cxxdasp

#endif // CXXDASP_USE_FFT_BACKEND_KISS_FFT
#endif // CXXDASP_FFT_FFT_BACKEND_KISS_FFT_HPP_
