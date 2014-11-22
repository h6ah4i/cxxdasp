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

#ifndef CXXDASP_CONVERTER_GENERAL_SAMPLE_FORMAT_CONVERTER_CORE_OPERATOR_HPP_
#define CXXDASP_CONVERTER_GENERAL_SAMPLE_FORMAT_CONVERTER_CORE_OPERATOR_HPP_

#include <algorithm>
#include <limits>

#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/type_traits>

#include <cxxdasp/utils/utils.hpp>

namespace cxxdasp {
namespace converter {

/// @cond INTERNAL_FIELD
namespace one_sample_converters {

template <typename T>
struct clamp {
    static T apply(T x) { return utils::clamp(x, static_cast<T>(-1), static_cast<T>(1)); }
};

#define DECL_ONE_SAMPLE_CONVERTER(TSrc, TDest, process)                                                                \
    template <>                                                                                                        \
    struct one_sample_converter<TSrc, TDest> {                                                                         \
        TDest operator()(TSrc x) { return static_cast<TDest>(process); }                                               \
    }

template <typename TSrc, typename TDest>
struct one_sample_converter {
};

DECL_ONE_SAMPLE_CONVERTER(int16_t, int16_t, (x));
DECL_ONE_SAMPLE_CONVERTER(int32_t, int16_t, (x >> 16));
DECL_ONE_SAMPLE_CONVERTER(float, int16_t, (clamp<float>::apply(x) * std::numeric_limits<int16_t>::max()));
DECL_ONE_SAMPLE_CONVERTER(double, int16_t, (clamp<double>::apply(x) * std::numeric_limits<int16_t>::max()));

DECL_ONE_SAMPLE_CONVERTER(int16_t, int32_t, (x << 16));
DECL_ONE_SAMPLE_CONVERTER(int32_t, int32_t, (x));
DECL_ONE_SAMPLE_CONVERTER(float, int32_t,
                          (clamp<double>::apply(static_cast<double>(x)) * std::numeric_limits<int32_t>::max()));
DECL_ONE_SAMPLE_CONVERTER(double, int32_t, (clamp<double>::apply(x) * std::numeric_limits<int32_t>::max()));

DECL_ONE_SAMPLE_CONVERTER(int16_t, float, (clamp<float>::apply(x *(1.0f / std::numeric_limits<int16_t>::max()))));
DECL_ONE_SAMPLE_CONVERTER(int32_t, float, (clamp<double>::apply(x *(1.0 / std::numeric_limits<int32_t>::max()))));
DECL_ONE_SAMPLE_CONVERTER(float, float, (x));
DECL_ONE_SAMPLE_CONVERTER(double, float, (x));

DECL_ONE_SAMPLE_CONVERTER(int16_t, double, (clamp<double>::apply(x *(1.0 / std::numeric_limits<int16_t>::max()))));
DECL_ONE_SAMPLE_CONVERTER(int32_t, double, (clamp<double>::apply(x *(1.0 / std::numeric_limits<int32_t>::max()))));
DECL_ONE_SAMPLE_CONVERTER(float, double, (x));
DECL_ONE_SAMPLE_CONVERTER(double, double, (x));

#undef DECL_ONE_SAMPLE_CONVERTER

} /* one_sample_converter */
/// @endcond

/**
 * General sample format converter.
 * @tparam TSrcFrame source audio frame type
 * @tparam TDestFrame destination audio frame type
 */
template <typename TSrcFrame, typename TDestFrame>
class general_sample_format_converter_core_operator {

    /// @cond INTERNAL_FIELD
    general_sample_format_converter_core_operator(const general_sample_format_converter_core_operator &) = delete;
    general_sample_format_converter_core_operator &
    operator=(const general_sample_format_converter_core_operator &) = delete;
    /// @endcond

public:
    /**
     * Source audio frame type.
     */
    typedef TSrcFrame src_frame_type;

    /**
     * Destination audio frame type.
     */
    typedef TDestFrame dest_frame_type;

    static_assert((src_frame_type::num_channels == dest_frame_type::num_channels) ||
                      (src_frame_type::num_channels == 1),
                  "Channel count constraints are not satisfied");

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    static CXXPH_OPTIONAL_CONSTEXPR bool is_supported() CXXPH_NOEXCEPT { return true; }

    /**
     * Destructor.
     */
    general_sample_format_converter_core_operator() {}

    /**
     * Destructor.
     */
    ~general_sample_format_converter_core_operator() {}

    /**
     * Perform sample format conversion.
     *
     * @param src [in] source buffer
     * @param dest [out] destination buffer
     * @param n [in] count of samples
     */
    void perform(const src_frame_type *CXXPH_RESTRICT src, dest_frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT;
};

template <typename TSrcFrame, typename TDestFrame>
inline void general_sample_format_converter_core_operator<TSrcFrame, TDestFrame>::perform(
    const src_frame_type *CXXPH_RESTRICT src, dest_frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    typedef typename src_frame_type::data_type src_data_type;
    typedef typename dest_frame_type::data_type dest_data_type;
    enum { src_num_channels = src_frame_type::num_channels, dest_num_channels = dest_frame_type::num_channels, };

    one_sample_converters::one_sample_converter<src_data_type, dest_data_type> conv;

    if (src_num_channels == dest_num_channels) {
        for (int i = 0; i < n; ++i) {
            for (int ch = 0; ch < dest_num_channels; ++ch) {
                dest[i].c(ch) = conv(src[i].c(ch));
            }
        }
    } else if (src_num_channels == 1 && dest_num_channels > 1) {
        // monaural -> multi channel
        for (int i = 0; i < n; ++i) {
            const dest_data_type v = conv(src[i].c(0));
            for (int ch = 0; ch < dest_num_channels; ++ch) {
                dest[i].c(ch) = v;
            }
        }
    }
}

} // namespace converter
} // namespace cxxdasp

#endif // CXXDASP_CONVERTER_GENERAL_SAMPLE_FORMAT_CONVERTER_CORE_OPERATOR_HPP_
