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

#ifndef CXXDASP_FILTER_CASCADED_TSVF_GENERAL_CASCADED_TSVF_CORE_OPERATOR_HPP_
#define CXXDASP_FILTER_CASCADED_TSVF_GENERAL_CASCADED_TSVF_CORE_OPERATOR_HPP_

namespace cxxdasp {
namespace filter {

/**
 * General cascaded transposed state variable filter core operator
 *
 * @tparam TSingleTSVFCoreOperator transposed state variable filter core operator
 * @tparam NCascaded number of cascded
 */
template <typename TSingleTSVFCoreOperator, int NCascaded>
class general_cascaded_tsvf_core_operator {

    /// @cond INTERNAL_FIELD
    general_cascaded_tsvf_core_operator(const general_cascaded_tsvf_core_operator &) = delete;
    general_cascaded_tsvf_core_operator &operator=(const general_cascaded_tsvf_core_operator &) = delete;
    /// @endcond

public:
    /**
     * Audio frame type.
     */
    typedef typename TSingleTSVFCoreOperator::frame_type frame_type;

    /**
     * Value type.
     */
    typedef typename frame_type::data_type value_type;

/**
     * Number of cascaded filters.
     */
#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
    static constexpr int num_cascaded = NCascaded;
#else
    enum { num_cascaded = NCascaded };
#endif

    /**
     * Check this operator class is available.
     * @return whether the class is available
     */
    CXXPH_OPTIONAL_CONSTEXPR static bool is_supported() CXXPH_NOEXCEPT { return true; }

    /**
     * Constructor.
     */
    general_cascaded_tsvf_core_operator();

    /**
     * Destructor.
     */
    ~general_cascaded_tsvf_core_operator();

    /**
     * Set parameters.
     * @param filter_no [in] filter no.
     * @param a1 [in] a1 filter parameter
     * @param a2 [in] a2 filter parameter
     * @param a3 [in] a3 filter parameter
     * @param m0 [in] m0 filter parameter
     * @param m1 [in] m1 filter parameter
     * @param m2 [in] m2 filter parameter
     */
    void set_params(int filter_no, double a1, double a2, double a3, double m0, double m1, double m2) CXXPH_NOEXCEPT;

    /**
     * Reset state.
     * @param filter_no [in] finter no.
     */
    void reset(int filter_no) CXXPH_NOEXCEPT;

    /**
     * Perform filtering.
     * @param src_dest [in/out] data buffer
     * @param n [in] count of samples
     */
    void perform(frame_type *src_dest, int n) CXXPH_NOEXCEPT;

    /**
     * Perform filtering.
     * @param src [in] source data buffer
     * @param dest [out] destination data buffer
     * @param n [in] count of samples
     */
    void perform(const frame_type *CXXPH_RESTRICT src, frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT;

private:
    static_assert((num_cascaded >= 1), "NCascaded must be grater than or equals to 1");

    /// @cond INTERNAL_FIELD
    TSingleTSVFCoreOperator sub_filter_core_operators_[NCascaded];
    /// @endcond
};

template <typename TSingleTSVFCoreOperator, int NCascaded>
inline general_cascaded_tsvf_core_operator<TSingleTSVFCoreOperator, NCascaded>::general_cascaded_tsvf_core_operator()
{
    for (auto &f : sub_filter_core_operators_) {
        f.reset();
    }
}

template <typename TSingleTSVFCoreOperator, int NCascaded>
inline general_cascaded_tsvf_core_operator<TSingleTSVFCoreOperator, NCascaded>::~general_cascaded_tsvf_core_operator()
{
}

template <typename TSingleTSVFCoreOperator, int NCascaded>
inline void general_cascaded_tsvf_core_operator<TSingleTSVFCoreOperator, NCascaded>::set_params(
    int filter_no, double a1, double a2, double a3, double m0, double m1, double m2) CXXPH_NOEXCEPT
{
    sub_filter_core_operators_[filter_no].set_params(a1, a2, a3, m0, m1, m2);
}

template <typename TSingleTSVFCoreOperator, int NCascaded>
inline void general_cascaded_tsvf_core_operator<TSingleTSVFCoreOperator, NCascaded>::reset(int filter_no) CXXPH_NOEXCEPT
{
    sub_filter_core_operators_[filter_no].reset();
}

template <typename TSingleTSVFCoreOperator, int NCascaded>
inline void general_cascaded_tsvf_core_operator<TSingleTSVFCoreOperator, NCascaded>::perform(frame_type *src_dest,
                                                                                             int n) CXXPH_NOEXCEPT
{

    for (auto &f : sub_filter_core_operators_) {
        f.perform(src_dest, n);
    }
}

template <typename TSingleTSVFCoreOperator, int NCascaded>
inline void general_cascaded_tsvf_core_operator<TSingleTSVFCoreOperator, NCascaded>::perform(
    const frame_type *CXXPH_RESTRICT src, frame_type *CXXPH_RESTRICT dest, int n) CXXPH_NOEXCEPT
{

    sub_filter_core_operators_[0].perform(src, dest, n);
    for (int i = 1; i < num_cascaded; ++i) {
        sub_filter_core_operators_[i].perform(dest, n);
    }
}

} // namespace filter
} // namespace cxxdasp

#endif // CXXDASP_FILTER_CASCADED_TSVF_GENERAL_CASCADED_TSVF_CORE_OPERATOR_HPP_
