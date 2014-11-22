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
class MirrorConjTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef T value_type;
    cxxporthelper::aligned_memory<std::complex<T>> data_;
};

template <typename TMirrorConjTest>
void do_mirror_conj_test(TMirrorConjTest *thiz, int n)
{
    typedef typename TMirrorConjTest::value_type value_type;
    typedef std::complex<value_type> complex_type;

    std::vector<complex_type> expected;

    for (int i = 0; i < (n / 2) + 1; ++i) {
        expected.push_back(complex_type(static_cast<value_type>(i), static_cast<value_type>(i * 0.3)));
    }
    for (int i = 0; i < (n / 2); ++i) {
        expected.push_back(std::conj(expected[(n / 2) - 1 - i]));
    }

    thiz->data_.allocate(n);
    ::memcpy(&(thiz->data_[0]), &expected[0], sizeof(complex_type) * ((n / 2) + 1));
    utils::mirror_conj(&(thiz->data_[n / 2]), (n / 2));

    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_COMPLEX_EQ(expected[i], thiz->data_[i]);
    }
}

template <typename TMirrorConjTest>
void do_aligned_mirror_conj_test(TMirrorConjTest *thiz, int n)
{
    typedef typename TMirrorConjTest::value_type value_type;
    typedef std::complex<value_type> complex_type;

    std::vector<complex_type> expected;

    for (int i = 0; i < (n / 2) + 1; ++i) {
        expected.push_back(complex_type(static_cast<value_type>(i), static_cast<value_type>(i * 0.3)));
    }
    for (int i = 0; i < (n / 2); ++i) {
        expected.push_back(std::conj(expected[(n / 2) - 1 - i]));
    }

    thiz->data_.allocate(n);
    ::memcpy(&(thiz->data_[0]), &expected[0], sizeof(complex_type) * ((n / 2) + 1));
    utils::mirror_conj_aligned(&(thiz->data_[n / 2]), (n / 2));

    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_COMPLEX_EQ(expected[i], thiz->data_[i]);
    }
}

//
// utils::mirror_conj(float *center, int n)
//
typedef MirrorConjTest<float> MirrorConjTest_Float;
TEST_F(MirrorConjTest_Float, mirror_conj) { do_mirror_conj_test(this, 257); }

//
// utils::mirror_conj(double *center, int n)
//
typedef MirrorConjTest<double> MirrorConjTest_Double;
TEST_F(MirrorConjTest_Double, mirror_conj) { do_mirror_conj_test(this, 257); }

//
// utils::mirror_conj_aligned(float *center, int n)
//
typedef MirrorConjTest<float> MirrorConjAlignedTest_Float;
TEST_F(MirrorConjAlignedTest_Float, mirror_conj_aligned) { do_aligned_mirror_conj_test(this, 257); }

//
// utils::mirror_conj_aligned(double *center, int n)
//
typedef MirrorConjTest<double> MirrorConjAlignedTest_Double;
TEST_F(MirrorConjAlignedTest_Double, mirror_conj_aligned) { do_aligned_mirror_conj_test(this, 257); }
