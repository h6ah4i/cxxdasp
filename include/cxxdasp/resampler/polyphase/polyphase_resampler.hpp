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

#ifndef CXXDASP_RESAMPLER_POLYPHASE_RESAMPLER_HPP_
#define CXXDASP_RESAMPLER_POLYPHASE_RESAMPLER_HPP_

#include <memory>
#include <cassert>

#include <cxxporthelper/type_traits>
#include <cxxporthelper/utility>
#include <cxxporthelper/cstdint>
#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/cxxdasp_config.hpp>
#include <cxxdasp/datatype/audio_frame.hpp>
#include <cxxdasp/resampler/polyphase/polyphase_resampler_utils.hpp>
#include <cxxdasp/utils/utils.hpp>

// FIR delay line alignment
#define CXXDASP_POLYPHASE_RESAMPLER_DELAY_LINE_ALIGN_SIZE 32

// FIR coefficients alignment
#define CXXDASP_POLYPHASE_RESAMPLER_COEFFS_ARRAY_ALIGN_SIZE 32

namespace cxxdasp {
namespace resampler {

/**
 * Poly-phase FIR based resampler class.
 *
 * @tparam TSrcFrame source audio frame type
 * @tparam TDestFrame destination audio frame type
 * @tparam TCoeffs FIR coefficient data type
 * @tparam TPolyCoreOperator core operator class type
 */
template <typename TSrcFrame, typename TDestFrame, typename TCoeffs, class TPolyCoreOperator>
class polyphase_resampler {
    // validate template parameters
    static_assert(std::is_same<TSrcFrame, typename TPolyCoreOperator::src_frame_t>::value,
                  "source frame type is different");
    static_assert(std::is_same<TDestFrame, typename TPolyCoreOperator::dest_frame_t>::value,
                  "source frame type is different");
    static_assert(std::is_same<TCoeffs, typename TPolyCoreOperator::coeffs_t>::value,
                  "FIR coefficient type is different");

    /// @cond INTERNAL_FIELD
    polyphase_resampler(const polyphase_resampler &) = delete;
    polyphase_resampler &operator=(const polyphase_resampler &) = delete;
    /// @endcond

public:
    /** Data type of source audio frame */
    typedef TSrcFrame src_frame_t;

    /** Data type of destination audio frame */
    typedef TDestFrame dest_frame_t;

    /** Data type of FIR coefficients */
    typedef TCoeffs coeffs_t;

    /** Operator */
    typedef TPolyCoreOperator core_operator_type;

    /**
     * Constructor (with not-interleaved coefficients array).
     *
     * @param [in] coeffs FIR coefficients
     * @param [in] num_coeffs number of FIR coefficients
     * @param [in] m oversampling ratio
     * @param [in] l decimation ratio
     * @param [in] base_block_size specify additional internal delay line size to improve efficiency (1 >) [frames]
     *
     * @sa polyphase_resampler_utils::calc_interleaved_coeffs_subtable_size()
     * @sa polyphase_resampler_utils::make_interleaved_coeffs_table()
     *
     * @note {output rate} = (M / L) * {input rate}
     * @note ex.) 44100 -> 48000: M = 160, L = 147
     */
    polyphase_resampler(const coeffs_t *coeffs, int num_coeffs, int m, int l, int base_block_size);

    /**
     * Constructor (with interleaved coefficients array).
     *
     * @param [in] interleaved_coeffs FIR coefficients (poly-phase optimized form)
     * @param [in] num_coeffs number of FIR coefficients
     * @param [in] copy_coeffs specify whether the passed coefficients array requires to make a copy
     * @param [in] m oversampling ratio
     * @param [in] l decimation ratio
     * @param [in] base_block_size specify additional internal delay line size to improve efficiency (1 >) [frames]
     *
     * @sa polyphase_resampler_utils::calc_interleaved_coeffs_subtable_size()
     * @sa polyphase_resampler_utils::make_interleaved_coeffs_table()
     *
     * @note {output rate} = (M / L) * {input rate}
     * @note ex.) 44100 -> 48000: M = 160, L = 147
     */
    polyphase_resampler(const coeffs_t *interleaved_coeffs, int num_coeffs, bool copy_coeffs, int m, int l,
                        int base_block_size);

    /**
     * Destructor.
     */
    ~polyphase_resampler();

    /**
     * Reset internal state.
     */
    void reset() CXXPH_NOEXCEPT;

    /**
     * Flush internal buffer.
     *
     * @note call this function when after all source data put into the resampler.
     */
    void flush() CXXPH_NOEXCEPT;

    /**
     * Put multiple audio frames.
     *
     * @param [in] s pointer of source audio frames
     * @param [in] n count of source audio frames (n <= num_can_put())
     *
     * @sa num_can_put()
     *
     * @note you have to check whether the internal buffer is not full before calling this function.
     */
    void put_n(const src_frame_t *s, int n) CXXPH_NOEXCEPT;

    /**
     * Get multiple resampled audio frames.
     *
     * @param [out] d pointer of resampled audio frames
     * @param [in] n count of resample audio frames (n <= num_can_get())
     *
     * @sa num_can_get()
     *
     * @note you have to check whether the internal buffer is not empty before calling this function. (num_can_get() > 0)
     */
    void get_n(dest_frame_t *d, int n) CXXPH_NOEXCEPT;

    /**
     * Get available free buffer space size.
     *
     * \return available buffer space size [frames]
     *
     * @sa put_n()
     */
    int num_can_put() const CXXPH_NOEXCEPT;

    /**
     * Get resampling-ready frame count.
     *
     * \return resampling-ready count [frames]
     *
     * @sa get_n()
     */
    int num_can_get() const CXXPH_NOEXCEPT;

private:
    /// @cond INTERNAL_FIELD
    typedef polyphase_resampler_utils pprutils;

    static int convolve_n(int m, int subtable_size, dest_frame_t *CXXPH_RESTRICT dest,
                          const core_operator_type &core_op, const src_frame_t *CXXPH_RESTRICT delay,
                          const coeffs_t *CXXPH_RESTRICT interleaved_coeffs, int l, int n, int rp) CXXPH_NOEXCEPT
    {

        const float inv_m = 1.0f / m;

        for (int i = 0; i < n; ++i) {
            // const int rp_div_m = rp / m;     // <-- this is very SLOW (!)
            const int rp_div_m = static_cast<int>(rp * inv_m);
            const int subtable_index = ((m - 1) - (rp - rp_div_m * m));
            const src_frame_t *CXXPH_RESTRICT data = &delay[rp_div_m];
            const coeffs_t *CXXPH_RESTRICT coeffs = &interleaved_coeffs[subtable_index * subtable_size];

            core_op.convolve(&dest[i], data, coeffs, subtable_size);
            rp += l;
        }

        return rp;
    }

private:
    /// @cond INTERNAL_FIELD
    const core_operator_type core_operator_;
    const int num_coeffs_;
    const int m_;
    const int l_;

    const bool pass_through_; // M=1 && L=1 && num_coeffs = 1 && coeffs[0] == 1.0
    const bool sparse_copy_;  // M=1 && L!=1 && num_coeffs = 1 && coeffs[0] == 1.0

    const int interleaved_coeffs_subtable_size_;
    const int delay_line_size_;
    const int m_delay_line_size_;

    int count_;
    int write_pos_;
    int read_pos_;
    bool flushed_;

    const coeffs_t *interleaved_coeffs_;
    src_frame_t *delay_;

    cxxporthelper::aligned_memory<coeffs_t> mem_interleaved_coeffs_;
    cxxporthelper::aligned_memory<src_frame_t> mem_delay_;
    /// @endcond
};

template <typename TSrcFrame, typename TDestFrame, typename TCoeffs, typename TPolyCoreOperator>
inline polyphase_resampler<TSrcFrame, TDestFrame, TCoeffs, TPolyCoreOperator>::polyphase_resampler(
    const coeffs_t *coeffs, int num_coeffs, int m, int l, int base_block_size)
    : core_operator_(), num_coeffs_(num_coeffs), m_(m), l_(l),
      pass_through_(std::is_same<src_frame_t, dest_frame_t>::value &&
                    pprutils::check_is_pass_through(coeffs, num_coeffs, m, l)),
      sparse_copy_(std::is_same<src_frame_t, dest_frame_t>::value &&
                   pprutils::check_is_sparse_copy(coeffs, num_coeffs, m, l)),
      interleaved_coeffs_subtable_size_(pprutils::calc_interleaved_coeffs_subtable_size(num_coeffs, m)),
      delay_line_size_(pprutils::calc_delay_line_size(num_coeffs_, m, l, base_block_size)),
      m_delay_line_size_(delay_line_size_), count_(0), write_pos_(0), read_pos_(0), flushed_(false),
      interleaved_coeffs_(nullptr), delay_(nullptr)
{
    // allocate memory blocks
    cxxporthelper::aligned_memory<src_frame_t> mem_delay;
    cxxporthelper::aligned_memory<coeffs_t> mem_interleaved_coeffs;

    mem_delay.allocate((delay_line_size_ * ((pass_through_ || sparse_copy_) ? 1 : 2)),
                       CXXDASP_POLYPHASE_RESAMPLER_DELAY_LINE_ALIGN_SIZE);

    if (!(pass_through_ || sparse_copy_)) {
        mem_interleaved_coeffs.allocate((interleaved_coeffs_subtable_size_ * m_),
                                        CXXDASP_POLYPHASE_RESAMPLER_COEFFS_ARRAY_ALIGN_SIZE);

        // make interleaved coefficient array
        pprutils::make_interleaved_coeffs_table(coeffs, num_coeffs_, m_, &mem_interleaved_coeffs[0]);
    }

    // update fields
    mem_interleaved_coeffs_ = std::move(mem_interleaved_coeffs);
    mem_delay_ = std::move(mem_delay);

    delay_ = &mem_delay_[0];
    interleaved_coeffs_ = &mem_interleaved_coeffs_[0];

    // reset states
    reset();
}

template <typename TSrcFrame, typename TDestFrame, typename TCoeffs, typename TPolyCoreOperator>
inline polyphase_resampler<TSrcFrame, TDestFrame, TCoeffs, TPolyCoreOperator>::polyphase_resampler(
    const coeffs_t *interleaved_coeffs, int num_coeffs, bool copy_coeffs, int m, int l, int base_block_size)
    : core_operator_(), num_coeffs_(num_coeffs), m_(m), l_(l),
      interleaved_coeffs_subtable_size_(pprutils::calc_interleaved_coeffs_subtable_size(num_coeffs, m)),
      delay_line_size_(pprutils::calc_delay_line_size(num_coeffs_, m, l, base_block_size)),
      m_delay_line_size_(m_ * delay_line_size_),
      pass_through_(std::is_same<src_frame_t, dest_frame_t>::value &&
                    pprutils::check_is_pass_through(interleaved_coeffs, num_coeffs, m, l)),
      sparse_copy_(std::is_same<src_frame_t, dest_frame_t>::value &&
                   pprutils::check_is_sparse_copy(interleaved_coeffs, num_coeffs, m, l)),
      count_(0), write_pos_(0), read_pos_(0), flushed_(false), interleaved_coeffs_(nullptr), delay_(nullptr)
{
    // allocate memory blocks
    cxxporthelper::aligned_memory<src_frame_t> mem_delay;
    cxxporthelper::aligned_memory<coeffs_t> mem_interleaved_coeffs;

    mem_delay.allocate((delay_line_size_ * ((pass_through_ || sparse_copy_) ? 1 : 2)),
                       CXXDASP_POLYPHASE_RESAMPLER_DELAY_LINE_ALIGN_SIZE);

    if (!(pass_through_ || sparse_copy_)) {
        if (copy_coeffs || ((reinterpret_cast<uintptr_t>(interleaved_coeffs) %
                             CXXDASP_POLYPHASE_RESAMPLER_COEFFS_ARRAY_ALIGN_SIZE) != 0)) {
            mem_interleaved_coeffs.allocate((interleaved_coeffs_subtable_size_ * m_),
                                            CXXDASP_POLYPHASE_RESAMPLER_COEFFS_ARRAY_ALIGN_SIZE);
        }
    }

    // update fields
    mem_interleaved_coeffs_ = std::move(mem_interleaved_coeffs);
    mem_delay_ = std::move(mem_delay);

    delay_ = &mem_delay_[0];

    if (mem_interleaved_coeffs) {
        // make a copy of the passed coefficients array
        utils::fast_pod_copy(&mem_interleaved_coeffs[0], &interleaved_coeffs[0], num_coeffs_);

        interleaved_coeffs_ = &mem_interleaved_coeffs[0];
    } else {
        // just hold the passed coefficients array
        // (have to manage the life time of the array outside of this class!)
        interleaved_coeffs_ = interleaved_coeffs;
    }

    // reset states
    reset();
}

template <typename TSrcFrame, typename TDestFrame, typename TCoeffs, typename TPolyCoreOperator>
inline polyphase_resampler<TSrcFrame, TDestFrame, TCoeffs, TPolyCoreOperator>::~polyphase_resampler()
{
}

template <typename TSrcFrame, typename TDestFrame, typename TCoeffs, typename TPolyCoreOperator>
inline void polyphase_resampler<TSrcFrame, TDestFrame, TCoeffs, TPolyCoreOperator>::reset() CXXPH_NOEXCEPT
{
    for (int i = 0; i < mem_delay_.size(); ++i) {
        delay_[i] = 0.0f;
    }

    if (pass_through_ || sparse_copy_) {
        count_ = 0;
        write_pos_ = 0;
        read_pos_ = 0;
    } else {
        count_ = -(m_ - 1);
        write_pos_ = (num_coeffs_ / 2) / m_;
        read_pos_ = (m_ - 1) - ((num_coeffs_ / 2) % m_);
    }

    flushed_ = false;
}

template <typename TSrcFrame, typename TDestFrame, typename TCoeffs, typename TPolyCoreOperator>
inline void polyphase_resampler<TSrcFrame, TDestFrame, TCoeffs, TPolyCoreOperator>::flush() CXXPH_NOEXCEPT
{
    if (CXXPH_UNLIKELY(flushed_)) {
        return;
    }

    if (pass_through_ || sparse_copy_) {
        if (count_ > 0) {
            int n = (num_coeffs_ - 1);
            int k = (n + (m_ - 1)) / m_;
            int c = num_can_put();

            if (k > c) {
                k = c;
            }

            const src_frame_t zero_pad(0.0f);
            int orig_count = count_;
            for (int i = 0; i < k; ++i) {
                put_n(&zero_pad, 1);
            }

            if (count_ > (orig_count + n)) {
                count_ = orig_count + n;
            }
        }
    }

    flushed_ = true;
}

template <typename TSrcFrame, typename TDestFrame, typename TCoeffs, typename TPolyCoreOperator>
inline void polyphase_resampler<TSrcFrame, TDestFrame, TCoeffs, TPolyCoreOperator>::put_n(const src_frame_t *s, int n)
    CXXPH_NOEXCEPT
{

    assert(n <= num_can_put());

    const int dn = (delay_line_size_ - write_pos_);
    int n1, n2;

    if (CXXPH_LIKELY(dn >= n)) {
        n1 = n;
        n2 = 0;
    } else {
        n1 = dn;
        n2 = (n - n1);
    }

    int n_append;

    if (pass_through_ || sparse_copy_) {
        if (CXXPH_LIKELY(n1 > 0)) {
            ::memcpy(&(delay_[write_pos_]), &(s[0]), (sizeof(src_frame_t) * n1));
        }

        if (CXXPH_UNLIKELY(n2 > 0)) {
            ::memcpy(&(delay_[0]), &(s[n1]), (sizeof(src_frame_t) * n2));
        }
    } else {
        if (CXXPH_LIKELY(n1 > 0)) {
            core_operator_.dual_copy(&(delay_[(0 * delay_line_size_) + write_pos_]),
                                     &(delay_[(1 * delay_line_size_) + write_pos_]), &(s[0]), n1);
        }

        if (CXXPH_UNLIKELY(n2 > 0)) {
            core_operator_.dual_copy(&(delay_[(0 * delay_line_size_) + 0]), &(delay_[(1 * delay_line_size_) + 0]),
                                     &(s[n1]), n2);
        }
    }

    write_pos_ = (write_pos_ + n);
    if (CXXPH_UNLIKELY(write_pos_ >= delay_line_size_)) {
        write_pos_ -= delay_line_size_;
    }
    count_ += (n * m_);
    assert(count_ <= m_delay_line_size_);
}

template <typename TSrcFrame, typename TDestFrame, typename TCoeffs, typename TPolyCoreOperator>
inline void polyphase_resampler<TSrcFrame, TDestFrame, TCoeffs, TPolyCoreOperator>::get_n(dest_frame_t *d, int n)
    CXXPH_NOEXCEPT
{

    assert(n <= num_can_get());

    int rp = read_pos_;
    int n_consumed;

    if (pass_through_ || sparse_copy_) {
        const int dn = (delay_line_size_ - rp + (l_ - 1)) / l_;
        int n1, n2;

        if (CXXPH_LIKELY(dn >= n)) {
            n1 = n;
            n2 = 0;
        } else {
            n1 = dn;
            n2 = (n - n1);
        }

        if (CXXPH_LIKELY(n1 > 0)) {
            utils::stride_pod_copy(reinterpret_cast<typename src_frame_t::pod_type *>(&(d[0])), 1,
                                   reinterpret_cast<const typename src_frame_t::pod_type *>(&(delay_[rp])), l_, n1);

            rp += (n1 * l_);
            if (CXXPH_UNLIKELY(rp >= delay_line_size_)) {
                rp -= delay_line_size_;
            }
        }

        if (CXXPH_UNLIKELY(n2 > 0)) {
            utils::stride_pod_copy(reinterpret_cast<typename src_frame_t::pod_type *>(&(d[n1])), 1,
                                   reinterpret_cast<const typename src_frame_t::pod_type *>(&(delay_[rp])), l_, n2);
            rp += (n2 * l_);
        }
    } else {
        rp = convolve_n(m_, interleaved_coeffs_subtable_size_, d, core_operator_, delay_, interleaved_coeffs_, l_, n,
                        rp);
    }

    if (CXXPH_UNLIKELY(rp >= m_delay_line_size_)) {
        rp -= m_delay_line_size_;
    }
    read_pos_ = rp;
    assert(read_pos_ < m_delay_line_size_);

    count_ -= (n * l_);
}

template <typename TSrcFrame, typename TDestFrame, typename TCoeffs, typename TPolyCoreOperator>
inline int polyphase_resampler<TSrcFrame, TDestFrame, TCoeffs, TPolyCoreOperator>::num_can_put() const CXXPH_NOEXCEPT
{
    if (pass_through_ || sparse_copy_) {
        if (CXXPH_LIKELY(!flushed_)) {
            const int rp = read_pos_;
            const int wp = write_pos_;
            int n;

            if (wp >= rp) {
                n = delay_line_size_ - (wp - rp) - 1;
            } else {
                n = (rp - wp) - 1;
            }

            assert((count_ + n) <= delay_line_size_);

            return n;
        } else {
            return 0;
        }
    } else {
        if (CXXPH_LIKELY(!flushed_)) {
            const int rp = read_pos_ / m_;
            const int wp = write_pos_;
            int n;

            if (wp >= rp) {
                n = delay_line_size_ - (wp - rp) - 1;
            } else {
                n = (rp - wp) - 1;
            }

            assert((count_ + (n * m_)) <= m_delay_line_size_);

            return n;
        } else {
            return 0;
        }
    }
}

template <typename TSrcFrame, typename TDestFrame, typename TCoeffs, typename TPolyCoreOperator>
inline int polyphase_resampler<TSrcFrame, TDestFrame, TCoeffs, TPolyCoreOperator>::num_can_get() const CXXPH_NOEXCEPT
{
    if (pass_through_ || sparse_copy_) {
        return count_ / l_;
    } else {
        if (count_ >= num_coeffs_) {
            return (count_ - num_coeffs_ + 1) / l_;
        } else {
            return 0;
        }
    }
}

} // namespace resampler
} // namespace cxxdasp

#endif // CXXDASP_RESAMPLER_POLYPHASE_RESAMPLER_HPP_
