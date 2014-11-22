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

#include <random>

#include "test_common.hpp"

#include <cxxdasp/utils/utils.hpp>
#include <cxxdasp/datatype/audio_frame.hpp>
#include <cxxdasp/filter/tsvf/tsvf.hpp>
#include <cxxdasp/filter/tsvf/tsvf_core_operators.hpp>
#include <cxxdasp/filter/cascaded_tsvf/cascaded_tsvf.hpp>
#include <cxxdasp/filter/cascaded_tsvf/cascaded_tsvf_core_operators.hpp>

using namespace cxxdasp;

class CascadedTrapezoidalStateVariableFilterTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}
};

filter::filter_params_t make_lpf_params()
{
    filter::filter_params_t params;

    params.type = filter::types::LowPass;
    params.fs = 44100.0;
    params.f0 = 4000.0;
    params.db_gain = 0.0; // Not used
    params.q = 0.7071;

    return params;
}

filter::filter_params_t make_hpf_params()
{
    filter::filter_params_t params;

    params.type = filter::types::HighPass;
    params.fs = 44100.0;
    params.f0 = 1000.0;
    params.db_gain = 0.0; // Not used
    params.q = 0.7071;

    return params;
}

template <typename TFrame>
void reference_filter_impl(const filter::filter_params_t fparams, const std::vector<TFrame> &src,
                           std::vector<TFrame> &dest, int n)
{
    typedef TFrame frame_type;
    typedef typename frame_type::data_type data_type;

    filter::trapezoidal_state_variable_filter_coeffs bcoeffs;

    ASSERT_TRUE(bcoeffs.make(fparams));

    const data_type a1 = static_cast<data_type>(bcoeffs.a1);
    const data_type a2 = static_cast<data_type>(bcoeffs.a2);
    const data_type a3 = static_cast<data_type>(bcoeffs.a3);
    const data_type m0 = static_cast<data_type>(bcoeffs.m0);
    const data_type m1 = static_cast<data_type>(bcoeffs.m1);
    const data_type m2 = static_cast<data_type>(bcoeffs.m2);

    frame_type ic1eq, ic2eq;

    ic1eq = ic2eq = 0;

    for (int i = 0; i < n; ++i) {
        const frame_type v0 = src[i];
        const frame_type v3 = v0 - ic2eq;
        const frame_type v1 = a1 * ic1eq + a2 * v3;
        const frame_type v2 = ic2eq + a2 * ic1eq + a3 * v3;
        ic1eq = 2 * v1 - ic1eq;
        ic2eq = 2 * v2 - ic2eq;
        dest[i] = m0 * v0 + m1 * v1 + m2 * v2;
    }
}

template <typename TFrame>
void make_random_data(std::vector<TFrame> &data, int n)
{
    typedef typename TFrame::data_type data_type;
    std::mt19937 mt(0);
    std::uniform_real_distribution<data_type> gen(static_cast<data_type>(-1), static_cast<data_type>(+1));

    data.resize(n);
    for (int i = 0; i < n; ++i) {
        for (int ch = 0; ch < TFrame::num_channels; ++ch) {
            data[i].c(ch) = gen(mt);
        }
    }
}

template <typename TFilter>
void do_oneshot_overwrite_test(int n)
{
    typedef TFilter filter_type;
    typedef typename filter_type::frame_type frame_type;
    std::vector<frame_type> in_data(n);
    std::vector<frame_type> in_out_data(n);
    std::vector<frame_type> tmp_data(n);
    std::vector<frame_type> expected_data(n);
    filter::filter_params_t fparams[2];
    filter_type flt;

    fparams[0] = make_lpf_params();
    fparams[1] = make_hpf_params();

    // make input data
    make_random_data(in_data, n);

    // calculate expected output
    reference_filter_impl(fparams[0], in_data, tmp_data, n);
    reference_filter_impl(fparams[1], tmp_data, expected_data, n);

    // initializ filter
    ASSERT_TRUE(flt.init_all(fparams));

    // perform
    in_out_data.resize(n);
    ::memcpy(&in_out_data[0], &in_data[0], sizeof(frame_type) * n);
    flt.perform(&in_out_data[0], n);

    // check
    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_AUDIO_FRAME_NEAR(expected_data[i], in_out_data[i],
                                     static_cast<typename frame_type::data_type>(1e-5));
    }
}

template <typename TFilter>
void do_oneshot_non_overwrite_test(int n)
{
    typedef TFilter filter_type;
    typedef typename filter_type::frame_type frame_type;
    std::vector<frame_type> in_data(n);
    std::vector<frame_type> out_data(n);
    std::vector<frame_type> tmp_data(n);
    std::vector<frame_type> expected_data(n);
    filter::filter_params_t fparams[2];
    filter_type flt;

    fparams[0] = make_lpf_params();
    fparams[1] = make_hpf_params();

    // make input data
    make_random_data(in_data, n);

    // calculate expected output
    reference_filter_impl(fparams[0], in_data, tmp_data, n);
    reference_filter_impl(fparams[1], tmp_data, expected_data, n);

    // initializ filter
    ASSERT_TRUE(flt.init_all(fparams));

    // perform
    out_data.resize(n);
    flt.perform(&in_data[0], &out_data[0], n);

    // check
    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_AUDIO_FRAME_NEAR(expected_data[i], out_data[i], static_cast<typename frame_type::data_type>(1e-5));
    }
}

template <typename TFilter>
void do_random_size_overwrite_test(int n, int max_size)
{
    typedef TFilter filter_type;
    typedef typename filter_type::frame_type frame_type;
    std::vector<frame_type> in_data(n);
    std::vector<frame_type> in_out_data(n);
    std::vector<frame_type> tmp_data(n);
    std::vector<frame_type> expected_data(n);
    filter::filter_params_t fparams[2];
    filter_type flt;

    fparams[0] = make_lpf_params();
    fparams[1] = make_hpf_params();

    // make input data
    make_random_data(in_data, n);

    // calculate expected output
    reference_filter_impl(fparams[0], in_data, tmp_data, n);
    reference_filter_impl(fparams[1], tmp_data, expected_data, n);

    // initializ filter
    ASSERT_TRUE(flt.init_all(fparams));

    // perform
    in_out_data.resize(n);
    ::memcpy(&in_out_data[0], &in_data[0], sizeof(frame_type) * n);

    std::mt19937 mt(0);
    std::uniform_int_distribution<int> gen(1, max_size);
    int offset = 0;
    while (offset < n) {
        const int s = (std::min)(gen(mt), (n - offset));
        flt.perform(&in_out_data[offset], s);

        // std::cout << "---" << std::endl;
        // for (int i = 0; i < s; ++i) {
        //     std::cout << (offset + i) << " : " << expected_data[offset + i] << " - " << in_out_data[offset + i] <<
        // std::endl;
        // }

        offset += s;
    }

    // check
    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_AUDIO_FRAME_NEAR(expected_data[i], in_out_data[i],
                                     static_cast<typename frame_type::data_type>(1e-5));
    }
}

template <typename TFilter>
void do_random_size_non_overwrite_test(int n, int max_size)
{
    typedef TFilter filter_type;
    typedef typename filter_type::frame_type frame_type;
    std::vector<frame_type> in_data(n);
    std::vector<frame_type> out_data(n);
    std::vector<frame_type> tmp_data(n);
    std::vector<frame_type> expected_data(n);
    filter::filter_params_t fparams[2];
    filter_type flt;

    fparams[0] = make_lpf_params();
    fparams[1] = make_hpf_params();

    // make input data
    make_random_data(in_data, n);

    // calculate expected output
    reference_filter_impl(fparams[0], in_data, tmp_data, n);
    reference_filter_impl(fparams[1], tmp_data, expected_data, n);

    // initializ filter
    ASSERT_TRUE(flt.init_all(fparams));

    // perform
    out_data.resize(n);

    std::mt19937 mt(0);
    std::uniform_int_distribution<int> gen(1, max_size);
    int offset = 0;
    while (offset < n) {
        const int s = (std::min)(gen(mt), (n - offset));
        flt.perform(&in_data[offset], &out_data[offset], s);

        // std::cout << "---" << std::endl;
        // for (int i = 0; i < s; ++i) {
        //     std::cout << (offset + i) << " : " << expected_data[offset + i] << " - " << out_data[offset + i] <<
        // std::endl;
        // }

        offset += s;
    }

    // check
    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_AUDIO_FRAME_NEAR(expected_data[i], out_data[i], static_cast<typename frame_type::data_type>(1e-5));
    }
}

//
// general_cascaded_tsvf_core_operator<general_tsvf_core_operator<f32_mono_frame_t>, 2>
//
class GeneralF32MonoCoreOperatorTest : public CascadedTrapezoidalStateVariableFilterTest {
};

TEST_F(GeneralF32MonoCoreOperatorTest, oneshot_overwrite)
{
    typedef datatype::f32_mono_frame_t frame_t;
    typedef filter::general_tsvf_core_operator<frame_t> single_core_operator_t;
    typedef filter::general_cascaded_tsvf_core_operator<single_core_operator_t, 2> cascaded_core_operator_t;
    typedef filter::cascaded_trapezoidal_state_variable_filter<frame_t, cascaded_core_operator_t> filter_t;

    do_oneshot_overwrite_test<filter_t>(100);
}

TEST_F(GeneralF32MonoCoreOperatorTest, oneshot_non_overwrite)
{
    typedef datatype::f32_mono_frame_t frame_t;
    typedef filter::general_tsvf_core_operator<frame_t> single_core_operator_t;
    typedef filter::general_cascaded_tsvf_core_operator<single_core_operator_t, 2> cascaded_core_operator_t;
    typedef filter::cascaded_trapezoidal_state_variable_filter<frame_t, cascaded_core_operator_t> filter_t;

    do_oneshot_non_overwrite_test<filter_t>(100);
}

TEST_F(GeneralF32MonoCoreOperatorTest, random_size_overwrite)
{
    typedef datatype::f32_mono_frame_t frame_t;
    typedef filter::general_tsvf_core_operator<frame_t> single_core_operator_t;
    typedef filter::general_cascaded_tsvf_core_operator<single_core_operator_t, 2> cascaded_core_operator_t;
    typedef filter::cascaded_trapezoidal_state_variable_filter<frame_t, cascaded_core_operator_t> filter_t;

    do_random_size_overwrite_test<filter_t>(100, 10);
}

TEST_F(GeneralF32MonoCoreOperatorTest, random_size_non_overwrite)
{
    typedef datatype::f32_mono_frame_t frame_t;
    typedef filter::general_tsvf_core_operator<frame_t> single_core_operator_t;
    typedef filter::general_cascaded_tsvf_core_operator<single_core_operator_t, 2> cascaded_core_operator_t;
    typedef filter::cascaded_trapezoidal_state_variable_filter<frame_t, cascaded_core_operator_t> filter_t;

    do_random_size_non_overwrite_test<filter_t>(100, 10);
}

//
// general_tsvf_core_operator<f64_mono_frame_t>
//
class GeneralF64MonoCoreOperatorTest : public CascadedTrapezoidalStateVariableFilterTest {
};

TEST_F(GeneralF64MonoCoreOperatorTest, oneshot_overwrite)
{
    typedef datatype::f64_mono_frame_t frame_t;
    typedef filter::general_tsvf_core_operator<frame_t> single_core_operator_t;
    typedef filter::general_cascaded_tsvf_core_operator<single_core_operator_t, 2> cascaded_core_operator_t;
    typedef filter::cascaded_trapezoidal_state_variable_filter<frame_t, cascaded_core_operator_t> filter_t;

    do_oneshot_overwrite_test<filter_t>(100);
}

TEST_F(GeneralF64MonoCoreOperatorTest, oneshot_non_overwrite)
{
    typedef datatype::f64_mono_frame_t frame_t;
    typedef filter::general_tsvf_core_operator<frame_t> single_core_operator_t;
    typedef filter::general_cascaded_tsvf_core_operator<single_core_operator_t, 2> cascaded_core_operator_t;
    typedef filter::cascaded_trapezoidal_state_variable_filter<frame_t, cascaded_core_operator_t> filter_t;

    do_oneshot_non_overwrite_test<filter_t>(100);
}

TEST_F(GeneralF64MonoCoreOperatorTest, random_size_overwrite)
{
    typedef datatype::f64_mono_frame_t frame_t;
    typedef filter::general_tsvf_core_operator<frame_t> single_core_operator_t;
    typedef filter::general_cascaded_tsvf_core_operator<single_core_operator_t, 2> cascaded_core_operator_t;
    typedef filter::cascaded_trapezoidal_state_variable_filter<frame_t, cascaded_core_operator_t> filter_t;

    do_random_size_overwrite_test<filter_t>(100, 10);
}

TEST_F(GeneralF64MonoCoreOperatorTest, random_size_non_overwrite)
{
    typedef datatype::f64_mono_frame_t frame_t;
    typedef filter::general_tsvf_core_operator<frame_t> single_core_operator_t;
    typedef filter::general_cascaded_tsvf_core_operator<single_core_operator_t, 2> cascaded_core_operator_t;
    typedef filter::cascaded_trapezoidal_state_variable_filter<frame_t, cascaded_core_operator_t> filter_t;

    do_random_size_non_overwrite_test<filter_t>(100, 10);
}
