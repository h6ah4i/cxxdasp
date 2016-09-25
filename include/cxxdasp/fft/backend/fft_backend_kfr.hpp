//
//    Copyright (C) 2015 Haruki Hasegawa
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

#ifndef CXXDASP_FFT_FFT_BACKEND_KFR_HPP_
#define CXXDASP_FFT_FFT_BACKEND_KFR_HPP_

#include <cxxdasp/cxxdasp_config.hpp>

#if CXXDASP_USE_FFT_BACKEND_KFR_D || CXXDASP_USE_FFT_BACKEND_KFR_F

#include <cxxdasp/fft/types.hpp>


#include <kfr/base.hpp>
#include <kfr/dft.hpp>

namespace cxxdasp {
namespace fft {
namespace backend {

#if CXXDASP_USE_FFT_BACKEND_KFR_F
namespace f {

/**
 * Single-precision FFT backend class
 * (using KFR library)
 */
struct kfr {
    /**
     * Real value type
     */
    typedef fftf_real_t fft_real_t;

    /**
     * Complex value type
     */
    typedef fftf_complex_t fft_complex_t;

    /// @cond INTERNAL_FIELD
    kfr() = delete;
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
        forward(int n, fft_complex_t *in, fft_complex_t *out)
         : base(n, in, out, 1), plan_(n), temp_(plan_.temp_size)
        {
        }

        /**
         * Destructor.
         */
        virtual ~forward() {}

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT {
            plan_.execute(
                reinterpret_cast<::kfr::complex<fft_real_t> *>(out()), 
                reinterpret_cast<::kfr::complex<fft_real_t> *>(in()),
                temp_.data(), false);
        }

    private:
        ::kfr::dft_plan<fft_real_t> plan_;
        ::kfr::univector<::kfr::u8> temp_;
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
        inverse(int n, fft_complex_t *in, fft_complex_t *out)
         : base(n, in, out, n), plan_(n), temp_(plan_.temp_size)
        {
        }

        /**
         * Destructor.
         */
        virtual ~inverse() {}

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT {
            plan_.execute(
                reinterpret_cast<::kfr::complex<fft_real_t> *>(out()), 
                reinterpret_cast<::kfr::complex<fft_real_t> *>(in()),
                temp_.data(), true);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::kfr::dft_plan<fft_real_t> plan_;
        ::kfr::univector<::kfr::u8> temp_;
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
        forward_real(int n, fft_real_t *in, fft_complex_t *out)
         : base(n, in, out, 1), plan_(n), temp_(plan_.temp_size)
        {
        }

        /**
         * Destructor.
         */
        virtual ~forward_real() {}

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT {
            plan_.execute(
                reinterpret_cast<::kfr::complex<fft_real_t> *>(out()), 
                reinterpret_cast<fft_real_t *>(in()),
                temp_.data(), ::kfr::dft_pack_format::CCs);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::kfr::dft_plan_real<fft_real_t> plan_;
        ::kfr::univector<::kfr::u8> temp_;
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
        inverse_real(int n, fft_complex_t *in, fft_real_t *out)
         : base(n, in, out, n), plan_(n), temp_(plan_.temp_size)
        {
        }

        /**
         * Destructor.
         */
        virtual ~inverse_real() {}

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT {
            plan_.execute(
                reinterpret_cast<fft_real_t *>(out()), 
                reinterpret_cast<::kfr::complex<fft_real_t> *>(in()),
                temp_.data(), ::kfr::dft_pack_format::CCs);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::kfr::dft_plan_real<fft_real_t> plan_;
        ::kfr::univector<::kfr::u8> temp_;
        /// @endcond
    };
};

} // namespace f

#endif // CXXDASP_USE_FFT_BACKEND_KFR_F

#if CXXDASP_USE_FFT_BACKEND_KFR_D
namespace d {

/**
 * Double-precision FFT backend class
 * (using KFR library)
 */
struct kfr {
    /**
     * Real value type
     */
    typedef fftd_real_t fft_real_t;

    /**
     * Complex value type
     */
    typedef fftd_complex_t fft_complex_t;

    /// @cond INTERNAL_FIELD
    kfr() = delete;
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
        forward(int n, fft_complex_t *in, fft_complex_t *out)
         : base(n, in, out, 1), plan_(n), temp_(plan_.temp_size)
        {
        }

        /**
         * Destructor.
         */
        virtual ~forward() {}

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT {
            plan_.execute(
                reinterpret_cast<::kfr::complex<fft_real_t> *>(out()), 
                reinterpret_cast<::kfr::complex<fft_real_t> *>(in()),
                temp_.data(), false);
        }

    private:
        ::kfr::dft_plan<fft_real_t> plan_;
        ::kfr::univector<::kfr::u8> temp_;
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
        inverse(int n, fft_complex_t *in, fft_complex_t *out)
         : base(n, in, out, n), plan_(n), temp_(plan_.temp_size)
        {
        }

        /**
         * Destructor.
         */
        virtual ~inverse() {}

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT {
            plan_.execute(
                reinterpret_cast<::kfr::complex<fft_real_t> *>(out()), 
                reinterpret_cast<::kfr::complex<fft_real_t> *>(in()),
                temp_.data(), true);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::kfr::dft_plan<fft_real_t> plan_;
        ::kfr::univector<::kfr::u8> temp_;
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
        forward_real(int n, fft_real_t *in, fft_complex_t *out)
         : base(n, in, out, 1), plan_(n), temp_(plan_.temp_size)
        {
        }

        /**
         * Destructor.
         */
        virtual ~forward_real() {}

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT {
            plan_.execute(
                reinterpret_cast<::kfr::complex<fft_real_t> *>(out()), 
                reinterpret_cast<fft_real_t *>(in()),
                temp_.data(), ::kfr::dft_pack_format::CCs);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::kfr::dft_plan_real<fft_real_t> plan_;
        ::kfr::univector<::kfr::u8> temp_;
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
        inverse_real(int n, fft_complex_t *in, fft_real_t *out)
         : base(n, in, out, n), plan_(n), temp_(plan_.temp_size)
        {
        }

        /**
         * Destructor.
         */
        virtual ~inverse_real() {}

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT {
            plan_.execute(
                reinterpret_cast<fft_real_t *>(out()), 
                reinterpret_cast<::kfr::complex<fft_real_t>*>(in()),
                temp_.data(), ::kfr::dft_pack_format::CCs);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::kfr::dft_plan_real<fft_real_t> plan_;
        ::kfr::univector<::kfr::u8> temp_;
        /// @endcond
    };
};


} // namespace d

#endif // CXXDASP_USE_FFT_BACKEND_KFR_D
} // namespace backend
} // namespace fft
} // namespace cxxdasp

#endif // CXXDASP_USE_FFT_BACKEND_KFR_D || CXXDASP_USE_FFT_BACKEND_KFR_F
#endif // CXXDASP_FFT_FFT_BACKEND_KFR_HPP_
