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

using namespace cxxdasp;
using ::testing::Eq;
using ::testing::FloatEq;

class CopyTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}
};

//
// utils::fast_pod_copy()
//
TEST(CopyTest, fast_pod_copy_float_float)
{
    std::vector<float> src;
    std::vector<float> dest;

    for (int i = 0; i < 10; ++i) {
        src.push_back(static_cast<float>(i));
    }

    dest.resize(src.size());
    utils::fast_pod_copy(&dest[0], &src[0], static_cast<int>(src.size()));

    for (std::vector<float>::size_type i = 0; i < src.size(); ++i) {
        ASSERT_FLOAT_EQ(src[i], dest[i]);
    }
}

TEST(CopyTest, fast_pod_copy_float_double)
{
    std::vector<float> src;
    std::vector<double> dest;

    for (int i = 0; i < 10; ++i) {
        src.push_back(static_cast<float>(i));
    }

    dest.resize(src.size());
    utils::fast_pod_copy(&dest[0], &src[0], static_cast<int>(src.size()));

    for (std::vector<float>::size_type i = 0; i < src.size(); ++i) {
        ASSERT_DOUBLE_EQ(static_cast<double>(src[i]), dest[i]);
    }
}

//
// utils::stride_pod_copy()
//
TEST(CopyTest, stride_pod_copy_float_1_float_1)
{
    std::vector<float> src;
    std::vector<float> dest;

    for (int i = 0; i < 10; ++i) {
        src.push_back(static_cast<float>(i));
    }

    dest.resize(src.size());
    utils::stride_pod_copy(&dest[0], 1, &src[0], 1, static_cast<int>(src.size()));

    for (std::vector<float>::size_type i = 0; i < src.size(); ++i) {
        ASSERT_FLOAT_EQ(src[i], dest[i]);
    }
}

TEST(CopyTest, stride_pod_copy_float_1_double_1)
{
    std::vector<float> src;
    std::vector<double> dest;

    for (std::vector<float>::size_type i = 0; i < 10; ++i) {
        src.push_back(static_cast<float>(i));
    }

    dest.resize(src.size());
    utils::stride_pod_copy(&dest[0], 1, &src[0], 1, static_cast<int>(src.size()));

    for (std::vector<float>::size_type i = 0; i < src.size(); ++i) {
        ASSERT_DOUBLE_EQ(static_cast<double>(src[i]), dest[i]);
    }
}

TEST(CopyTest, stride_pod_copy_float_2_double_1)
{
    std::vector<float> src;
    std::vector<double> dest;

    for (int i = 0; i < 10; ++i) {
        src.push_back(static_cast<float>(i));
    }

    dest.resize(src.size() / 2);
    utils::stride_pod_copy(&dest[0], 1, &src[0], 2, static_cast<int>(src.size()) / 2);

    for (std::vector<float>::size_type i = 0; i < src.size() / 2; ++i) {
        ASSERT_DOUBLE_EQ(static_cast<double>(src[2 * i]), dest[i]);
    }
}

TEST(CopyTest, stride_pod_copy_float_2_double_2)
{
    std::vector<float> src;
    std::vector<double> dest;

    for (int i = 0; i < 10; ++i) {
        src.push_back(static_cast<float>(i));
    }

    dest.resize(src.size());
    utils::stride_pod_copy(&dest[0], 2, &src[0], 2, static_cast<int>(src.size()) / 2);

    for (std::vector<float>::size_type i = 0; i < src.size(); ++i) {
        if (i % 2 == 0) {
            ASSERT_DOUBLE_EQ(static_cast<double>(src[i]), dest[i]);
        } else {
            ASSERT_DOUBLE_EQ(0.0, dest[i]);
        }
    }
}
