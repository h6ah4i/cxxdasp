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

#include "test_common.hpp"

#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/resampler/polyphase/polyphase_core_operators.hpp>

using namespace cxxdasp;

#define MAX_TEST_DATA_SIZE 32

template <typename TSample>
class PolyphaseCoreOperatorDualCopyTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef TSample sample_t;

    std::vector<TSample> src_;
    std::vector<TSample> dest1_;
    std::vector<TSample> dest2_;
};

template <typename TCoeffs, typename TSample>
class PolyphaseCoreOperatorConvolveTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef TCoeffs coeffs_t;
    typedef TSample sample_t;

    cxxporthelper::aligned_memory<TCoeffs> coeffs_;
    std::vector<TSample> src_;
    TSample dest_;
};

typedef PolyphaseCoreOperatorDualCopyTest<datatype::f32_mono_frame_t> FloatMonoPolyphaseCoreOperatorDualCopyTest;
typedef PolyphaseCoreOperatorDualCopyTest<datatype::f32_stereo_frame_t> FloatStereoPolyphaseCoreOperatorDualCopyTest;
typedef PolyphaseCoreOperatorDualCopyTest<datatype::f64_mono_frame_t> DoubleMonoPolyphaseCoreOperatorDualCopyTest;
typedef PolyphaseCoreOperatorDualCopyTest<datatype::f64_stereo_frame_t> DoubleStereoPolyphaseCoreOperatorDualCopyTest;

typedef PolyphaseCoreOperatorConvolveTest<float, datatype::f32_mono_frame_t> FloatMonoPolyphaseCoreOperatorConvolveTest;
typedef PolyphaseCoreOperatorConvolveTest<float, datatype::f32_stereo_frame_t>
FloatStereoPolyphaseCoreOperatorConvolveTest;
typedef PolyphaseCoreOperatorConvolveTest<double, datatype::f64_mono_frame_t>
DoubleMonoPolyphaseCoreOperatorConvolveTest;
typedef PolyphaseCoreOperatorConvolveTest<double, datatype::f64_stereo_frame_t>
DoubleStereoPolyphaseCoreOperatorConvolveTest;

template <typename T1, typename T2, typename T3>
T3 convolve(const T1 *a, const T2 *b, int n)
{
    T3 c = 0;
    for (int i = 0; i < n; ++i) {
        c += a[i] * b[i];
    }
    return c;
}

template <class TTest, class TCoreOpearator>
void sub_test_dual_copy(TTest &tst, TCoreOpearator &op, int n)
{
    const int n_channels = TTest::sample_t::num_channels;

    // allocate memory
    tst.src_.resize(n);
    tst.dest1_.resize(n);
    tst.dest2_.resize(n);

    // generate input data
    for (int i = 0; i < n; ++i) {
        for (int ch = 0; ch < n_channels; ++ch) {
            tst.src_[i].c(ch) = static_cast<typename TCoreOpearator::src_frame_t::data_type>(i * (ch + 1));
        }
    }

    // perform
    op.dual_copy(&(tst.dest1_[0]), &(tst.dest2_[0]), &(tst.src_[0]), n);

    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_AUDIO_FRAME_NEAR(tst.src_[i], tst.dest1_[i], static_cast<typename TTest::sample_t::data_type>(0));
        ASSERT_AUTO_AUDIO_FRAME_NEAR(tst.src_[i], tst.dest2_[i], static_cast<typename TTest::sample_t::data_type>(0));
    }
}

template <class TTest, class TCoreOpearator>
void sub_test_convolve(TTest &tst, TCoreOpearator &op, int n)
{
    const int n_channels = TTest::sample_t::num_channels;

    // allocate memory
    tst.coeffs_.allocate(n);
    tst.src_.resize(n);

    // generate input data
    for (int i = 0; i < n; ++i) {
        tst.coeffs_[i] = static_cast<typename TCoreOpearator::coeffs_t>(i);
    }

    for (int i = 0; i < n; ++i) {
        for (int ch = 0; ch < n_channels; ++ch) {
            tst.src_[i].c(ch) = static_cast<typename TCoreOpearator::src_frame_t::data_type>(i * (ch + 1));
        }
    }

    // perform
    op.convolve(&(tst.dest_), &(tst.src_[0]), &(tst.coeffs_[0]), n);

    typename TTest::sample_t actual = tst.dest_;
    typename TTest::sample_t expected =
        convolve<typename TTest::sample_t, typename TTest::coeffs_t, typename TTest::sample_t>(&(tst.src_[0]),
                                                                                               &(tst.coeffs_[0]), n);

    // std::cout << "expected = " << expected << std::endl;
    // std::cout << "actual = " << actual << std::endl;

    ASSERT_AUTO_AUDIO_FRAME_NEAR(expected, actual, static_cast<typename TTest::sample_t::data_type>(1e-5));
}

template <class TTest, class TCoreOpearator>
void do_test_dual_copy(TTest &tst, TCoreOpearator &op, int n)
{
    for (int i = 1; i <= n; ++i) {
        std::cout << "sub_test_dual_copy(n = " << i << ")" << std::endl;
        sub_test_dual_copy(tst, op, i);
    }
}

template <class TTest, class TCoreOpearator>
void do_test_convolve(TTest &tst, TCoreOpearator &op, int n)
{
    for (int i = 1; i <= n; ++i) {
        std::cout << "sub_test_convolve(n = " << i << ")" << std::endl;
        sub_test_convolve(tst, op, i);
    }
}

//
// general_polyphase_core_operator
//
TEST_F(FloatMonoPolyphaseCoreOperatorDualCopyTest, general_polyphase_core_operator)
{
    resampler::general_polyphase_core_operator<float, float, float, 1> op;
    do_test_dual_copy(*this, op, MAX_TEST_DATA_SIZE);
}

TEST_F(FloatStereoPolyphaseCoreOperatorDualCopyTest, general_polyphase_core_operator)
{
    resampler::general_polyphase_core_operator<float, float, float, 2> op;
    do_test_dual_copy(*this, op, MAX_TEST_DATA_SIZE);
}

TEST_F(DoubleMonoPolyphaseCoreOperatorDualCopyTest, general_polyphase_core_operator)
{
    resampler::general_polyphase_core_operator<double, double, double, 1> op;
    do_test_dual_copy(*this, op, MAX_TEST_DATA_SIZE);
}

TEST_F(DoubleStereoPolyphaseCoreOperatorDualCopyTest, general_polyphase_core_operator)
{
    resampler::general_polyphase_core_operator<double, double, double, 2> op;
    do_test_dual_copy(*this, op, MAX_TEST_DATA_SIZE);
}

TEST_F(FloatMonoPolyphaseCoreOperatorConvolveTest, general_polyphase_core_operator)
{
    resampler::general_polyphase_core_operator<float, float, float, 1> op;
    do_test_convolve(*this, op, MAX_TEST_DATA_SIZE);
}

TEST_F(FloatStereoPolyphaseCoreOperatorConvolveTest, general_polyphase_core_operator)
{
    resampler::general_polyphase_core_operator<float, float, float, 2> op;
    do_test_convolve(*this, op, MAX_TEST_DATA_SIZE);
}

TEST_F(DoubleMonoPolyphaseCoreOperatorConvolveTest, general_polyphase_core_operator)
{
    resampler::general_polyphase_core_operator<double, double, double, 1> op;
    do_test_convolve(*this, op, MAX_TEST_DATA_SIZE);
}

TEST_F(DoubleStereoPolyphaseCoreOperatorConvolveTest, general_polyphase_core_operator)
{
    resampler::general_polyphase_core_operator<double, double, double, 2> op;
    do_test_convolve(*this, op, MAX_TEST_DATA_SIZE);
}

//
// SSE optimized core operators
//
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
TEST_F(FloatMonoPolyphaseCoreOperatorDualCopyTest, f32_mono_sse_polyphase_core_operator)
{

    if (!resampler::f32_mono_sse_polyphase_core_operator::is_supported()) {
        std::cout << "SKIPPED: f32_mono_sse_polyphase_core_operator is not supported" << std::endl;
        return;
    }

    resampler::f32_mono_sse_polyphase_core_operator op;
    do_test_dual_copy(*this, op, MAX_TEST_DATA_SIZE);
}

#if CXXPH_COMPILER_SUPPORTS_X86_SSE3
TEST_F(FloatMonoPolyphaseCoreOperatorDualCopyTest, f32_mono_sse3_polyphase_core_operator)
{

    if (!resampler::f32_mono_sse3_polyphase_core_operator::is_supported()) {
        std::cout << "SKIPPED: f32_mono_sse3_polyphase_core_operator is not supported" << std::endl;
        return;
    }

    resampler::f32_mono_sse3_polyphase_core_operator op;
    do_test_dual_copy(*this, op, MAX_TEST_DATA_SIZE);
}
#endif

TEST_F(FloatStereoPolyphaseCoreOperatorDualCopyTest, f32_stereo_sse_polyphase_core_operator)
{

    if (!resampler::f32_stereo_sse_polyphase_core_operator::is_supported()) {
        std::cout << "SKIPPED: f32_stereo_sse_polyphase_core_operator is not supported" << std::endl;
        return;
    }

    resampler::f32_stereo_sse_polyphase_core_operator op;
    do_test_dual_copy(*this, op, MAX_TEST_DATA_SIZE);
}

TEST_F(FloatMonoPolyphaseCoreOperatorConvolveTest, f32_mono_sse_polyphase_core_operator)
{

    if (!resampler::f32_mono_sse_polyphase_core_operator::is_supported()) {
        std::cout << "SKIPPED: f32_mono_sse_polyphase_core_operator is not supported" << std::endl;
        return;
    }

    resampler::f32_mono_sse_polyphase_core_operator op;
    do_test_convolve(*this, op, MAX_TEST_DATA_SIZE);
}

#if CXXPH_COMPILER_SUPPORTS_X86_SSE3
TEST_F(FloatMonoPolyphaseCoreOperatorConvolveTest, f32_mono_sse3_polyphase_core_operator)
{

    if (!resampler::f32_mono_sse3_polyphase_core_operator::is_supported()) {
        std::cout << "SKIPPED: f32_mono_sse3_polyphase_core_operator is not supported" << std::endl;
        return;
    }

    resampler::f32_mono_sse3_polyphase_core_operator op;
    do_test_convolve(*this, op, MAX_TEST_DATA_SIZE);
}
#endif

TEST_F(FloatStereoPolyphaseCoreOperatorConvolveTest, f32_stereo_sse_polyphase_core_operator)
{

    if (!resampler::f32_stereo_sse_polyphase_core_operator::is_supported()) {
        std::cout << "SKIPPED: f32_stereo_sse_polyphase_core_operator is not supported" << std::endl;
        return;
    }

    resampler::f32_stereo_sse_polyphase_core_operator op;
    do_test_convolve(*this, op, MAX_TEST_DATA_SIZE);
}
#endif

//
// NEON optimized core operators
//
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON
TEST_F(FloatMonoPolyphaseCoreOperatorDualCopyTest, f32_mono_neon_polyphase_core_operator)
{

    if (!resampler::f32_mono_neon_polyphase_core_operator::is_supported()) {
        std::cout << "SKIPPED: f32_mono_neon_polyphase_core_operator is not supported" << std::endl;
        return;
    }

    resampler::f32_mono_neon_polyphase_core_operator op;
    do_test_dual_copy(*this, op, MAX_TEST_DATA_SIZE);
}

TEST_F(FloatStereoPolyphaseCoreOperatorDualCopyTest, f32_stereo_neon_polyphase_core_operator)
{

    if (!resampler::f32_stereo_neon_polyphase_core_operator::is_supported()) {
        std::cout << "SKIPPED: f32_stereo_neon_polyphase_core_operator is not supported" << std::endl;
        return;
    }

    resampler::f32_stereo_neon_polyphase_core_operator op;
    do_test_dual_copy(*this, op, MAX_TEST_DATA_SIZE);
}

TEST_F(FloatMonoPolyphaseCoreOperatorConvolveTest, f32_mono_neon_polyphase_core_operator)
{

    if (!resampler::f32_mono_neon_polyphase_core_operator::is_supported()) {
        std::cout << "SKIPPED: f32_mono_neon_polyphase_core_operator is not supported" << std::endl;
        return;
    }

    resampler::f32_mono_neon_polyphase_core_operator op;
    do_test_convolve(*this, op, MAX_TEST_DATA_SIZE);
}

TEST_F(FloatStereoPolyphaseCoreOperatorConvolveTest, f32_stereo_neon_polyphase_core_operator)
{

    if (!resampler::f32_stereo_neon_polyphase_core_operator::is_supported()) {
        std::cout << "SKIPPED: f32_stereo_neon_polyphase_core_operator is not supported" << std::endl;
        return;
    }

    resampler::f32_stereo_neon_polyphase_core_operator op;
    do_test_convolve(*this, op, MAX_TEST_DATA_SIZE);
}
#endif
