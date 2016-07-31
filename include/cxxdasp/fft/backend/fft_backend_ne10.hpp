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

#ifndef CXXDASP_FFT_FFT_BACKEND_NE10_HPP_
#define CXXDASP_FFT_FFT_BACKEND_NE10_HPP_

#include <cxxdasp/cxxdasp_config.hpp>

#if CXXDASP_USE_FFT_BACKEND_NE10

#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/platform_info.hpp>

#include <cxxdasp/fft/types.hpp>

#include <NE10.h>

namespace cxxdasp {
namespace fft {
namespace backend {

namespace f {

/**
 * Single-precision FFT Backend class
 * (using Ne10:"An open optimized software library project for the ARM® Architecture")
 */
struct ne10 {
    /**
     * Real value type
     */
    typedef fftf_real_t fft_real_t;

    /**
     * Complex value type
     */
    typedef fftf_complex_t fft_complex_t;

    /// @cond INTERNAL_FIELD
    ne10() = delete;
    /// @endcond

private:
    /// @cond INTERNAL_FIELD
    typedef void (*fp_ne10_fft_c2c_1d_float32)(ne10_fft_cpx_float32_t *fout, ne10_fft_cpx_float32_t *fin,
                                               ne10_fft_cfg_float32_t cfg, ne10_int32_t inverse_fft);

    typedef void (*fp_ne10_fft_r2c_1d_float32)(ne10_fft_cpx_float32_t *fout, ne10_float32_t *fin,
                                               ne10_fft_r2c_cfg_float32_t cfg);

    typedef void (*fp_ne10_fft_c2r_1d_float32)(ne10_float32_t *fout, ne10_fft_cpx_float32_t *fin,
                                               ne10_fft_r2c_cfg_float32_t cfg);
    /// @endcond

public:
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
            if (cxxporthelper::platform_info::support_arm_neon()) {
                cfg_ = ::ne10_fft_alloc_c2c_float32_neon(n);
                fp_execute_ = ne10_fft_c2c_1d_float32_neon;
            } else {
                cfg_ = ::ne10_fft_alloc_c2c_float32_c(n);
                fp_execute_ = ne10_fft_c2c_1d_float32_c;
            }
        }

        /**
         * Destructor.
         */
        virtual ~forward()
        {
            NE10_FREE(cfg_);
            cfg_ = nullptr;
            fp_execute_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            (*fp_execute_)(reinterpret_cast<ne10_fft_cpx_float32_t *>(out_),
                           reinterpret_cast<ne10_fft_cpx_float32_t *>(in_), cfg_, 0);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::ne10_fft_cfg_float32_t cfg_;
        fp_ne10_fft_c2c_1d_float32 fp_execute_;
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
        inverse(int n, fft_complex_t *in, fft_complex_t *out)
            : base(n, in, out, 1), cfg_(nullptr), fp_execute_(nullptr)
        {
            if (cxxporthelper::platform_info::support_arm_neon()) {
                cfg_ = ::ne10_fft_alloc_c2c_float32_neon(n);
                fp_execute_ = ne10_fft_c2c_1d_float32_neon;
            } else {
                cfg_ = ::ne10_fft_alloc_c2c_float32_c(n);
                fp_execute_ = ne10_fft_c2c_1d_float32_c;
            }
        }

        /**
         * Destructor.
         */
        virtual ~inverse()
        {
            NE10_FREE(cfg_);
            cfg_ = nullptr;
            fp_execute_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            (*fp_execute_)(reinterpret_cast<ne10_fft_cpx_float32_t *>(out_),
                           reinterpret_cast<ne10_fft_cpx_float32_t *>(in_), cfg_, 1);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::ne10_fft_cfg_float32_t cfg_;
        fp_ne10_fft_c2c_1d_float32 fp_execute_;
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
        forward_real(int n, fft_real_t *in, fft_complex_t *out) : base(n, in, out, 1), cfg_(nullptr)
        {
            cfg_ = ::ne10_fft_alloc_r2c_float32(n);

            if (cxxporthelper::platform_info::support_arm_neon()) {
                fp_execute_ = ne10_fft_r2c_1d_float32_neon;
            } else {
                fp_execute_ = ne10_fft_r2c_1d_float32_c;
            }
        }

        /**
         * Destructor.
         */
        virtual ~forward_real()
        {
            NE10_FREE(cfg_);
            cfg_ = nullptr;
            fp_execute_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            (*fp_execute_)(reinterpret_cast<ne10_fft_cpx_float32_t *>(&out_[0]),
                           reinterpret_cast<ne10_float32_t *>(&in_[0]), cfg_);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::ne10_fft_r2c_cfg_float32_t cfg_;
        fp_ne10_fft_r2c_1d_float32 fp_execute_;
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
            : base(n, in, out, 1), cfg_(nullptr), fp_execute_(nullptr)
        {
            cfg_ = ::ne10_fft_alloc_r2c_float32(n);

            if (cxxporthelper::platform_info::support_arm_neon()) {
                fp_execute_ = ne10_fft_c2r_1d_float32_neon;
            } else {
                fp_execute_ = ne10_fft_c2r_1d_float32_c;
            }
        }

        /**
         * Destructor.
         */
        virtual ~inverse_real()
        {
            NE10_FREE(cfg_);
            cfg_ = nullptr;
            fp_execute_ = nullptr;
        }

        /**
         * Execute FFT.
         */
        void execute() CXXPH_NOEXCEPT
        {
            const fft_complex_t::value_type orig_in_0_imag = in_[0].imag();

            (*fp_execute_)(reinterpret_cast<ne10_float32_t *>(&out_[0]),
                           reinterpret_cast<ne10_fft_cpx_float32_t *>(&in_[0]), cfg_);
        }

    private:
        /// @cond INTERNAL_FIELD
        ::ne10_fft_r2c_cfg_float32_t cfg_;
        fp_ne10_fft_c2r_1d_float32 fp_execute_;
        /// @endcond
    };
};

} // namespace f

} // namespace backend
} // namespace fft
} // namespace cxxdasp

#endif // CXXDASP_USE_FFT_BACKEND_NE10
#endif // CXXDASP_FFT_FFT_BACKEND_NE10_HPP_
