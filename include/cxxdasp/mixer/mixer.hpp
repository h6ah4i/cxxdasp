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

#ifndef CXXDASP_MIXER_MIXER_HPP_
#define CXXDASP_MIXER_MIXER_HPP_

#include <cxxporthelper/compiler.hpp>
#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace mixer {

/**
 * Mixer
 *
 * @tparam TFrame audio frame type
 * @tparam TTableCoeff
 * @tparam TMixerCoreOperator core operator class
 */
template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
class mixer {

    /// @cond INTERNAL_FIELD
    mixer(const mixer &) = delete;
    mixer &operator=(const mixer &) = delete;
    /// @endcond

public:
    /**
     * Audio frame type.
     */
    typedef TFrame frame_type;

    /**
     * Coefficient table type.
     */
    typedef TTableCoeff table_coeff_type;

    /**
     * Core operator class.
     */
    typedef TMixerCoreOperator core_operator_type;

    /**
     * Constructor.
     */
    mixer();

    /**
     * Destructor.
     */
    ~mixer();

    /**
     * Multiply (scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     */
    void mul_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const frame_type &scale,
                   int n) const CXXPH_NOEXCEPT;

    /**
     * Multiply and Accumulate (scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     */
    void mac_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const frame_type &scale,
                   int n) const CXXPH_NOEXCEPT;

    /**
     * Multiply / Multiply and Accumulate (scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     * @param accumulate [in] false: multiply only, true: multiply accumulate
     */
    void mul_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const frame_type &scale,
                   int n, bool accumulate) const CXXPH_NOEXCEPT;

    /**
     * Multiply (forward table, scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param table [in] scalar coefficient table
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     *
     * @note The table is reffered forward direction. (table[0], table[1], ... table[n-1])
     */
    void mul_forward_table_and_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                     const table_coeff_type *CXXPH_RESTRICT table, const frame_type &scale,
                                     int n) const CXXPH_NOEXCEPT;

    /**
     * Multiply and Accumulate (forward table, scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param table [in] scalar coefficient table
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     *
     * @note The table is reffered forward direction. (table[0], table[1], ... table[n-1])
     */
    void mac_forward_table_and_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                     const table_coeff_type *CXXPH_RESTRICT table, const frame_type &scale,
                                     int n) const CXXPH_NOEXCEPT;

    /**
     * Multiply / Multiply and Accumulate (forward table, scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param table [in] scalar coefficient table
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     * @param accumulate [in] false: multiply only, true: multiply accumulate
     *
     * @note The table is reffered forward direction. (table[0], table[1], ... table[n-1])
     */
    void mul_forward_table_and_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                     const table_coeff_type *CXXPH_RESTRICT table, const frame_type &scale, int n,
                                     bool accumulate) const CXXPH_NOEXCEPT;

    /**
     * Multiply (backward table, scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param table [in] scalar coefficient table
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     *
     * @note The table is reffered backward direction. (table[0], table[-1], ... table[-(n-1)])
     */
    void mul_backward_table_and_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                      const table_coeff_type *CXXPH_RESTRICT table, const frame_type &scale,
                                      int n) const CXXPH_NOEXCEPT;

    /**
     * Multiply and Accumulate (backward table, scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param table [in] scalar coefficient table
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     *
     * @note The table is reffered backward direction. (table[0], table[-1], ... table[-(n-1)])
     */
    void mac_backward_table_and_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                      const table_coeff_type *CXXPH_RESTRICT table, const frame_type &scale,
                                      int n) const CXXPH_NOEXCEPT;

    /**
     * Multiply / Multiply and Accumulate (backward table, scale).
     *
     * @param dest [in] destination buffer
     * @param src [in] source buffer
     * @param table [in] scalar coefficient table
     * @param scale [in] scaling coefficient
     * @param n [in] count [frames]
     * @param accumulate [in] false: multiply only, true: multiply accumulate
     *
     * @note The table is reffered backward direction. (table[0], table[-1], ... table[-(n-1)])
     */
    void mul_backward_table_and_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                      const table_coeff_type *CXXPH_RESTRICT table, const frame_type &scale, int n,
                                      bool accumulate) const CXXPH_NOEXCEPT;

private:
    /// @cond INTERNAL_FIELD
    core_operator_type core_operator_;
    /// @endcond
};

template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
inline mixer<TFrame, TTableCoeff, TMixerCoreOperator>::mixer()
    : core_operator_()
{
}

template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
inline mixer<TFrame, TTableCoeff, TMixerCoreOperator>::~mixer()
{
}

template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
inline void mixer<TFrame, TTableCoeff, TMixerCoreOperator>::mul_scale(frame_type *CXXPH_RESTRICT dest,
                                                                      const frame_type *CXXPH_RESTRICT src,
                                                                      const frame_type &scale, int n) const
    CXXPH_NOEXCEPT
{
    core_operator_.mul_scale(dest, src, scale, n);
}

template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
inline void mixer<TFrame, TTableCoeff, TMixerCoreOperator>::mac_scale(frame_type *CXXPH_RESTRICT dest,
                                                                      const frame_type *CXXPH_RESTRICT src,
                                                                      const frame_type &scale, int n) const
    CXXPH_NOEXCEPT
{
    core_operator_.mac_scale(dest, src, scale, n);
}

template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
inline void mixer<TFrame, TTableCoeff, TMixerCoreOperator>::mul_scale(frame_type *CXXPH_RESTRICT dest,
                                                                      const frame_type *CXXPH_RESTRICT src,
                                                                      const frame_type &scale, int n,
                                                                      bool accumulate) const CXXPH_NOEXCEPT
{
    if (accumulate) {
        mac_scale(dest, src, scale, n);
    } else {
        mul_scale(dest, src, scale, n);
    }
}

template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
inline void mixer<TFrame, TTableCoeff, TMixerCoreOperator>::mul_forward_table_and_scale(
    frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const table_coeff_type *CXXPH_RESTRICT table,
    const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    core_operator_.mul_forward_table_and_scale(dest, src, table, scale, n);
}

template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
inline void mixer<TFrame, TTableCoeff, TMixerCoreOperator>::mac_forward_table_and_scale(
    frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const table_coeff_type *CXXPH_RESTRICT table,
    const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    core_operator_.mac_forward_table_and_scale(dest, src, table, scale, n);
}

template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
inline void mixer<TFrame, TTableCoeff, TMixerCoreOperator>::mul_forward_table_and_scale(
    frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const table_coeff_type *CXXPH_RESTRICT table,
    const frame_type &scale, int n, bool accumulate) const CXXPH_NOEXCEPT
{
    if (accumulate) {
        mac_forward_table_and_scale(dest, src, table, scale, n);
    } else {
        mul_forward_table_and_scale(dest, src, table, scale, n);
    }
}

template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
inline void mixer<TFrame, TTableCoeff, TMixerCoreOperator>::mul_backward_table_and_scale(
    frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const table_coeff_type *CXXPH_RESTRICT table,
    const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    core_operator_.mul_backward_table_and_scale(dest, src, table, scale, n);
}

template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
inline void mixer<TFrame, TTableCoeff, TMixerCoreOperator>::mac_backward_table_and_scale(
    frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const table_coeff_type *CXXPH_RESTRICT table,
    const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    core_operator_.mac_backward_table_and_scale(dest, src, table, scale, n);
}

template <typename TFrame, typename TTableCoeff, class TMixerCoreOperator>
inline void mixer<TFrame, TTableCoeff, TMixerCoreOperator>::mul_backward_table_and_scale(
    frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const table_coeff_type *CXXPH_RESTRICT table,
    const frame_type &scale, int n, bool accumulate) const CXXPH_NOEXCEPT
{
    if (accumulate) {
        mac_backward_table_and_scale(dest, src, table, scale, n);
    } else {
        mul_backward_table_and_scale(dest, src, table, scale, n);
    }
}

} // namespace filter
} // namespace cxxdasp

#endif // CXXDASP_MIXER_MIXER_HPP_
