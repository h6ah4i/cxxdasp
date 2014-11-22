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

#ifndef CXXDASP_CONVERTER_SAMPLE_FORMAT_CONVERTER_HPP_
#define CXXDASP_CONVERTER_SAMPLE_FORMAT_CONVERTER_HPP_

#include <cxxporthelper/compiler.hpp>

namespace cxxdasp {
namespace converter {

/**
 * Sample format converter
 *
 * @tparam TSrcFrame source audio frame type
 * @tparam TDestFrame destination audio frame type
 * @tparam TCoreOperator core operator class
 */
template <typename TSrcFrame, typename TDestFrame, class TCoreOperator>
class sample_format_converter {

    /// @cond INTERNAL_FIELD
    sample_format_converter(const sample_format_converter &) = delete;
    sample_format_converter &operator=(const sample_format_converter &) = delete;
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

    /**
     * Core operator class.
     */
    typedef TCoreOperator core_operator_type;

    /**
     * Constructor.
     */
    sample_format_converter() {}

    /**
     * Destructor.
     */
    ~sample_format_converter() {}

    /**
     * Perform sample format conversion.
     *
     * @param src [in] source buffer
     * @param dest [out] destination buffer
     * @param n [in] count of samples
     */
    void perform(const src_frame_type *CXXPH_RESTRICT src, dest_frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT;

private:
    /// @cond INTERNAL_FIELD
    core_operator_type core_operator_;
    /// @endcond
};

template <typename TSrcFrame, typename TDestFrame, class TCoreOperator>
inline void sample_format_converter<TSrcFrame, TDestFrame, TCoreOperator>::perform(
    const src_frame_type *CXXPH_RESTRICT src, dest_frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{
    core_operator_.perform(src, dest, n);
}

} // namespace converter
} // namespace cxxdasp

#endif // CXXDASP_CONVERTER_SAMPLE_FORMAT_CONVERTER_HPP_
