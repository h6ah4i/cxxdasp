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

#include <cxxdasp/utils/utils.hpp>

using namespace cxxdasp;

class MiscTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}
};

//
// utils::gcd()
//
TEST(MiscTest, gcd__0_0) { ASSERT_EQ(0, utils::gcd(0, 0)); }

TEST(MiscTest, gcd__a_is_zero)
{
    ASSERT_EQ(-2, utils::gcd(0, -2));
    ASSERT_EQ(-1, utils::gcd(0, -1));
    ASSERT_EQ(1, utils::gcd(0, 1));
    ASSERT_EQ(2, utils::gcd(0, 2));
}

TEST(MiscTest, gcd__b_is_zero)
{
    ASSERT_EQ(-2, utils::gcd(-2, 0));
    ASSERT_EQ(-1, utils::gcd(-1, 0));
    ASSERT_EQ(1, utils::gcd(1, 0));
    ASSERT_EQ(2, utils::gcd(2, 0));
}

TEST(MiscTest, gcd__3_6) { ASSERT_EQ(3, utils::gcd(3, 6)); }

//
// utils::forward_fft_real_num_outputs
//
TEST(MiscTest, forward_fft_real_num__outputs_odd)
{
    // (n + 1) / 2
    ASSERT_EQ((1 + 1) / 2, utils::forward_fft_real_num_outputs(1));
    ASSERT_EQ((3 + 1) / 2, utils::forward_fft_real_num_outputs(3));
    ASSERT_EQ((5 + 1) / 2, utils::forward_fft_real_num_outputs(5));
}

TEST(MiscTest, forward_fft_real_num__outputs_even)
{
    // (n / 2) + 1
    ASSERT_EQ((2 / 2) + 1, utils::forward_fft_real_num_outputs(2));
    ASSERT_EQ((4 / 2) + 1, utils::forward_fft_real_num_outputs(4));
    ASSERT_EQ((6 / 2) + 1, utils::forward_fft_real_num_outputs(6));
}

//
// utils::is_pow_of_two
//

TEST(MiscTest, is_pow_of_two__0) { ASSERT_FALSE(utils::is_pow_of_two(0)); }

TEST(MiscTest, is_pow_of_two__true)
{
    ASSERT_TRUE(utils::is_pow_of_two(1));
    ASSERT_TRUE(utils::is_pow_of_two(2));
    ASSERT_TRUE(utils::is_pow_of_two(4));
    ASSERT_TRUE(utils::is_pow_of_two(8));
}

TEST(MiscTest, is_pow_of_two__false)
{
    ASSERT_FALSE(utils::is_pow_of_two(3));
    ASSERT_FALSE(utils::is_pow_of_two(5));
    ASSERT_FALSE(utils::is_pow_of_two(6));
    ASSERT_FALSE(utils::is_pow_of_two(7));
}

//
// utils::next_pow_of_two
//
TEST(MiscTest, next_pow_of_two__0) { ASSERT_EQ(1, utils::next_pow_of_two(0)); }

TEST(MiscTest, next_pow_of_two__pot)
{
    ASSERT_EQ(1, utils::next_pow_of_two(1));
    ASSERT_EQ(2, utils::next_pow_of_two(2));
    ASSERT_EQ(4, utils::next_pow_of_two(4));
    ASSERT_EQ(8, utils::next_pow_of_two(8));
}

TEST(MiscTest, next_pow_of_two__npot)
{
    ASSERT_EQ(4, utils::next_pow_of_two(3));
    ASSERT_EQ(8, utils::next_pow_of_two(5));
    ASSERT_EQ(8, utils::next_pow_of_two(6));
    ASSERT_EQ(8, utils::next_pow_of_two(7));
}

//
// utils::clamp
//
TEST(MiscTest, clamp__int)
{
    ASSERT_EQ(-10, utils::clamp(-100, -10, 10));
    ASSERT_EQ(0, utils::clamp(0, -10, 10));
    ASSERT_EQ(10, utils::clamp(100, -10, 10));
}

TEST(MiscTest, clamp__float)
{
    ASSERT_EQ(-10.0f, utils::clamp(-100.0f, -10.0f, 10.0f));
    ASSERT_EQ(0.0f, utils::clamp(0.0f, -10.0f, 10.0f));
    ASSERT_EQ(10.0f, utils::clamp(100.0f, -10.0f, 10.0f));
}

TEST(MiscTest, clamp__double)
{
    ASSERT_EQ(-10.0, utils::clamp(-100.0, -10.0, 10.0));
    ASSERT_EQ(0.0, utils::clamp(0.0, -10.0, 10.0));
    ASSERT_EQ(10.0, utils::clamp(100.0, -10.0, 10.0));
}
