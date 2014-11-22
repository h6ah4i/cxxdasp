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

#include <limits>
#include <random>

#include "test_common.hpp"

#include <cxxdasp/mixer/mixer.hpp>
#include <cxxdasp/mixer/mixer_core_operators.hpp>

using namespace cxxdasp;

class MixerTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}
};

template <typename TData, int NChannels>
void make_random_data(std::vector<datatype::audio_frame<TData, NChannels>> &data, int n, int seed = 0)
{
    std::mt19937 mt(seed);
    std::uniform_real_distribution<TData> gen(static_cast<TData>(-1.0), static_cast<TData>(1.0));

    data.resize(n);
    for (int i = 0; i < n; ++i) {
        for (int ch = 0; ch < NChannels; ++ch) {
            data[i].c(ch) = gen(mt);
        }
    }
}

template <typename TData>
void make_random_data(std::vector<TData> &data, int n, int seed = 0)
{
    std::mt19937 mt(seed);
    std::uniform_real_distribution<TData> gen(static_cast<TData>(-1.0), static_cast<TData>(1.0));

    data.resize(n);
    for (int i = 0; i < n; ++i) {
        data[i] = gen(mt);
    }
}

template <typename TAudioFrame, typename TTableCoeff, typename TMixerCoreOperator>
void do_mixer_mul_scale_test(int n)
{
    typedef mixer::mixer<TAudioFrame, TTableCoeff, TMixerCoreOperator> mixer_type;

    const int src_seed = 123;
    const int scale_seed = 456;
    const int dest_seed = 789;

    mixer_type mix;

    std::vector<TAudioFrame> src;
    std::vector<TAudioFrame> scale;
    std::vector<TAudioFrame> excepted;
    std::vector<TAudioFrame> actual;

    // generate source data
    make_random_data(src, n, src_seed);
    make_random_data(scale, 1, scale_seed);

    // fill destination buffer with random value
    make_random_data(actual, n, dest_seed);

    // calculate excepted result
    excepted.resize(n);
    for (int i = 0; i < n; ++i) {
        excepted[i] = src[i] * scale[0];
    }

    // call test method
    mix.mul_scale(&actual[0], &src[0], scale[0], n);

    // check
    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_AUDIO_FRAME_EQ(excepted[i], actual[i]);
    }
}

template <typename TAudioFrame, typename TTableCoeff, typename TMixerCoreOperator>
void do_mixer_mac_scale_test(int n)
{
    typedef mixer::mixer<TAudioFrame, TTableCoeff, TMixerCoreOperator> mixer_type;

    const int src_seed = 123;
    const int scale_seed = 456;
    const int dest_seed = 789;

    mixer_type mix;

    std::vector<TAudioFrame> src;
    std::vector<TAudioFrame> scale;
    std::vector<TAudioFrame> excepted;
    std::vector<TAudioFrame> actual;

    // generate source data
    make_random_data(src, n, src_seed);
    make_random_data(scale, 1, scale_seed);

    // fill destination buffer with random value
    make_random_data(excepted, n, dest_seed);
    make_random_data(actual, n, dest_seed);

    // calculate excepted result
    for (int i = 0; i < n; ++i) {
        excepted[i] += src[i] * scale[0];
    }

    // call test method
    mix.mac_scale(&actual[0], &src[0], scale[0], n);

    // check
    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_AUDIO_FRAME_EQ(excepted[i], actual[i]);
    }
}

template <typename TAudioFrame, typename TTableCoeff, typename TMixerCoreOperator>
void do_mixer_mul_forward_table_and_scale_test(int n)
{
    typedef mixer::mixer<TAudioFrame, TTableCoeff, TMixerCoreOperator> mixer_type;

    const int src_seed = 123;
    const int scale_seed = 456;
    const int dest_seed = 789;
    const int table_seed = 1011;

    mixer_type mix;

    std::vector<TAudioFrame> src;
    std::vector<TTableCoeff> table;
    std::vector<TAudioFrame> scale;
    std::vector<TAudioFrame> excepted;
    std::vector<TAudioFrame> actual;

    // generate source data
    make_random_data(src, n, src_seed);
    make_random_data(scale, 1, scale_seed);

    // generate table
    make_random_data(table, n, table_seed);

    // fill destination buffer with random value
    make_random_data(actual, n, dest_seed);

    // calculate excepted result
    excepted.resize(n);
    for (int i = 0; i < n; ++i) {
        excepted[i] = src[i] * table[i] * scale[0];
    }

    // call test method
    mix.mul_forward_table_and_scale(&actual[0], &src[0], &table[0], scale[0], n);

    // check
    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_AUDIO_FRAME_EQ(excepted[i], actual[i]);
    }
}

template <typename TAudioFrame, typename TTableCoeff, typename TMixerCoreOperator>
void do_mixer_mac_forward_table_and_scale_test(int n)
{
    typedef mixer::mixer<TAudioFrame, TTableCoeff, TMixerCoreOperator> mixer_type;

    const int src_seed = 123;
    const int scale_seed = 456;
    const int dest_seed = 789;
    const int table_seed = 1011;

    mixer_type mix;

    std::vector<TAudioFrame> src;
    std::vector<TTableCoeff> table;
    std::vector<TAudioFrame> scale;
    std::vector<TAudioFrame> excepted;
    std::vector<TAudioFrame> actual;

    // generate source data
    make_random_data(src, n, src_seed);
    make_random_data(scale, 1, scale_seed);

    // generate table
    make_random_data(table, n, table_seed);

    // fill destination buffer with random value
    make_random_data(excepted, n, dest_seed);
    make_random_data(actual, n, dest_seed);

    // calculate excepted result
    excepted.resize(n);
    for (int i = 0; i < n; ++i) {
        excepted[i] += src[i] * table[i] * scale[0];
    }

    // call test method
    mix.mac_forward_table_and_scale(&actual[0], &src[0], &table[0], scale[0], n);

    // check
    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_AUDIO_FRAME_EQ(excepted[i], actual[i]);
    }
}

template <typename TAudioFrame, typename TTableCoeff, typename TMixerCoreOperator>
void do_mixer_mul_backward_table_and_scale_test(int n)
{
    typedef mixer::mixer<TAudioFrame, TTableCoeff, TMixerCoreOperator> mixer_type;

    const int src_seed = 123;
    const int scale_seed = 456;
    const int dest_seed = 789;
    const int table_seed = 1011;

    mixer_type mix;

    std::vector<TAudioFrame> src;
    std::vector<TTableCoeff> table;
    std::vector<TAudioFrame> scale;
    std::vector<TAudioFrame> excepted;
    std::vector<TAudioFrame> actual;

    // generate source data
    make_random_data(src, n, src_seed);
    make_random_data(scale, 1, scale_seed);

    // generate table
    make_random_data(table, n, table_seed);

    // fill destination buffer with random value
    make_random_data(actual, n, dest_seed);

    // calculate excepted result
    excepted.resize(n);
    for (int i = 0; i < n; ++i) {
        excepted[i] = src[i] * table[(n - 1) - i] * scale[0];
    }

    // call test method
    mix.mul_backward_table_and_scale(&actual[0], &src[0], &table[n - 1], scale[0], n);

    // check
    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_AUDIO_FRAME_EQ(excepted[i], actual[i]);
    }
}

template <typename TAudioFrame, typename TTableCoeff, typename TMixerCoreOperator>
void do_mixer_mac_backward_table_and_scale_test(int n)
{
    typedef mixer::mixer<TAudioFrame, TTableCoeff, TMixerCoreOperator> mixer_type;

    const int src_seed = 123;
    const int scale_seed = 456;
    const int dest_seed = 789;
    const int table_seed = 1011;

    mixer_type mix;

    std::vector<TAudioFrame> src;
    std::vector<TTableCoeff> table;
    std::vector<TAudioFrame> scale;
    std::vector<TAudioFrame> excepted;
    std::vector<TAudioFrame> actual;

    // generate source data
    make_random_data(src, n, src_seed);
    make_random_data(scale, 1, scale_seed);

    // generate table
    make_random_data(table, n, table_seed);

    // fill destination buffer with random value
    make_random_data(excepted, n, dest_seed);
    make_random_data(actual, n, dest_seed);

    // calculate excepted result
    excepted.resize(n);
    for (int i = 0; i < n; ++i) {
        excepted[i] += src[i] * table[(n - 1) - i] * scale[0];
    }

    // call test method
    mix.mac_backward_table_and_scale(&actual[0], &src[0], &table[n - 1], scale[0], n);

    // check
    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_AUDIO_FRAME_EQ(excepted[i], actual[i]);
    }
}

template <typename TDataType, int NChannels, typename TTableCoeff>
void do_general_mixer_mul_scale_test(int n)
{
    typedef datatype::audio_frame<TDataType, NChannels> audio_frame_t;
    typedef mixer::general_mixer_core_operator<audio_frame_t, TTableCoeff> core_operator_type;

    do_mixer_mul_scale_test<audio_frame_t, TTableCoeff, core_operator_type>(n);
}

template <typename TDataType, int NChannels, typename TTableCoeff>
void do_general_mixer_mac_scale_test(int n)
{
    typedef datatype::audio_frame<TDataType, NChannels> audio_frame_t;
    typedef mixer::general_mixer_core_operator<audio_frame_t, TTableCoeff> core_operator_type;

    do_mixer_mac_scale_test<audio_frame_t, TTableCoeff, core_operator_type>(n);
}

template <typename TDataType, int NChannels, typename TTableCoeff>
void do_general_mixer_mul_forward_table_and_scale_test(int n)
{
    typedef datatype::audio_frame<TDataType, NChannels> audio_frame_t;
    typedef mixer::general_mixer_core_operator<audio_frame_t, TTableCoeff> core_operator_type;

    do_mixer_mul_forward_table_and_scale_test<audio_frame_t, TTableCoeff, core_operator_type>(n);
}

template <typename TDataType, int NChannels, typename TTableCoeff>
void do_general_mixer_mac_forward_table_and_scale_test(int n)
{
    typedef datatype::audio_frame<TDataType, NChannels> audio_frame_t;
    typedef mixer::general_mixer_core_operator<audio_frame_t, TTableCoeff> core_operator_type;

    do_mixer_mac_forward_table_and_scale_test<audio_frame_t, TTableCoeff, core_operator_type>(n);
}

template <typename TDataType, int NChannels, typename TTableCoeff>
void do_general_mixer_mul_backward_table_and_scale_test(int n)
{
    typedef datatype::audio_frame<TDataType, NChannels> audio_frame_t;
    typedef mixer::general_mixer_core_operator<audio_frame_t, TTableCoeff> core_operator_type;

    do_mixer_mul_backward_table_and_scale_test<audio_frame_t, TTableCoeff, core_operator_type>(n);
}

template <typename TDataType, int NChannels, typename TTableCoeff>
void do_general_mixer_mac_backward_table_and_scale_test(int n)
{
    typedef datatype::audio_frame<TDataType, NChannels> audio_frame_t;
    typedef mixer::general_mixer_core_operator<audio_frame_t, TTableCoeff> core_operator_type;

    do_mixer_mac_backward_table_and_scale_test<audio_frame_t, TTableCoeff, core_operator_type>(n);
}

#if 1
//
// general_mixer_core_operator
//
class GeneralF32MonoMixerTest : public MixerTest {
};

TEST_F(GeneralF32MonoMixerTest, mul_scale) { do_general_mixer_mul_scale_test<float, 1, float>(100); }

TEST_F(GeneralF32MonoMixerTest, mac_scale) { do_general_mixer_mac_scale_test<float, 1, float>(100); }

TEST_F(GeneralF32MonoMixerTest, mul_forward_table_and_scale)
{
    do_general_mixer_mul_forward_table_and_scale_test<float, 1, float>(100);
}

TEST_F(GeneralF32MonoMixerTest, mac_forward_table_and_scale)
{
    do_general_mixer_mac_forward_table_and_scale_test<float, 1, float>(100);
}

TEST_F(GeneralF32MonoMixerTest, mul_backward_table_and_scale)
{
    // do_general_mixer_mul_backward_table_and_scale_test<float, 1, float>(100);
}

TEST_F(GeneralF32MonoMixerTest, mac_backward_table_and_scale)
{
    do_general_mixer_mac_backward_table_and_scale_test<float, 1, float>(100);
}

class GeneralF32StereoMixerTest : public MixerTest {
};

TEST_F(GeneralF32StereoMixerTest, mul_scale) { do_general_mixer_mul_scale_test<float, 2, float>(100); }

TEST_F(GeneralF32StereoMixerTest, mac_scale) { do_general_mixer_mac_scale_test<float, 2, float>(100); }

TEST_F(GeneralF32StereoMixerTest, mul_forward_table_and_scale)
{
    do_general_mixer_mul_forward_table_and_scale_test<float, 2, float>(100);
}

TEST_F(GeneralF32StereoMixerTest, mac_forward_table_and_scale)
{
    do_general_mixer_mac_forward_table_and_scale_test<float, 2, float>(100);
}

TEST_F(GeneralF32StereoMixerTest, mul_backward_table_and_scale)
{
    do_general_mixer_mul_backward_table_and_scale_test<float, 2, float>(100);
}

TEST_F(GeneralF32StereoMixerTest, mac_backward_table_and_scale)
{
    do_general_mixer_mac_backward_table_and_scale_test<float, 2, float>(100);
}
#endif

#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)

class F32MonoSSEMixerTest : public MixerTest {
};

TEST_F(F32MonoSSEMixerTest, mul_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_sse_mixer_core_operator core_operator_type;
    do_mixer_mul_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32MonoSSEMixerTest, mac_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_sse_mixer_core_operator core_operator_type;
    do_mixer_mac_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32MonoSSEMixerTest, mul_forward_table_and_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_sse_mixer_core_operator core_operator_type;
    do_mixer_mul_forward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32MonoSSEMixerTest, mac_forward_table_and_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_sse_mixer_core_operator core_operator_type;
    do_mixer_mac_forward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32MonoSSEMixerTest, mul_backward_table_and_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_sse_mixer_core_operator core_operator_type;
    do_mixer_mul_backward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32MonoSSEMixerTest, mac_backward_table_and_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_sse_mixer_core_operator core_operator_type;
    do_mixer_mac_backward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

class F32StereoSSEMixerTest : public MixerTest {
};

TEST_F(F32StereoSSEMixerTest, mul_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_sse_mixer_core_operator core_operator_type;
    do_mixer_mul_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32StereoSSEMixerTest, mac_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_sse_mixer_core_operator core_operator_type;
    do_mixer_mac_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32StereoSSEMixerTest, mul_forward_table_and_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_sse_mixer_core_operator core_operator_type;
    do_mixer_mul_forward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32StereoSSEMixerTest, mac_forward_table_and_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_sse_mixer_core_operator core_operator_type;
    do_mixer_mac_forward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32StereoSSEMixerTest, mul_backward_table_and_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_sse_mixer_core_operator core_operator_type;
    do_mixer_mul_backward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32StereoSSEMixerTest, mac_backward_table_and_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_sse_mixer_core_operator core_operator_type;
    do_mixer_mac_backward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

class F32MonoNEONMixerTest : public MixerTest {
};

TEST_F(F32MonoNEONMixerTest, mul_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_neon_mixer_core_operator core_operator_type;
    do_mixer_mul_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32MonoNEONMixerTest, mac_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_neon_mixer_core_operator core_operator_type;
    do_mixer_mac_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32MonoNEONMixerTest, mul_forward_table_and_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_neon_mixer_core_operator core_operator_type;
    do_mixer_mul_forward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32MonoNEONMixerTest, mac_forward_table_and_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_neon_mixer_core_operator core_operator_type;
    do_mixer_mac_forward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32MonoNEONMixerTest, mul_backward_table_and_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_neon_mixer_core_operator core_operator_type;
    do_mixer_mul_backward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32MonoNEONMixerTest, mac_backward_table_and_scale)
{
    typedef datatype::audio_frame<float, 1> audio_frame_t;
    typedef mixer::f32_mono_neon_mixer_core_operator core_operator_type;
    do_mixer_mac_backward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

class F32StereoNEONMixerTest : public MixerTest {
};

TEST_F(F32StereoNEONMixerTest, mul_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_neon_mixer_core_operator core_operator_type;
    do_mixer_mul_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32StereoNEONMixerTest, mac_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_neon_mixer_core_operator core_operator_type;
    do_mixer_mac_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32StereoNEONMixerTest, mul_forward_table_and_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_neon_mixer_core_operator core_operator_type;
    do_mixer_mul_forward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32StereoNEONMixerTest, mac_forward_table_and_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_neon_mixer_core_operator core_operator_type;
    do_mixer_mac_forward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32StereoNEONMixerTest, mul_backward_table_and_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_neon_mixer_core_operator core_operator_type;
    do_mixer_mul_backward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}

TEST_F(F32StereoNEONMixerTest, mac_backward_table_and_scale)
{
    typedef datatype::audio_frame<float, 2> audio_frame_t;
    typedef mixer::f32_stereo_neon_mixer_core_operator core_operator_type;
    do_mixer_mac_backward_table_and_scale_test<audio_frame_t, float, core_operator_type>(100);
}
#endif
