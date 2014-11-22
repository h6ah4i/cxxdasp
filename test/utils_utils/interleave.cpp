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
class InterleaveTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef T value_type;
};

template <typename T>
class DeinterleaveTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef T value_type;
};

typedef InterleaveTest<float> InterleaveTest_Float;
typedef InterleaveTest<double> InterleaveTest_Double;

typedef DeinterleaveTest<float> DeinterleaveTest_Float;
typedef DeinterleaveTest<double> DeinterleaveTest_Double;

template <typename TInterleaveTest>
void do_interleave_test(TInterleaveTest *thiz, int n)
{
    typedef typename TInterleaveTest::value_type value_type;

    std::vector<value_type> in_data1;
    std::vector<value_type> in_data2;
    std::vector<value_type> expected;
    std::vector<value_type> out_data;

    for (int i = 0; i < n; ++i) {
        in_data1.push_back(static_cast<float>(i));
        in_data2.push_back(static_cast<float>(i * 2));
    }

    for (int i = 0; i < n; ++i) {
        expected.push_back(in_data1[i]);
        expected.push_back(in_data2[i]);
    }

    out_data.resize(2 * n);
    utils::interleave(&out_data[0], &in_data1[0], &in_data2[0], n);

    for (int i = 0; i < n; ++i) {
        ASSERT_EQ(expected[2 * i + 0], out_data[2 * i + 0]);
        ASSERT_EQ(expected[2 * i + 1], out_data[2 * i + 1]);
    }
}

template <typename TInterleaveTest>
void do_deinterleave_test(TInterleaveTest *thiz, int n)
{
    typedef typename TInterleaveTest::value_type value_type;

    std::vector<value_type> in_data;
    std::vector<value_type> expected1;
    std::vector<value_type> expected2;
    std::vector<value_type> out_data1;
    std::vector<value_type> out_data2;

    for (int i = 0; i < (2 * n); ++i) {
        in_data.push_back(static_cast<float>(i));
    }

    for (int i = 0; i < n; ++i) {
        expected1.push_back(in_data[2 * i + 0]);
        expected2.push_back(in_data[2 * i + 1]);
    }

    out_data1.resize(n);
    out_data2.resize(n);
    utils::deinterleave(&out_data1[0], &out_data2[0], &in_data[0], n);

    for (int i = 0; i < n; ++i) {
        ASSERT_EQ(expected1[i], out_data1[i]);
        ASSERT_EQ(expected2[i], out_data2[i]);
    }
}

//
// utils::interleave(float *dest, const float *src1, const float *src2, int n)
//
TEST_F(InterleaveTest_Float, interleave) { do_interleave_test(this, 1000); }

TEST_F(InterleaveTest_Double, interleave) { do_interleave_test(this, 1000); }

//
// utils::deinterleave(float *dest1, float *dest2, const float *src, int n)
//
TEST_F(DeinterleaveTest_Float, deinterleave) { do_deinterleave_test(this, 1000); }

TEST_F(DeinterleaveTest_Double, deinterleave) { do_deinterleave_test(this, 1000); }
