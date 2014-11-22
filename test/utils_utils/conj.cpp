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

using namespace cxxdasp;

template <typename T>
class CommonSrcDestConjTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef T value_type;
    cxxporthelper::aligned_memory<std::complex<T>> in_out_;
};

template <typename T>
class SeparateSrcDestConjTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef T value_type;
    cxxporthelper::aligned_memory<std::complex<T>> in_;
    cxxporthelper::aligned_memory<std::complex<T>> out_;
};

template <typename TCommonSrcDestConjTest>
void do_common_src_dest_conj_test(TCommonSrcDestConjTest *thiz, int n)
{
    typedef typename TCommonSrcDestConjTest::value_type value_type;
    typedef std::complex<value_type> complex_type;

    std::vector<complex_type> in_data;
    std::vector<complex_type> expected;

    for (int i = 0; i < n; ++i) {
        in_data.push_back(complex_type(static_cast<value_type>(i), static_cast<value_type>(i * 0.3)));
    }

    for (int i = 0; i < n; ++i) {
        expected.push_back(std::conj(in_data[i]));
    }

    thiz->in_out_.allocate(n);
    ::memcpy(&(thiz->in_out_[0]), &in_data[0], sizeof(complex_type) * n);
    utils::conj(&(thiz->in_out_[0]), n);

    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_COMPLEX_EQ(expected[i], thiz->in_out_[i]);
    }
}

template <typename TCommonSrcDestConjTest>
void do_common_src_dest_conj_aligned_test(TCommonSrcDestConjTest *thiz, int n)
{
    typedef typename TCommonSrcDestConjTest::value_type value_type;
    typedef std::complex<value_type> complex_type;

    std::vector<complex_type> in_data;
    std::vector<complex_type> expected;

    for (int i = 0; i < n; ++i) {
        in_data.push_back(complex_type(static_cast<value_type>(i), static_cast<value_type>(i * 0.3)));
    }

    for (int i = 0; i < n; ++i) {
        expected.push_back(std::conj(in_data[i]));
    }

    thiz->in_out_.allocate(n);
    ::memcpy(&(thiz->in_out_[0]), &in_data[0], sizeof(complex_type) * n);
    utils::conj_aligned(&(thiz->in_out_[0]), n);

    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_COMPLEX_EQ(expected[i], thiz->in_out_[i]);
    }
}

template <typename TSeparateSrcDestConjTest>
void do_separate_src_dest_conj_test(TSeparateSrcDestConjTest *thiz, int n)
{
    typedef typename TSeparateSrcDestConjTest::value_type value_type;
    typedef std::complex<value_type> complex_type;

    std::vector<complex_type> in_data;
    std::vector<complex_type> expected;

    for (int i = 0; i < n; ++i) {
        in_data.push_back(complex_type(static_cast<value_type>(i), static_cast<value_type>(i * 0.3)));
    }

    for (int i = 0; i < n; ++i) {
        expected.push_back(std::conj(in_data[i]));
    }

    thiz->in_.allocate(n);
    thiz->out_.allocate(n);
    ::memcpy(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n);
    utils::conj(&(thiz->out_[0]), &(thiz->in_[0]), n);

    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_COMPLEX_EQ(expected[i], thiz->out_[i]);
    }
}

template <typename TSeparateSrcDestConjTest>
void do_separate_src_dest_conj_aligned_test(TSeparateSrcDestConjTest *thiz, int n)
{
    typedef typename TSeparateSrcDestConjTest::value_type value_type;
    typedef std::complex<value_type> complex_type;

    std::vector<complex_type> in_data;
    std::vector<complex_type> expected;

    for (int i = 0; i < n; ++i) {
        in_data.push_back(complex_type(static_cast<value_type>(i), static_cast<value_type>(i * 0.3)));
    }

    for (int i = 0; i < n; ++i) {
        expected.push_back(std::conj(in_data[i]));
    }

    thiz->in_.allocate(n);
    thiz->out_.allocate(n);
    ::memcpy(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n);
    utils::conj_aligned(&(thiz->out_[0]), &(thiz->in_[0]), n);

    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_COMPLEX_EQ(expected[i], thiz->out_[i]);
    }
}

//
// utils::conj(float *src_dest, int n)
//
typedef CommonSrcDestConjTest<float> CommonSrcDestConjTest_Float;
TEST_F(CommonSrcDestConjTest_Float, conj) { do_common_src_dest_conj_test(this, 5000); }

//
// utils::conj(double *src_dest, int n)
//
typedef CommonSrcDestConjTest<double> CommonSrcDestConjTest_Double;
TEST_F(CommonSrcDestConjTest_Double, conj) { do_common_src_dest_conj_test(this, 5000); }

//
// utils::conj_aligned(float *src_dest, int n)
//
// typedef CommonSrcDestConjTest<float> CommonSrcDestConjTest_Float;
TEST_F(CommonSrcDestConjTest_Float, conj_aligned) { do_common_src_dest_conj_aligned_test(this, 5000); }

//
// utils::conj_aligned(double *src_dest, int n)
//
// typedef CommonSrcDestConjTest<double> CommonSrcDestConjTest_Double;
TEST_F(CommonSrcDestConjTest_Double, conj_aligned) { do_common_src_dest_conj_aligned_test(this, 5000); }

//
// utils::conj(float *dest, float *src, int n)
//
typedef SeparateSrcDestConjTest<float> SeparateSrcDestConjTest_Float;
TEST_F(SeparateSrcDestConjTest_Float, conj) { do_separate_src_dest_conj_test(this, 5000); }

//
// utils::conj(double *dest, double *src, int n)
//
typedef SeparateSrcDestConjTest<double> SeparateSrcDestConjTest_Double;
TEST_F(SeparateSrcDestConjTest_Double, conj) { do_separate_src_dest_conj_test(this, 5000); }

//
// utils::conj_aligned(float *dest, float *src, int n)
//
// typedef SeparateSrcDestConjTest<float> SeparateSrcDestConjTest_Float;
TEST_F(SeparateSrcDestConjTest_Float, conj_aligned) { do_separate_src_dest_conj_aligned_test(this, 5000); }

//
// utils::conj_aligned(double *dest, double *src, int n)
//
// typedef SeparateSrcDestConjTest<double> SeparateSrcDestConjTest_Double;
TEST_F(SeparateSrcDestConjTest_Double, conj_aligned) { do_separate_src_dest_conj_aligned_test(this, 5000); }
