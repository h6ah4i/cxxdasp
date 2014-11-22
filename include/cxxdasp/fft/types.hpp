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

#ifndef CXXDASP_FFT_FFT_TYPES_HPP_
#define CXXDASP_FFT_FFT_TYPES_HPP_

#include <cxxporthelper/utility>
#include <cxxporthelper/complex>
#include <cxxporthelper/compiler.hpp>

#define FFT_MEMORY_ALIGNMENT 64

namespace cxxdasp {
namespace fft {

namespace f {
typedef float fft_real_t;
typedef std::complex<float> fft_complex_t;
}

namespace d {
typedef double fft_real_t;
typedef std::complex<double> fft_complex_t;
}

typedef f::fft_real_t fftf_real_t;
typedef f::fft_complex_t fftf_complex_t;

typedef d::fft_real_t fftd_real_t;
typedef d::fft_complex_t fftd_complex_t;

/**
 * Base class for FFT backend implementation.
 *
 * @tparam Tin  input type
 * @tparam Tout  output type
 */
template <typename Tin, typename Tout>
class fft_backend_base {

    /// @cond INTERNAL_FIELD
    fft_backend_base(const fft_backend_base &) = delete;
    fft_backend_base &operator=(const fft_backend_base &) = delete;
    /// @endcond

public:
    /**
     * Input data type
     */
    typedef Tin in_type;

    /**
     * Output data type
     */
    typedef Tout out_type;

    /**
     * Constructor.
     *
     * @param n [in] FFT size
     * @param in [in] Input buffer
     * @param out [in] Output buffer
     * @param scale [in] FFT output scale factor
     */
    fft_backend_base(int n, Tin *in, Tout *out, int scale) : n_(n), in_(in), out_(out), scale_(scale) {}

    /**
     * Destructor.
     */
    virtual ~fft_backend_base() {}

    /**
     * FFT size
     * @returns FFT size
     */
    int n() const CXXPH_NOEXCEPT { return n_; }

    /**
     * Scale factor of FFT applied data
     * @returns Scale of FFT applied datavalue
     * @note Normally this method returns 1 for forward FFT and "n" for backward FFT.
     */
    int scale() const CXXPH_NOEXCEPT { return scale_; }

    /**
     * Input buffer accessor.
     * @returns pointer to the input buffer
     * @note this buffer is aligned on FFT_MEMORY_ALIGNMENT size
     */
    /// @{
    const in_type *in() const CXXPH_NOEXCEPT { return in_; }

    in_type *in() CXXPH_NOEXCEPT { return in_; }
    /// @}

    /**
     * Output buffer accessor.
     * @returns pointer to the output buffer
     * @note this buffer is aligned on FFT_MEMORY_ALIGNMENT size
     */
    /// @{
    const out_type *out() const CXXPH_NOEXCEPT { return out_; }

    out_type *out() CXXPH_NOEXCEPT { return out_; }
    /// @}

protected:
    /// @cond INTERNAL_FIELD
    const int n_;
    in_type *const in_;
    out_type *const out_;
    const int scale_;
    /// @endcond
};

/**
 * FFT template class.
 *
 * @tparam Tin  input type
 * @tparam Tout  output type
 * @tparam Tbackend  backend class
 */
template <typename Tin, typename Tout, class Tbackend>
class fft {

    fft(const fft &) = delete;
    fft &operator=(const fft &) = delete;

public:
    /**
     * Input data type
     */
    typedef Tin in_type;

    /**
     * Output data type
     */
    typedef Tout out_type;

    /**
     * Backend class type
     */
    typedef Tbackend backend_class;

    /**
     * Constructor.
     */
    fft() CXXPH_NOEXCEPT : backend_(nullptr) {}

    /**
     * Constructor with initialization.
     * @param n [in] FFT size
     * @param in [in] Input buffer
     * @param out [in] Output buffer
     */
    fft(int n, in_type *in, out_type *out) : backend_(nullptr) { setup(n, in, out); }

    /**
     * Destructor.
     */
    ~fft()
    {
        delete backend_;
        backend_ = nullptr;
    }

    /**
     * Setup
     * @param n [in] FFT size
     * @param in [in] Input buffer
     * @param out [in] Output buffer
     */
    void setup(int n, in_type *in, out_type *out)
    {
        delete backend_;
        backend_ = nullptr;

        backend_ = new backend_class(n, in, out);
    }

    /**
     * FFT size
     * @returns FFT size
     */
    int n() const CXXPH_NOEXCEPT { return backend_->n(); }

    /**
     * Scale of FFT applied datavalue
     * @returns Scale of FFT applied datavalue
     * @note Normally this method returns 1 for forward FFT and "n" for backward FFT.
     */
    int scale() const CXXPH_NOEXCEPT { return backend_->scale(); }

    /**
     * Input buffer accessor.
     * @returns pointer to the input buffer
     * @note this buffer is aligned on FFT_MEMORY_ALIGNMENT size
     */
    /// @{
    const in_type *in() const CXXPH_NOEXCEPT { return backend_->in(); }

    in_type *in() CXXPH_NOEXCEPT { return backend_->in(); }
    /// @}

    /**
     * Output buffer accessor.
     * @returns pointer to the output buffer
     * @note this buffer is aligned on FFT_MEMORY_ALIGNMENT size
     */
    /// @{
    const out_type *out() const CXXPH_NOEXCEPT { return backend_->out(); }

    out_type *out() CXXPH_NOEXCEPT { return backend_->out(); }
    /// @}

    /**
     * Execute FFT.
     */
    void execute() CXXPH_NOEXCEPT { backend_->execute(); }

    /**
     * Move operation.
     */
    /// @{
    fft &operator=(fft &&other) CXXPH_NOEXCEPT
    {
        if (this == &other) {
            return (*this);
        }

        move(std::forward<fft &&>(other));

        return (*this);
    }
    /// @}

private:
    /// @cond INTERNAL_FIELD
    void move(fft &&other) CXXPH_NOEXCEPT
    {
        delete backend_;

        backend_ = other.backend_;
        other.backend_ = nullptr;
    }
    /// @endcond

private:
    /// @cond INTERNAL_FIELD
    backend_class *backend_;
    /// @endcond
};

} // namespace fft
} // namespace cxxdasp

#endif // CXXDASP_FFT_FFT_TYPES_HPP_
