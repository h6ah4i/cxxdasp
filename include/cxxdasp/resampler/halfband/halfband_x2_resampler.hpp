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

#ifndef CXXDASP_RESAMPLER_HALFBAND_HALFBAND_X2_RESAMPLER_HPP_
#define CXXDASP_RESAMPLER_HALFBAND_HALFBAND_X2_RESAMPLER_HPP_

#include <algorithm>
#include <cstring>
#include <cassert>

#include <cxxporthelper/type_traits>
#include <cxxporthelper/utility>
#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/utils/utils.hpp>

namespace cxxdasp {
namespace resampler {

/**
 * Half band filter based x2 resampler class.
 *
 * @tparam TSrcFrame source audio frame type
 * @tparam TDestFrame destination audio frame type
 * @tparam TCoeffs FIR coefficient data type
 * @tparam THBFRCoreOperator Half band filter resampler implementation class type
 */
template <typename TSrc, typename TDest, typename TCoeffs, class THBFRCoreOperator>
class halfband_x2_resampler {

    /// @cond INTERNAL_FIELD
    halfband_x2_resampler(const halfband_x2_resampler &) = delete;
    halfband_x2_resampler &operator=(const halfband_x2_resampler &) = delete;
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
     * Operator
     */
    typedef THBFRCoreOperator core_operator_type;

    /**
     * Constructor (with normal FIR coefficients array).
     *
     * @param [in] filter_kernel FIR coefficients
     * @param [in] filter_length number of FIR coefficients
     * @param [in] process_block_size_adjust specify processing block size (0..)
     *
     * @note Processing block size is calculated by the following equation:
     *         {processing block size} = {(4 * filter_length) * (1 << process_block_size_adjust)}
     *       Large block size improves processing efficiency, however memory consumption and latency are increased.
     */
    halfband_x2_resampler(const coeffs_t *filter_kernel, int filter_length, int process_block_size_adjust);

    /**
     * Destructor.
     */
    ~halfband_x2_resampler();

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
    void process_flush() CXXPH_NOEXCEPT;

private:
    const core_operator_type core_operator_;

    cxxporthelper::aligned_memory<coeffs_t> filter_kernel_;
    cxxporthelper::aligned_memory<src_frame_t> src_buff_;
    int filter_length_;
    int process_block_size_;
    int read_pos_;
    int write_pos_;
    int pending_flush_count_;
    bool flushed_;
};

template <typename TSrc, typename TDest, typename TCoeffs, class THBFRCoreOperator>
inline halfband_x2_resampler<TSrc, TDest, TCoeffs, THBFRCoreOperator>::halfband_x2_resampler(
    const coeffs_t *filter_kernel, int filter_length, int process_block_size_adjust)
    : core_operator_(), filter_kernel_(), filter_length_(filter_length), src_buff_(), process_block_size_(0),
      read_pos_(0), write_pos_(0), pending_flush_count_(0), flushed_(false)
{
    process_block_size_ = (4 * filter_length) * (1 << process_block_size_adjust);

    assert(utils::is_pow_of_two(filter_length));
    assert(filter_length >= 4);

    const size_t coeffs_align = CXXPH_PLATFORM_SIMD_ALIGNMENT;
    filter_kernel_.allocate(((filter_length + coeffs_align - 1) / coeffs_align) * coeffs_align * 2);

    // copy coefficient data
    const coeffs_t *CXXPH_RESTRICT cf = &filter_kernel[0];
    const coeffs_t *CXXPH_RESTRICT cb = &filter_kernel[filter_length - 1];

    coeffs_t *CXXPH_RESTRICT mcf = &filter_kernel_[0];
    coeffs_t *CXXPH_RESTRICT mcb = &filter_kernel_[filter_kernel_.size() / 2];

    for (int i = 0; i < filter_length; ++i) {
        mcf[i] = (*cf);
        mcb[i] = (*cb);
        ++cf;
        --cb;
    }

    src_buff_.allocate(process_block_size_);

    reset();
}

template <typename TSrc, typename TDest, typename TCoeffs, class THBFRCoreOperator>
inline halfband_x2_resampler<TSrc, TDest, TCoeffs, THBFRCoreOperator>::~halfband_x2_resampler()
{
}

template <typename TSrc, typename TDest, typename TCoeffs, class THBFRCoreOperator>
inline void halfband_x2_resampler<TSrc, TDest, TCoeffs, THBFRCoreOperator>::reset() CXXPH_NOEXCEPT
{
    const int N = filter_length_ * 2;
    const int HN = filter_length_;

    ::memset(&(src_buff_[0]), 0, sizeof(src_frame_t) * HN);

    write_pos_ = HN - 1;
    read_pos_ = 2 * N;

    pending_flush_count_ = 0;
    flushed_ = false;
}

template <typename TSrc, typename TDest, typename TCoeffs, class THBFRCoreOperator>
inline void halfband_x2_resampler<TSrc, TDest, TCoeffs, THBFRCoreOperator>::flush() CXXPH_NOEXCEPT
{
    const int N = filter_length_ * 2;
    const int HN = filter_length_;

    if (CXXPH_UNLIKELY(flushed_)) {
        return;
    }

    flushed_ = true;
    pending_flush_count_ = (HN + 1);

    process_flush();
}

template <typename TSrc, typename TDest, typename TCoeffs, class THBFRCoreOperator>
inline void halfband_x2_resampler<TSrc, TDest, TCoeffs, THBFRCoreOperator>::put_n(const src_frame_t *s, int n)
    CXXPH_NOEXCEPT
{
    assert(n <= num_can_put());

    src_frame_t *buff = &src_buff_[write_pos_];
    for (int i = 0; i < n; ++i) {
        buff[i] = s[i];
    }
    write_pos_ += n;
}

template <typename TSrc, typename TDest, typename TCoeffs, class THBFRCoreOperator>
inline void halfband_x2_resampler<TSrc, TDest, TCoeffs, THBFRCoreOperator>::get_n(dest_frame_t *d, int n) CXXPH_NOEXCEPT
{
    assert(n <= num_can_get());

    if (n <= 0) {
        return;
    }

    int rp = read_pos_;

    const int n1 = (rp & 1);
    const int n2 = (n - n1) / 2;
    const int n3 = (n - (n1 + n2 * 2));

    const int N = filter_length_ * 2;
    const int HN = filter_length_;
    const src_frame_t *CXXPH_RESTRICT src_f = &(src_buff_[(rp - (2 * N)) >> 1]);
    const src_frame_t *CXXPH_RESTRICT src_b = &(src_f[HN]);
    const coeffs_t *CXXPH_RESTRICT coeffs_f = &(filter_kernel_[0]);
    const coeffs_t *CXXPH_RESTRICT coeffs_b = &(coeffs_f[filter_kernel_.size() / 2]);

    if (CXXPH_UNLIKELY(n1 != 0)) {
        // odd
        core_operator_.dual_convolve(d, src_f, src_b, coeffs_f, coeffs_b, HN);

        src_f += 1;
        src_b += 1;
        d += 1;
    }

    if (CXXPH_LIKELY(n2 != 0)) {
        for (int i = n2; i != 0; --i) {
            // even
            (*d) = src_f[HN - 1];
            d += 1;

            // odd
            core_operator_.dual_convolve(d, src_f, src_b, coeffs_f, coeffs_b, HN);

            src_f += 1;
            src_b += 1;
            d += 1;
        }
    }

    if (CXXPH_UNLIKELY(n3 != 0)) {
        // even
        (*d) = src_f[HN - 1];
    }

    read_pos_ += n;

    if (CXXPH_UNLIKELY(read_pos_ == (process_block_size_ * 2))) {
        assert(write_pos_ == process_block_size_);

        ::memcpy(&(src_buff_[0]), &(src_buff_[process_block_size_ - N]), (sizeof(src_frame_t) * N));

        write_pos_ = N;
        read_pos_ = 2 * N;

        process_flush();
    }
}

template <typename TSrc, typename TDest, typename TCoeffs, class THBFRCoreOperator>
inline int halfband_x2_resampler<TSrc, TDest, TCoeffs, THBFRCoreOperator>::num_can_put() const CXXPH_NOEXCEPT
{
    if (CXXPH_UNLIKELY(flushed_)) {
        return 0;
    } else {
        return (process_block_size_ - write_pos_);
    }
}

template <typename TSrc, typename TDest, typename TCoeffs, class THBFRCoreOperator>
inline int halfband_x2_resampler<TSrc, TDest, TCoeffs, THBFRCoreOperator>::num_can_get() const CXXPH_NOEXCEPT
{
    return (std::max)(0, ((write_pos_ * 2) - read_pos_));
}

template <typename TSrc, typename TDest, typename TCoeffs, class THBFRCoreOperator>
inline void halfband_x2_resampler<TSrc, TDest, TCoeffs, THBFRCoreOperator>::process_flush() CXXPH_NOEXCEPT
{
    if (CXXPH_UNLIKELY(pending_flush_count_ > 0)) {
        const int n_flush = (std::min)(pending_flush_count_, (process_block_size_ - write_pos_));
        ::memset(&src_buff_[write_pos_], 0, (sizeof(src_frame_t) * n_flush));
        write_pos_ += n_flush;
        pending_flush_count_ -= n_flush;
    }
}

} // namespace resampler
} // namespace cxxdasp

#endif // CXXDASP_RESAMPLER_HALFBAND_HALFBAND_X2_RESAMPLER_HPP_
