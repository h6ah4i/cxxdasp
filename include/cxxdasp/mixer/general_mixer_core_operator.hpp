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

#ifndef CXXDASP_MIXER_GENERAL_MIXER_CORE_OPERATOR_HPP_
#define CXXDASP_MIXER_GENERAL_MIXER_CORE_OPERATOR_HPP_

#include <cxxporthelper/type_traits>
#include <cxxporthelper/compiler.hpp>

#include <cxxdasp/datatype/audio_frame.hpp>

namespace cxxdasp {
namespace mixer {

/**
 * General mixer core operator
 *
 * @tparam TFrame audio frame type
 *
 */
template <typename TFrame, typename TTableCoeff>
class general_mixer_core_operator {

    /// @cond INTERNAL_FIELD
    general_mixer_core_operator(const general_mixer_core_operator &) = delete;
    general_mixer_core_operator &operator=(const general_mixer_core_operator &) = delete;
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
     * Check this operator class is available.
     * @return whether the class is available
     */
    CXXPH_OPTIONAL_CONSTEXPR static bool is_supported() CXXPH_NOEXCEPT { return true; }

    /**
     * Constructor.
     */
    general_mixer_core_operator() {}

    /**
     * Destructor.
     */
    ~general_mixer_core_operator() {}

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

private:
    static_assert(std::is_floating_point<typename frame_type::data_type>::value, "floating point type is required");

    static_assert(std::is_floating_point<table_coeff_type>::value, "floating point type is required");
};

template <typename TFrame, typename TTableCoeff>
inline void general_mixer_core_operator<TFrame, TTableCoeff>::mul_scale(frame_type *CXXPH_RESTRICT dest,
                                                                        const frame_type *CXXPH_RESTRICT src,
                                                                        const frame_type &scale, int n) const
    CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * scale;
    }
}

template <typename TFrame, typename TTableCoeff>
inline void general_mixer_core_operator<TFrame, TTableCoeff>::mac_scale(frame_type *CXXPH_RESTRICT dest,
                                                                        const frame_type *CXXPH_RESTRICT src,
                                                                        const frame_type &scale, int n) const
    CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest[i] += src[i] * scale;
    }
}

template <typename TFrame, typename TTableCoeff>
inline void general_mixer_core_operator<TFrame, TTableCoeff>::mul_forward_table_and_scale(
    frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const table_coeff_type *CXXPH_RESTRICT table,
    const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * (*table) * scale;
        ++table;
    }
}

template <typename TFrame, typename TTableCoeff>
inline void general_mixer_core_operator<TFrame, TTableCoeff>::mac_forward_table_and_scale(
    frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const table_coeff_type *CXXPH_RESTRICT table,
    const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest[i] += src[i] * (*table) * scale;
        ++table;
    }
}

template <typename TFrame, typename TTableCoeff>
inline void general_mixer_core_operator<TFrame, TTableCoeff>::mul_backward_table_and_scale(
    frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const table_coeff_type *CXXPH_RESTRICT table,
    const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * (*table) * scale;
        --table;
    }
}

template <typename TFrame, typename TTableCoeff>
inline void general_mixer_core_operator<TFrame, TTableCoeff>::mac_backward_table_and_scale(
    frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src, const table_coeff_type *CXXPH_RESTRICT table,
    const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        dest[i] += src[i] * (*table) * scale;
        --table;
    }
}

} // namespace filter
} // namespace cxxdasp

#endif // CXXDASP_MIXER_GENERAL_MIXER_CORE_OPERATOR_HPP_
