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

#ifndef CXXDASP_RESAMPLER_FFT_SINGLE_CHANNEL_FFT_X2_RESAMPLER_HPP_
#define CXXDASP_RESAMPLER_FFT_SINGLE_CHANNEL_FFT_X2_RESAMPLER_HPP_

#include <algorithm>
#include <cstring>
#include <cassert>

#include <cxxporthelper/type_traits>
#include <cxxporthelper/utility>
#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/fft/fft.hpp>
#include <cxxdasp/utils/utils.hpp>

namespace cxxdasp {
namespace resampler {

/*
 * Overview:
 *   Fast x2 upsampling specialized resampler class.
 *   This class using FFT/IFFT for x2 over sampling and combolution.
 *
 * Signal flow:
 *
 *   {Filter[N]} ---- <FFT> --------------------\
 *                                          <Multiply> -- <IFFT> -- {x2 Signal[N]}
 *   {Signal[N/2]} -- <FFT> -- <Mirror (*)> ----/
 *
 *   (*) Zero-insertion x2 over sampling in frequency domain.
 *
 *
 * FYI; Equivalent signal flow processed in time domain:
 *
 *   {Filter[N]} -------------------------------------------\
 *                                                      <Convolve> -- {x2 Signal[N]}
 *   {Signal[N/2]} -- <x2 zero-insertion over sampling> ----/
 *
 *   (*) Zero-insertion x2 over sampling in frequency domain.
 *
 *
 * Buffer usage (input buffer):
 *
 *   [###########|%%%%%%%%%%%%%%$$$$$$$$$$$]
 *   |<-- M/2 -->|<--------- L/2 --------->|
 *   |<--------------- N/2 --------------->|
 *
 *   ###      : copy of the previous block tail  (overlapped area)
 *   %%%, $$$ : new data area
 *   $$$      : this area will be copied to the ### area after FFT processed
 *              (overlapped area)
 *
 * References:
 *
 *   - Overlap-save method
 *     http://en.wikipedia.org/wiki/Overlap%E2%80%93save_method
 */

//
// forward declarations
//
template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
class single_channel_fft_x2_resampler;

/**
 * x2 oversampler using FFT based fast-convolution technique
 *
 * @tparam TSrc source data type
 * @tparam TDest destination data type
 * @tparam TCoeffs FIR coefficients type
 * @tparam TFFTBackend FFT backend class
 */
template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
class single_channel_fft_x2_resampler_shared_context {
    friend class single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>;

    /// @cond INTERNAL_FIELD
    single_channel_fft_x2_resampler_shared_context(const single_channel_fft_x2_resampler_shared_context &) = delete;
    single_channel_fft_x2_resampler_shared_context &
    operator=(const single_channel_fft_x2_resampler_shared_context &) = delete;
    /// @endcond

public:
    /**
     * Source data type.
     */
    typedef TSrc source_data_t;

    /**
     * Destination data type.
     */
    typedef TDest dest_data_t;

    /**
     * FIR coefficients type.
     */
    typedef TCoeffs coeffs_t;

    /**
     * FFT backend class.
     */
    typedef TFFTBackend fft_backend_type;

    /**
     * FFT real value type.
     */
    typedef typename TFFTBackend::fft_real_t fft_real_t;

    /**
     * FFT complex value type.
     */
    typedef typename TFFTBackend::fft_complex_t fft_complex_t;

    /**
     * Self type.
     */
    typedef single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend> resampler_class;

public:
    /**
     * Constructor.
     *
     * @param [in] filter_kernel FIR coefficients
     * @param [in] filter_length number of FIR coefficients
     * @param [in] process_block_size_adjust specify processing block size (0..)
     *
     * @note Processing block size is calculated by the following equation:
     *         {processing block size} = {filter_length * ((2 << process_block_size_adjust) - 1)}
     *       Large block size improves processing efficiency, however memory consumption and latency are increased.
     */
    single_channel_fft_x2_resampler_shared_context(const coeffs_t *filter_kernel, int filter_length,
                                                   int process_block_size_adjust);

    /**
     * Destructor.
     */
    ~single_channel_fft_x2_resampler_shared_context();

    /// @cond INTERNAL_FIELD

    // this method is referred from multi_channel_single_channel_fft_x2_resampler class
    int N() const CXXPH_NOEXCEPT { return n_; }
    /// @endcond

private:
    static_assert(std::is_floating_point<source_data_t>::value, "source_data_t requires floating point type");
    static_assert(std::is_floating_point<dest_data_t>::value, "dest_data_t requires floating point type");

    /// @cond INTERNAL_FIELD
    typedef fft::fft<fft_real_t, fft_complex_t, typename TFFTBackend::forward_real> fft_forward_real;
    typedef fft::fft<fft_complex_t, fft_real_t, typename TFFTBackend::backward_real> fft_backward_real;

    int M() const CXXPH_NOEXCEPT { return m_; }

    int N2() const CXXPH_NOEXCEPT { return n2_; }

    int L() const CXXPH_NOEXCEPT { return l_; }

    int delay() const CXXPH_NOEXCEPT { return delay_; }
    /// @endcond

private:
    /// @cond INTERNAL_FIELD
    int filter_length_;
    int process_block_size_adjust_;
    cxxporthelper::aligned_memory<fft_complex_t> mem_f_filter_kernel_; // Frequency response of the filter

    int m_;
    int n_;
    int n2_;
    int l_;
    int delay_;
    /// @endcond
};

/**
 * FFT based x2 upsampling specialized resampler class.
 *
 * @tparam TSrc source data type
 * @tparam TDest destination data type
 * @tparam TCoeffs FIR coefficient data type
 * @tparam TFFTBackend FFT implementation class type
 */
template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
class single_channel_fft_x2_resampler {

    /// @cond INTERNAL_FIELD
    single_channel_fft_x2_resampler(const single_channel_fft_x2_resampler &) = delete;
    single_channel_fft_x2_resampler &operator=(const single_channel_fft_x2_resampler &) = delete;
    /// @endcond

public:
    /**
     * Shared context type.
     */
    typedef single_channel_fft_x2_resampler_shared_context<TSrc, TDest, TCoeffs, TFFTBackend> shared_context;

    /**
     * Source audio frame type.
     */
    typedef typename shared_context::source_data_t source_data_t;

    /**
     * Destination audio frame type.
     */
    typedef typename shared_context::dest_data_t dest_data_t;

    /**
     * FIR coefficients type.
     */
    typedef typename shared_context::coeffs_t coeffs_t;

    /**
     * FFT real value type.
     */
    typedef typename shared_context::fft_real_t fft_real_t;

    /**
     * FFT complex value type.
     */
    typedef typename shared_context::fft_complex_t fft_complex_t;

    /**
     * Constructor (with normal FIR coefficients array).
     *
     * @param [in] filter_kernel FIR coefficients
     * @param [in] filter_length number of FIR coefficients
     * @param [in] process_block_size_adjust specify processing block size (0..)
     *
     * @note Processing block size is calculated by the following equation:
     *         {processing block size} = {filter_length * ((2 << process_block_size_adjust) - 1)}
     *       Large block size improves processing efficiency, however memory consumption and latency are increased.
     */
    single_channel_fft_x2_resampler(const coeffs_t *filter_kernel, int filter_length, int process_block_size_adjust);

    /**
     * Constructor (with shared context object).
     *
     * @param [in] p_shared_context shared context object
     */
    single_channel_fft_x2_resampler(const shared_context *p_shared_context);

    /**
     * Destructor.
     */
    ~single_channel_fft_x2_resampler();

    /**
     * Reset state.
     */
    void reset() CXXPH_NOEXCEPT;

    /**
     * Flush buffered data.
     */
    void flush() CXXPH_NOEXCEPT;

    /**
     * Put input (original) data
     *
     * @param s [in] source data buffer
     * @param n [in] count of data  (n >= 0 && n < num_can_put())
     * @param stride [in] buffer stride (>= 1)
     */
    void put_n(const source_data_t *s, int n, int stride = 1) CXXPH_NOEXCEPT;

    /**
     * Get output (resampled) data
     *
     * @param d [in] destination data buffer
     * @param n [in] count of data  (n >= 0 && n < num_can_get())
     * @param stride [in] buffer stride (>= 1)
     */
    void get_n(dest_data_t *d, int n, int stride = 1) CXXPH_NOEXCEPT;

    /**
     * Get free size of internal buffer.
     * @returns count of free size of internal buffer [frames]
     */
    int num_can_put() const CXXPH_NOEXCEPT;

    /**
     * Get count of available resampled data.
     * @returns count of available resampled data [frames]
     */
    int num_can_get() const CXXPH_NOEXCEPT;

    //
    // Advanced APIs
    //

    /**
     * Directly refer the internal output buffer.
     * @param d [out] buffer pointer
     * @param n [out] size of available resampled data [frames]
     */
    void refer_direct_output_buffer(const fft_real_t **d, int *n) CXXPH_NOEXCEPT;

    /**
     * Notify directly consumed data count.
     * @param n [in] count of consumed data [frames]
     */
    void notify_direct_consumed_output_buffer_items(int n) CXXPH_NOEXCEPT;

private:
    /// @cond INTERNAL_FIELD
    typedef typename shared_context::fft_forward_real fft_forward_real;
    typedef typename shared_context::fft_backward_real fft_backward_real;

    single_channel_fft_x2_resampler(const shared_context *p_shared_context, bool shared_context_is_private);

    int get_n_from_pooled_output_data(dest_data_t *CXXPH_RESTRICT dest, int n, int stride) CXXPH_NOEXCEPT;
    void fill_output_buffer() CXXPH_NOEXCEPT;
    /// @endcond

private:
    /// @cond INTERNAL_FIELD
    const shared_context *shared_context_;
    bool shared_context_is_private_;

    int num_pooled_input_data_;
    int num_pooled_output_data_;
    int num_removed_delay_;
    int num_appended_zero_samples_;
    int output_data_read_position_;
    bool flushed_;

    cxxporthelper::aligned_memory<fft_real_t> mem_fft_f_in_;
    cxxporthelper::aligned_memory<fft_complex_t> mem_fft_f_out_b_in_;
    cxxporthelper::aligned_memory<fft_real_t> mem_fft_b_out_;

    fft_forward_real fftr_f_;
    fft_backward_real fftr_b_;
    /// @endcond
};

//
// single_channel_fft_x2_resampler_shared_context
//
template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline single_channel_fft_x2_resampler_shared_context<
    TSrc, TDest, TCoeffs, TFFTBackend>::single_channel_fft_x2_resampler_shared_context(const coeffs_t *filter_kernel,
                                                                                       int filter_length,
                                                                                       int process_block_size_adjust)
    : filter_length_(0), process_block_size_adjust_(0), mem_f_filter_kernel_(), m_(0), n_(0), n2_(0), l_(0), delay_(0)
{
    assert(utils::is_pow_of_two(filter_length));

    const int M = filter_length;
    const int K = (2 << process_block_size_adjust);
    const int N = M * K;
    const int L = N - M;
    const int N2 = utils::forward_fft_real_num_outputs(N); // == N/2+1

    cxxporthelper::aligned_memory<fft_real_t> mem_filter_kernel(N, FFT_MEMORY_ALIGNMENT);
    cxxporthelper::aligned_memory<fft_complex_t> mem_f_filter_kernel(N2, FFT_MEMORY_ALIGNMENT);

    // calculate frequency response of the filter
    fft_forward_real fftr_f_filter(N, &mem_filter_kernel[0], &mem_f_filter_kernel[0]);

    utils::fast_pod_copy(&mem_filter_kernel[0], &filter_kernel[0], M);
    ::memset(&mem_filter_kernel[M], 0, sizeof(fft_real_t) * L);

    fftr_f_filter.execute();

    const fft_real_t post_scale = fft_real_t(1) / fftr_f_filter.scale();
    if (post_scale != fft_real_t(1)) {
        utils::multiply_scaler_aligned(&mem_f_filter_kernel[0], post_scale, N2);
    }

    // update fields
    filter_length_ = filter_length;
    process_block_size_adjust_ = process_block_size_adjust;
    mem_f_filter_kernel_ = std::move(mem_f_filter_kernel);

    m_ = M;
    n_ = N;
    n2_ = N2;
    l_ = L;
    delay_ = (M / 2);
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline single_channel_fft_x2_resampler_shared_context<TSrc, TDest, TCoeffs,
                                                      TFFTBackend>::~single_channel_fft_x2_resampler_shared_context()
{
}

//
// single_channel_fft_x2_resampler
//
template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::single_channel_fft_x2_resampler(
    const coeffs_t *filter_kernel, int filter_length, int process_block_size_adjust)
    : single_channel_fft_x2_resampler(new shared_context(filter_kernel, filter_length, process_block_size_adjust), true)
{
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::single_channel_fft_x2_resampler(
    const shared_context *p_shared_context)
    : single_channel_fft_x2_resampler(p_shared_context, false)
{
}

/// @cond INTERNAL_FIELD
template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::single_channel_fft_x2_resampler(
    const shared_context *p_shared_context, bool shared_context_is_private)
    : shared_context_(nullptr), shared_context_is_private_(false), num_pooled_input_data_(0),
      num_pooled_output_data_(0), num_removed_delay_(0), num_appended_zero_samples_(0), output_data_read_position_(0),
      flushed_(false), mem_fft_f_in_(), mem_fft_f_out_b_in_(), mem_fft_b_out_(), fftr_f_(), fftr_b_()
{
    const int M = p_shared_context->filter_length_;
    const int N = p_shared_context->N();
    const int N2 = p_shared_context->N2();

    try
    {
        // allocate memory blocks
        cxxporthelper::aligned_memory<fft_real_t> mem_fft_f_in(N / 2, FFT_MEMORY_ALIGNMENT);
        cxxporthelper::aligned_memory<fft_complex_t> mem_fft_f_out_b_in(N2, FFT_MEMORY_ALIGNMENT);
        cxxporthelper::aligned_memory<fft_real_t> mem_fft_b_out(N, FFT_MEMORY_ALIGNMENT);

        // create fft objects
        fft_forward_real fftr_f(N / 2, &mem_fft_f_in[0], &mem_fft_f_out_b_in[0]);
        fft_backward_real fftr_b(N, &mem_fft_f_out_b_in[0], &mem_fft_b_out[0]);

        // update fields
        shared_context_ = p_shared_context;
        shared_context_is_private_ = shared_context_is_private;

        mem_fft_f_in_ = std::move(mem_fft_f_in);
        mem_fft_f_out_b_in_ = std::move(mem_fft_f_out_b_in);
        mem_fft_b_out_ = std::move(mem_fft_b_out);

        fftr_f_ = std::move(fftr_f);
        fftr_b_ = std::move(fftr_b);
    }
    catch (...)
    {
        if (shared_context_is_private) {
            delete p_shared_context;
        }
        throw;
    }

    // reset
    reset();
}
/// @endcond

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::~single_channel_fft_x2_resampler()
{
    if (shared_context_is_private_) {
        delete shared_context_;
        shared_context_ = nullptr;
    }
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::reset() CXXPH_NOEXCEPT
{
    num_pooled_input_data_ = 0;
    num_pooled_output_data_ = 0;
    num_removed_delay_ = 0;
    num_appended_zero_samples_ = 0;
    output_data_read_position_ = 0;
    flushed_ = false;

    // clear input buffer
    ::memset(&mem_fft_f_in_[0], 0, sizeof(fft_real_t) * mem_fft_f_in_.size());
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::flush() CXXPH_NOEXCEPT
{
    if (CXXPH_UNLIKELY(flushed_)) {
        return;
    }
    flushed_ = true;
    fill_output_buffer();
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::put_n(const source_data_t *s, int n,
                                                                                      int stride) CXXPH_NOEXCEPT
{
    assert(n <= num_can_put());

    const int M = shared_context_->M();
    const size_t offset = num_pooled_input_data_ + (M / 2);
    fft_real_t *CXXPH_RESTRICT in_data = fftr_f_.in();

    utils::stride_pod_copy(&in_data[offset], 1, &s[0], stride, n);

    num_pooled_input_data_ += n;

    fill_output_buffer();
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::get_n(dest_data_t *d, int n, int stride)
    CXXPH_NOEXCEPT
{
    assert(n <= num_can_get());

    get_n_from_pooled_output_data(&d[0], n, stride);
}

/// @cond INTERNAL_FIELD
template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline int single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::get_n_from_pooled_output_data(
    dest_data_t *CXXPH_RESTRICT dest, int n, int stride) CXXPH_NOEXCEPT
{
    const fft_real_t *out_data = nullptr;
    int n_available = 0;

    refer_direct_output_buffer(&out_data, &n_available);

    const int n_copy = (std::min)(n, n_available);

    utils::stride_pod_copy(&dest[0], stride, &out_data[0], 1, n_copy);

    notify_direct_consumed_output_buffer_items(n_copy);

    return n_copy;
}
/// @endcond

/// @cond INTERNAL_FIELD
template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::fill_output_buffer() CXXPH_NOEXCEPT
{
    const int N = shared_context_->N();
    const int N2 = shared_context_->N2();
    const int M = shared_context_->M();
    const int L = shared_context_->L();
    const int delay = shared_context_->delay();

    if (CXXPH_LIKELY(output_data_read_position_ < num_pooled_output_data_)) {
        // output buffer is not empty
        return;
    }

    if (CXXPH_LIKELY(!flushed_)) {
        assert(num_pooled_input_data_ <= (L / 2));
        if (num_pooled_input_data_ < (L / 2)) {
            // input buffer is not filled
            return;
        }
    } else {
        if (num_pooled_input_data_ == 0 && num_appended_zero_samples_ == delay) {
            // input buffer is empty & already zero samples appended
            return;
        }

        // pad zeros
        const int n_pad_zeros = (L / 2) - num_pooled_input_data_;

        if (CXXPH_UNLIKELY(n_pad_zeros > 0)) {
            fft_real_t *CXXPH_RESTRICT in_data = fftr_f_.in();
            size_t offset = num_pooled_input_data_ + (M / 2);
            ::memset(&in_data[offset], 0, sizeof(fft_real_t) * n_pad_zeros);
        }

        // append zero samples
        if (num_appended_zero_samples_ < delay) {
            const int n_zeros = (std::min)(n_pad_zeros, (delay - num_appended_zero_samples_) / 2);
            num_appended_zero_samples_ += n_zeros * 2;
            num_pooled_input_data_ += n_zeros;
        }
    }

    fft_complex_t *CXXPH_RESTRICT f_indata = fftr_f_.out();
    // forward FFT
    fftr_f_.execute();

    // mirror FFT result (x2 oversampling in frequency domain)
    utils::mirror_conj_aligned(&f_indata[N / 4], (N / 4));

    // apply filter
    utils::multiply_aligned(&f_indata[0], &(shared_context_->mem_f_filter_kernel_[0]), N2);

    // backward FFT
    fftr_b_.execute();

    // normalize
    const fft_real_t post_scale = fft_real_t(2) / (fftr_f_.scale() * fftr_b_.scale()); // 2: to cancel zero insertion effect
    if (post_scale != fft_real_t(1)) {
        fft_real_t *out_data = fftr_b_.out();
        utils::multiply_scaler_aligned(&out_data[M], post_scale, L);
    }

    num_pooled_output_data_ = num_pooled_input_data_ * 2;
    num_pooled_input_data_ = 0;
    output_data_read_position_ = 0;

    // remove delay
    if (CXXPH_UNLIKELY(num_removed_delay_ < delay)) {
        const int n_delays = (std::min)(num_pooled_output_data_, (delay - num_removed_delay_));
        num_removed_delay_ += n_delays;
        output_data_read_position_ += n_delays;
    }

    // copy the tail of the input data to head (overlap)
    fft_real_t *CXXPH_RESTRICT in_data = fftr_f_.in();
    utils::fast_pod_copy(&in_data[0], &in_data[L / 2], (M / 2));
}
/// @endcond

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline int single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::num_can_put() const CXXPH_NOEXCEPT
{
    const int L = shared_context_->L();

    if (CXXPH_LIKELY(!flushed_)) {
        return ((L / 2) - num_pooled_input_data_);
    } else {
        return 0;
    }
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline int single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::num_can_get() const CXXPH_NOEXCEPT
{
    const int L = shared_context_->L();
    const int delay = shared_context_->delay();

    int n = 0;

    n += (num_pooled_output_data_ - output_data_read_position_);
    n -= (delay - num_removed_delay_);
    n = (std::max)(n, 0);

    if (CXXPH_UNLIKELY(flushed_)) {
        n += (delay - num_appended_zero_samples_);
    }

    return n;
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void
single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::refer_direct_output_buffer(const fft_real_t **d,
                                                                                               int *n) CXXPH_NOEXCEPT
{
    assert(d);
    assert(n);

    fill_output_buffer();

    const int M = shared_context_->M();
    const fft_real_t *CXXPH_RESTRICT out_data = fftr_b_.out();
    const size_t offset = M + output_data_read_position_;

    (*d) = &out_data[offset];
    (*n) = (num_pooled_output_data_ - output_data_read_position_);
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void
single_channel_fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::notify_direct_consumed_output_buffer_items(int n)
    CXXPH_NOEXCEPT
{
    assert(n <= (num_pooled_output_data_ - output_data_read_position_));

    output_data_read_position_ += n;

    fill_output_buffer();
}

} // namespace resampler
} // namespace cxxdasp

#endif // CXXDASP_RESAMPLER_FFT_SINGLE_CHANNEL_FFT_X2_RESAMPLER_HPP_
