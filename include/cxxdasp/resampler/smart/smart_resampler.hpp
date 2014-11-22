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

#ifndef CXXDASP_RESAMPLER_SMART_SMART_RESAMPLER_HPP_
#define CXXDASP_RESAMPLER_SMART_SMART_RESAMPLER_HPP_

#include <cxxporthelper/utility>
#include <cxxporthelper/memory>
#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>
#include <cxxdasp/resampler/fft/fft_x2_resampler.hpp>
#include <cxxdasp/resampler/halfband/halfband_x2_resampler.hpp>
#include <cxxdasp/resampler/halfband/halfband_x2_core_operators.hpp>
#include <cxxdasp/resampler/polyphase/polyphase_resampler.hpp>
#include <cxxdasp/resampler/polyphase/polyphase_core_operators.hpp>
#include <cxxdasp/resampler/smart/smart_resampler_params.hpp>

namespace cxxdasp {
namespace resampler {

/**
 * Smart resampler
 *
 * @tparam Tsrc source audio frame type
 * @tparam TDest destination audio frame type
 * @tparam THBFRCoreOperator half band filter resampler core operator
 * @tparam TFFTBackend FFT backend class
 * @tparam TPolyCoreOperator polyphase filter core operator class
 */
template <typename TSrc, typename TDest, class THBFRCoreOperator, class TFFTBackend, class TPolyCoreOperator>
class smart_resampler {

    /// @cond INTERNAL_FIELD
    smart_resampler(const smart_resampler &) = delete;
    smart_resampler &operator=(const smart_resampler &) = delete;
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
     * Halfband  ilter x2 resampler core operator class.
     */
    typedef THBFRCoreOperator halfband_x2_resampler_operator_type;

    /**
     * FFT backend class.
     */
    typedef TFFTBackend fft_backend_type;

    /**
     * Polyphase filter core operator class.
     */
    typedef TPolyCoreOperator polyphase_operator_type;

    /**
     * Constructor.
     *
     * @param params [in] parameters
     */
    smart_resampler(const smart_resampler_params &params);

    /**
     * Destructor.
     */
    ~smart_resampler();

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

private:
    /// @cond INTERNAL_FIELD

    typedef typename smart_resampler_params::stage1_coeffs_t stage1_coeffs_t;
    typedef typename smart_resampler_params::stage2_coeffs_t stage2_coeffs_t;
    typedef typename src_frame_t::data_type src_data_type;
    typedef typename dest_frame_t::data_type dest_data_type;

    typedef fft_x2_resampler<src_frame_t, src_frame_t, stage1_coeffs_t, fft_backend_type> stage1_fft_resampler_type;
    typedef halfband_x2_resampler<src_frame_t, src_frame_t, stage1_coeffs_t, halfband_x2_resampler_operator_type>
    stage1_halfband_resampler_type;
    typedef polyphase_resampler<src_frame_t, dest_frame_t, stage2_coeffs_t, polyphase_operator_type>
    stage2_resampler_type;

    enum { num_channels = src_frame_t::num_channels, };

    void move_stage1_output_to_stage2_input() CXXPH_NOEXCEPT;
    void check_stage2_flush() CXXPH_NOEXCEPT;

    bool have_stage1() const CXXPH_NOEXCEPT { return params_.have_stage1; }

    bool have_stage2() const CXXPH_NOEXCEPT { return params_.have_stage2; }

    // fields
    const smart_resampler_params params_;
    bool stage1_flushed_;
    bool stage2_flushed_;

    std::unique_ptr<stage1_halfband_resampler_type> stage1_halfband_resampler_;
    std::unique_ptr<stage1_fft_resampler_type> stage1_fft_resampler_;
    std::unique_ptr<stage2_resampler_type> stage2_resampler_;

    // verify template parameters
    static_assert(static_cast<int>(src_frame_t::num_channels) == static_cast<int>(dest_frame_t::num_channels),
                  "channel count requirements");

    /// @endcond
};

template <typename TSrc, typename TDest, class THBFRCoreOperator, class TFFTBackend, class TPolyCoreOperator>
inline smart_resampler<TSrc, TDest, THBFRCoreOperator, TFFTBackend, TPolyCoreOperator>::smart_resampler(
    const smart_resampler_params &params)
    : params_(params), stage1_flushed_(false), stage2_flushed_(false), stage1_halfband_resampler_(),
      stage1_fft_resampler_(), stage2_resampler_()
{
    std::unique_ptr<stage1_halfband_resampler_type> s1_halfband_resampler;
    std::unique_ptr<stage1_fft_resampler_type> s1_fft_resampler;
    std::unique_ptr<stage2_resampler_type> s2_resampler;

    try
    {
        const smart_resampler_params::stage1_x2_fir_info &s1 = params.stage1;
        const smart_resampler_params::stage2_poly_fir_info &s2 = params.stage2;

        int s2_block_size;

        if (params.have_stage1 && !s1.use_fft_resampler) {
            int k;
            for (k = 0; (s1.n_coeffs << k) < 512; ++k)
                ;
            s1_halfband_resampler.reset(new stage1_halfband_resampler_type(s1.coeffs, s1.n_coeffs, k));
            s2_block_size = (4 * s1.n_coeffs) * (1 << k);
        } else if (params.have_stage1 && s1.use_fft_resampler) {
            int k;
            for (k = 0; (s1.n_coeffs << k) < 4096; ++k)
                ;
            s1_fft_resampler.reset(new stage1_fft_resampler_type(s1.coeffs, s1.n_coeffs, k));
            s2_block_size = (((2 << k) - 1) * s1.n_coeffs);
        } else {
            s2_block_size = 4096;
        }

        if (params.have_stage2) {
            s2_resampler.reset(
                new stage2_resampler_type(s2.coeffs, s2.n_coeffs, !(s2.is_static), s2.m, s2.l, s2_block_size));
        }
    }
    catch (...) { throw; }

    // update fields
    stage1_halfband_resampler_ = std::move(s1_halfband_resampler);
    stage1_fft_resampler_ = std::move(s1_fft_resampler);
    stage2_resampler_ = std::move(s2_resampler);
}

template <typename TSrc, typename TDest, class THBFRCoreOperator, class TFFTBackend, class TPolyCoreOperator>
inline smart_resampler<TSrc, TDest, THBFRCoreOperator, TFFTBackend, TPolyCoreOperator>::~smart_resampler()
{
}

template <typename TSrc, typename TDest, class THBFRCoreOperator, class TFFTBackend, class TPolyCoreOperator>
inline void smart_resampler<TSrc, TDest, THBFRCoreOperator, TFFTBackend, TPolyCoreOperator>::reset() CXXPH_NOEXCEPT
{
    if (have_stage1()) {
        if (stage1_fft_resampler_) {
            stage1_fft_resampler_->reset();
        } else if (CXXPH_LIKELY(stage1_halfband_resampler_)) {
            stage1_halfband_resampler_->reset();
        } else {
            assert(false);
        }
    }

    if (have_stage2()) {
        stage2_resampler_->reset();
    }

    stage1_flushed_ = false;
    stage2_flushed_ = false;
}

template <typename TSrc, typename TDest, class THBFRCoreOperator, class TFFTBackend, class TPolyCoreOperator>
inline void smart_resampler<TSrc, TDest, THBFRCoreOperator, TFFTBackend, TPolyCoreOperator>::flush() CXXPH_NOEXCEPT
{
    if (have_stage1() && have_stage2()) {
        if (stage1_fft_resampler_) {
            stage1_fft_resampler_->flush();
        } else if (CXXPH_LIKELY(stage1_halfband_resampler_)) {
            stage1_halfband_resampler_->flush();
        } else {
            assert(false);
        }

        stage1_flushed_ = true;

        check_stage2_flush();
        move_stage1_output_to_stage2_input();
        check_stage2_flush();
    } else if (have_stage2()) {
        stage2_resampler_->flush();

        stage1_flushed_ = true;
        stage2_flushed_ = true;
    }
}

template <typename TSrc, typename TDest, class THBFRCoreOperator, class TFFTBackend, class TPolyCoreOperator>
inline void smart_resampler<TSrc, TDest, THBFRCoreOperator, TFFTBackend, TPolyCoreOperator>::put_n(const src_frame_t *s,
                                                                                                   int n) CXXPH_NOEXCEPT
{
    if (have_stage1() && have_stage2()) {
        if (stage1_fft_resampler_) {
            stage1_fft_resampler_->put_n(s, n);
        } else if (CXXPH_LIKELY(stage1_halfband_resampler_)) {
            stage1_halfband_resampler_->put_n(s, n);
        } else {
            assert(false);
        }

        move_stage1_output_to_stage2_input();
        check_stage2_flush();
    } else if (have_stage2()) {
        stage2_resampler_->put_n(s, n);
    }
}

template <typename TSrc, typename TDest, class THBFRCoreOperator, class TFFTBackend, class TPolyCoreOperator>
inline void smart_resampler<TSrc, TDest, THBFRCoreOperator, TFFTBackend, TPolyCoreOperator>::get_n(dest_frame_t *d,
                                                                                                   int n) CXXPH_NOEXCEPT
{
    if (have_stage1() && have_stage2()) {
        stage2_resampler_->get_n(d, n);

        move_stage1_output_to_stage2_input();
        check_stage2_flush();
    } else if (have_stage2()) {
        stage2_resampler_->get_n(d, n);
    }
}

template <typename TSrc, typename TDest, class THBFRCoreOperator, class TFFTBackend, class TPolyCoreOperator>
inline int smart_resampler<TSrc, TDest, THBFRCoreOperator, TFFTBackend, TPolyCoreOperator>::num_can_put() const
    CXXPH_NOEXCEPT
{
    if (stage1_flushed_) {
        return 0;
    }

    if (have_stage1() && have_stage2()) {
        if (stage1_fft_resampler_) {
            return stage1_fft_resampler_->num_can_put();
        } else if (CXXPH_LIKELY(stage1_halfband_resampler_)) {
            return stage1_halfband_resampler_->num_can_put();
        } else {
            assert(false);
            return 0;
        }
    } else if (have_stage2()) {
        return stage2_resampler_->num_can_put();
    }

    return 0;
}

template <typename TSrc, typename TDest, class THBFRCoreOperator, class TFFTBackend, class TPolyCoreOperator>
inline int smart_resampler<TSrc, TDest, THBFRCoreOperator, TFFTBackend, TPolyCoreOperator>::num_can_get() const
    CXXPH_NOEXCEPT
{
    if (have_stage2()) {
        return stage2_resampler_->num_can_get();
    }

    return 0;
}

/// @cond INTERNAL_FIELD
template <typename TSrc, typename TDest, class THBFRCoreOperator, class TFFTBackend, class TPolyCoreOperator>
inline void
smart_resampler<TSrc, TDest, THBFRCoreOperator, TFFTBackend, TPolyCoreOperator>::move_stage1_output_to_stage2_input()
    CXXPH_NOEXCEPT
{
    const int s2_n_can_put = stage2_resampler_->num_can_put();

    if (s2_n_can_put == 0) {
        return;
    }

    if (stage1_fft_resampler_) {
        auto &s1_resampler = stage1_fft_resampler_;
        const int s1_n_can_get = s1_resampler->num_can_get();

        int s2_remains = s2_n_can_put;
        while (s2_remains > 0) {
            const src_frame_t *s1_out_data = nullptr;
            int s1_n_available = 0;

            s1_resampler->refer_direct_output_buffer(&s1_out_data, &s1_n_available);

            if (s1_n_available == 0)
                break;

            const int n_consumed = (std::min)(s2_remains, s1_n_available);

            stage2_resampler_->put_n(s1_out_data, n_consumed);

            s1_resampler->notify_direct_consumed_output_buffer_items(n_consumed);

            s2_remains -= n_consumed;
        }
    } else if (CXXPH_LIKELY(stage1_halfband_resampler_)) {
        auto &s1_resampler = stage1_halfband_resampler_;
        src_frame_t work[128];

        int s2_remains = s2_n_can_put;
        while (CXXPH_LIKELY(s2_remains > 0)) {
            const int s1_n_available = s1_resampler->num_can_get();

            if (s1_n_available == 0)
                break;

            int n_consumed;
            n_consumed = (std::min)(s2_remains, s1_n_available);
            n_consumed = (std::min)(n_consumed, static_cast<int>(sizeof(work) / sizeof(work[0])));

            s1_resampler->get_n(work, n_consumed);

            stage2_resampler_->put_n(work, n_consumed);

            s2_remains -= n_consumed;
        }
    } else {
        assert(false);
    }
}

template <typename TSrc, typename TDest, class THBFRCoreOperator, class TFFTBackend, class TPolyCoreOperator>
inline void smart_resampler<TSrc, TDest, THBFRCoreOperator, TFFTBackend, TPolyCoreOperator>::check_stage2_flush()
    CXXPH_NOEXCEPT
{
    int s1_n_can_get;

    if (stage1_fft_resampler_) {
        s1_n_can_get = stage1_fft_resampler_->num_can_get();
    } else if (CXXPH_LIKELY(stage1_halfband_resampler_)) {
        s1_n_can_get = stage1_halfband_resampler_->num_can_get();
    } else {
        s1_n_can_get = 0;
        assert(false);
    }

    // flush stage 2 resampler after all of the stage 1 data has been consumed
    if (stage2_resampler_ && stage1_flushed_ && s1_n_can_get == 0 && !stage2_flushed_) {
        stage2_resampler_->flush();
        stage2_flushed_ = true;
    }
}
/// @endcond

} // namespace resampler
} // namespace cxxdasp

#endif // CXXDASP_RESAMPLER_SMART_SMART_RESAMPLER_HPP_
