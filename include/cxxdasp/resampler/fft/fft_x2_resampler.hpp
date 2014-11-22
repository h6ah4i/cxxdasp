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

#ifndef CXXDASP_RESAMPLER_FFT_FFT_X2_RESAMPLER_RESAMPLER_HPP_
#define CXXDASP_RESAMPLER_FFT_FFT_X2_RESAMPLER_RESAMPLER_HPP_

#include <cxxporthelper/utility>
#include <cxxporthelper/memory>
#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>
#include <cxxdasp/resampler/fft/single_channel_fft_x2_resampler.hpp>

namespace cxxdasp {
namespace resampler {

/**
 * FFT x2 resampler (multi channel)
 *
 * @tparam Tsrc source audio frame type
 * @tparam TDest destination audio frame type
 * @tparam TCoeffs FIR coefficients type
 * @tparam TFFTBackend FFT backend class
 */
template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
class fft_x2_resampler {

    /// @cond INTERNAL_FIELD
    fft_x2_resampler(const fft_x2_resampler &) = delete;
    fft_x2_resampler &operator=(const fft_x2_resampler &) = delete;
    /// @endcond

public:
    /**
     * Source audio frame type.
     */
    typedef TSrc src_frame_t;

    /**
     * Destination audio frame type.
     */
    typedef TDest dest_frame_t;

    /**
     * FIR coefficients type.
     */
    typedef TCoeffs coeffs_t;

    /**
     * FFT backend class.
     */
    typedef TFFTBackend fft_backend_type;

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
    fft_x2_resampler(const coeffs_t *filter_kernel, int filter_length, int process_block_size_adjust);

    /**
     * Destructor.
     */
    ~fft_x2_resampler();

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
     */
    void put_n(const src_frame_t *s, int n) CXXPH_NOEXCEPT;

    /**
     * Get output (resampled) data
     *
     * @param d [in] destination data buffer
     * @param n [in] count of data  (n >= 0 && n < num_can_get())
     */
    void get_n(dest_frame_t *d, int n) CXXPH_NOEXCEPT;

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
    void refer_direct_output_buffer(const dest_frame_t **d, int *n) CXXPH_NOEXCEPT;

    /**
     * Notify directly consumed data count.
     * @param n [in] count of consumed data [frames]
     */
    void notify_direct_consumed_output_buffer_items(int n) CXXPH_NOEXCEPT;

private:
    /// @cond INTERNAL_FIELD
    typedef typename src_frame_t::data_type src_data_type;
    typedef typename dest_frame_t::data_type dest_data_type;

    typedef single_channel_fft_x2_resampler_shared_context<src_data_type, src_data_type, coeffs_t, fft_backend_type>
    shared_context_type;
    typedef typename shared_context_type::resampler_class single_channel_resampler_type;

    enum { num_channels = src_frame_t::num_channels, };

    CXXPH_OPTIONAL_CONSTEXPR bool is_monaural_dest_optimized_mode() const CXXPH_NOEXCEPT
    {
        return (num_channels == 1 && std::is_same<typename fft_backend_type::fft_real_t, dest_data_type>::value);
    }

    CXXPH_OPTIONAL_CONSTEXPR bool is_stereo_src_optimized_mode() const CXXPH_NOEXCEPT
    {
        return (num_channels == 2 && std::is_same<typename fft_backend_type::fft_real_t, src_data_type>::value);
    }

    CXXPH_OPTIONAL_CONSTEXPR bool is_stereo_dest_optimized_mode() const CXXPH_NOEXCEPT
    {
        return (num_channels == 2 && std::is_same<typename fft_backend_type::fft_real_t, dest_data_type>::value);
    }

    // fields
    const shared_context_type shared_context_;
    std::unique_ptr<single_channel_resampler_type>
    channel_resamplers_[num_channels]; // NOTE: fft_x2_resampler only supports monaural
    cxxporthelper::aligned_memory<uint8_t> work_memory_;

    // verify template parameters
    static_assert(static_cast<int>(src_frame_t::num_channels) == static_cast<int>(dest_frame_t::num_channels),
                  "channel count requirements");

    /// @endcond
};

//
// fft_x2_resampler
//
template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::fft_x2_resampler(const coeffs_t *filter_kernel,
                                                                             int filter_length,
                                                                             int process_block_size_adjust)
    : shared_context_(filter_kernel, filter_length, process_block_size_adjust), work_memory_()
{
    std::unique_ptr<single_channel_resampler_type> channel_resamplers[num_channels];
    cxxporthelper::aligned_memory<uint8_t> work_memory;

    try
    {
        int s2_block_size;

        for (auto &p : channel_resamplers) {
            p.reset(new single_channel_resampler_type(&shared_context_));
        }

        if (!is_monaural_dest_optimized_mode()) {
            work_memory.allocate(shared_context_.N() * num_channels *
                                 (std::max)(sizeof(src_data_type), sizeof(dest_data_type)));

            if (!work_memory) {
                throw std::bad_alloc();
            }
        }
    }
    catch (...) { throw; }

    // update fields
    for (int i = 0; i < num_channels; ++i) {
        channel_resamplers_[i] = std::move(channel_resamplers[i]);
    }
    work_memory_ = std::move(work_memory);
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::~fft_x2_resampler()
{
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::reset() CXXPH_NOEXCEPT
{
    for (auto &p : channel_resamplers_) {
        p->reset();
    }
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::flush() CXXPH_NOEXCEPT
{
    for (auto &p : channel_resamplers_) {
        p->flush();
    }
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::put_n(const src_frame_t *s, int n) CXXPH_NOEXCEPT
{
    if (is_stereo_src_optimized_mode()) {
        const int work_size = static_cast<int>(work_memory_.size() / sizeof(src_frame_t));
        src_data_type *CXXPH_RESTRICT work = reinterpret_cast<src_data_type *>(&work_memory_[0]);

        int offset = 0;
        do {
            const int nt = (std::min)(work_size, (n - offset));

            utils::deinterleave(&work[0], &work[work_size], reinterpret_cast<const src_data_type *>(&s[offset]), nt);

            channel_resamplers_[0]->put_n(&work[0], nt);
            channel_resamplers_[1]->put_n(&work[work_size], nt);

            offset += nt;
        } while (CXXPH_UNLIKELY(offset < n));
    } else {
        const src_data_type *s_buff = reinterpret_cast<const src_data_type *>(s);

        for (int i = 0; i < num_channels; ++i) {
            channel_resamplers_[i]->put_n(&s_buff[i], n, num_channels);
        }
    }
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::get_n(dest_frame_t *d, int n) CXXPH_NOEXCEPT
{
    typedef typename fft_backend_type::fft_real_t fft_real_t;

    if (is_stereo_dest_optimized_mode()) {
        int offset = 0;
        do {
            int n0 = 0, n1 = 0;
            const fft_real_t *d0 = nullptr, *d1 = nullptr;

            channel_resamplers_[0]->refer_direct_output_buffer(&d0, &n0);
            channel_resamplers_[1]->refer_direct_output_buffer(&d1, &n1);

            assert(n0 == n1);

            const int nt = (std::min)(n0, (n - offset));

            utils::interleave(reinterpret_cast<dest_data_type *>(&d[offset]), d0, d1, nt);

            channel_resamplers_[0]->notify_direct_consumed_output_buffer_items(nt);
            channel_resamplers_[1]->notify_direct_consumed_output_buffer_items(nt);

            offset += nt;
        } while (CXXPH_UNLIKELY(offset < n));
    } else {
        dest_data_type *d_buff = reinterpret_cast<dest_data_type *>(d);

        for (int i = 0; i < num_channels; ++i) {
            channel_resamplers_[i]->get_n(&d_buff[i], n, num_channels);
        }
    }
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline int fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::num_can_put() const CXXPH_NOEXCEPT
{
    return channel_resamplers_[0]->num_can_put();
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline int fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::num_can_get() const CXXPH_NOEXCEPT
{
    return channel_resamplers_[0]->num_can_get();
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::refer_direct_output_buffer(const dest_frame_t **d,
                                                                                            int *n) CXXPH_NOEXCEPT
{
    typedef typename fft_backend_type::fft_real_t fft_real_t;

    assert(d);
    assert(n);

    if (is_monaural_dest_optimized_mode()) {
        channel_resamplers_[0]->refer_direct_output_buffer(reinterpret_cast<const fft_real_t **>(d), n);
    } else if (is_stereo_dest_optimized_mode()) {
        const int work_size = static_cast<int>(work_memory_.size() / sizeof(dest_frame_t));
        dest_data_type *CXXPH_RESTRICT work = reinterpret_cast<dest_data_type *>(&work_memory_[0]);

        int n0 = 0, n1 = 0;
        const fft_real_t *d0 = nullptr, *d1 = nullptr;

        channel_resamplers_[0]->refer_direct_output_buffer(&d0, &n0);
        channel_resamplers_[1]->refer_direct_output_buffer(&d1, &n1);

        assert(n0 == n1);
        assert(n0 <= work_size);

        if (n0 > 0) {
            utils::interleave(&work[0], d0, d1, n0);
            (*d) = reinterpret_cast<const dest_frame_t *>(work);
            (*n) = n0;
        } else {
            (*d) = nullptr;
            (*n) = 0;
        }
    } else {
        const int work_size = static_cast<int>(work_memory_.size() / sizeof(dest_frame_t));
        dest_data_type *CXXPH_RESTRICT work = reinterpret_cast<dest_data_type *>(&work_memory_[0]);

        int n_available = 0;
        const fft_real_t *ptr = nullptr;

        channel_resamplers_[0]->refer_direct_output_buffer(&ptr, &n_available);

        if (n_available > 0) {
            utils::stride_pod_copy(&work[0], num_channels, ptr, 1, n_available);
            assert(n_available <= work_size);

            for (int i = 1; i < num_channels; ++i) {
                channel_resamplers_[i]->refer_direct_output_buffer(&ptr, &n_available);

                utils::stride_pod_copy(&work[i], num_channels, ptr, 1, n_available);
                assert(n_available <= work_size);
            }

            (*d) = reinterpret_cast<const dest_frame_t *>(work);
            (*n) = n_available;
        } else {
            (*d) = nullptr;
            (*n) = 0;
        }
    }
}

template <typename TSrc, typename TDest, typename TCoeffs, class TFFTBackend>
inline void fft_x2_resampler<TSrc, TDest, TCoeffs, TFFTBackend>::notify_direct_consumed_output_buffer_items(int n)
    CXXPH_NOEXCEPT
{
    for (auto &p : channel_resamplers_) {
        p->notify_direct_consumed_output_buffer_items(n);
    }
}

} // namespace resampler
} // namespace cxxdasp

#endif // CXXDASP_RESAMPLER_FFT_FFT_X2_RESAMPLER_RESAMPLER_HPP_
