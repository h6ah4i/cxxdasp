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

template <typename T, int N>
class RealMultiplyScalerCommonSrcDestTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        cxxdasp_init();
        src_.allocate(N);
        src_dest_.allocate(N);
    }
    virtual void TearDown() {}

    void prepare_src_dest() { ::memcpy(&src_dest_[0], &src_[0], sizeof(T) * N); }

    static const int n_ = N;
    cxxporthelper::aligned_memory<T> src_;
    cxxporthelper::aligned_memory<T> src_dest_;
};

template <typename T, int N>
class RealMultiplyScalerSeparateSrcDestTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        cxxdasp_init();
        src_.allocate(N);
        dest_.allocate(N);
    }
    virtual void TearDown() {}

    static const int n_ = N;
    cxxporthelper::aligned_memory<T> src_;
    cxxporthelper::aligned_memory<T> dest_;
};

template <typename T, int N>
class RealMultiplyCommonSrcDestTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        cxxdasp_init();
        src_.allocate(N);
        src_dest_.allocate(N);
        x_.allocate(N);
    }
    virtual void TearDown() {}

    void prepare_src_dest() { ::memcpy(&src_dest_[0], &src_[0], sizeof(T) * N); }

    static const int n_ = N;
    cxxporthelper::aligned_memory<T> src_;
    cxxporthelper::aligned_memory<T> src_dest_;
    cxxporthelper::aligned_memory<T> x_;
};

template <typename T, int N>
class RealMultiplySeparateSrcDestTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        cxxdasp_init();
        src_.allocate(N);
        dest_.allocate(N);
        x_.allocate(N);
    }
    virtual void TearDown() {}

    static const int n_ = N;
    cxxporthelper::aligned_memory<T> src_;
    cxxporthelper::aligned_memory<T> dest_;
    cxxporthelper::aligned_memory<T> x_;
};

//
// utils::multiply_scaler(float *src_dest, float x, int n)
//
typedef RealMultiplyScalerCommonSrcDestTest<float, 100> RealMultiplyScalerCommonSrcDestTest_Float_100;
TEST_F(RealMultiplyScalerCommonSrcDestTest_Float_100, multiply_scaler)
{
    const float x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<float>(i);
    }

    prepare_src_dest();
    utils::multiply_scaler(&src_dest_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_EQ(src_[i] * x, src_dest_[i]);
    }
}

//
// utils::multiply_scaler(double *src_dest, double x, int n)
//
typedef RealMultiplyScalerCommonSrcDestTest<double, 100> RealMultiplyScalerCommonSrcDestTest_Double_100;
TEST_F(RealMultiplyScalerCommonSrcDestTest_Double_100, multiply_scaler)
{
    const double x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<double>(i);
    }

    prepare_src_dest();
    utils::multiply_scaler(&src_dest_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_EQ(src_[i] * x, src_dest_[i]);
    }
}

//
// utils::multiply_scaler(float *dest, float *src, float x, int n)
//
typedef RealMultiplyScalerSeparateSrcDestTest<float, 100> RealMultiplyScalerSeparateSrcDestTest_Float_100;
TEST_F(RealMultiplyScalerSeparateSrcDestTest_Float_100, multiply_scaler)
{
    const float x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<float>(i);
    }

    utils::multiply_scaler(&dest_[0], &src_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_EQ(src_[i] * x, dest_[i]);
    }
}

//
// utils::multiply_scaler(double *dest, double *src, double x, int n)
//
typedef RealMultiplyScalerSeparateSrcDestTest<double, 100> RealMultiplyScalerSeparateSrcDestTest_Double_100;
TEST_F(RealMultiplyScalerSeparateSrcDestTest_Double_100, multiply_scaler)
{
    const double x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<double>(i);
    }

    utils::multiply_scaler(&dest_[0], &src_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_EQ(src_[i] * x, dest_[i]);
    }
}

//
// utils::multiply_scaler_aligned(float *src_dest, float x, int n)
//
typedef RealMultiplyScalerCommonSrcDestTest<float, 100> RealMultiplyScalerAlignedCommonSrcDestTest_Float_100;
TEST_F(RealMultiplyScalerAlignedCommonSrcDestTest_Float_100, multiply_scaler_aligned)
{
    const float x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<float>(i);
    }

    prepare_src_dest();
    utils::multiply_scaler_aligned(&src_dest_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_EQ(src_[i] * x, src_dest_[i]);
    }
}

//
// utils::multiply_scaler_aligned(double *src_dest, double x, int n)
//
typedef RealMultiplyScalerCommonSrcDestTest<double, 100> RealMultiplyScalerAlignedCommonSrcDestTest_Double_100;
TEST_F(RealMultiplyScalerAlignedCommonSrcDestTest_Double_100, multiply_scaler_aligned)
{
    const double x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<double>(i);
    }

    prepare_src_dest();
    utils::multiply_scaler_aligned(&src_dest_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_EQ(src_[i] * x, src_dest_[i]);
    }
}

//
// utils::multiply_scaler_aligned(float *dest, float *src, float x, int n)
//
typedef RealMultiplyScalerSeparateSrcDestTest<float, 100> RealMultiplyScalerAlignedSeparateSrcDestTest_Float_100;
TEST_F(RealMultiplyScalerAlignedSeparateSrcDestTest_Float_100, multiply_scaler_aligned)
{
    const float x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<float>(i);
    }

    utils::multiply_scaler_aligned(&dest_[0], &src_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_EQ(src_[i] * x, dest_[i]);
    }
}

//
// utils::multiply_scaler_aligned(double *dest, double *src, double x, int n)
//
typedef RealMultiplyScalerSeparateSrcDestTest<double, 100> RealMultiplyScalerAlignedSeparateSrcDestTest_Double_100;
TEST_F(RealMultiplyScalerAlignedSeparateSrcDestTest_Double_100, multiply_scaler_aligned)
{
    const double x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<double>(i);
    }

    utils::multiply_scaler_aligned(&dest_[0], &src_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_EQ(src_[i] * x, dest_[i]);
    }
}

//
// utils::multiply(float *src_dest, float *x, int n)
//
typedef RealMultiplyCommonSrcDestTest<float, 100> RealMultiplyCommonSrcDestTest_Float_100;
TEST_F(RealMultiplyCommonSrcDestTest_Float_100, multiply)
{
    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<float>(i);
        x_[i] = i * 0.1f;
    }

    prepare_src_dest();
    utils::multiply(&src_dest_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_EQ(src_[i] * x_[i], src_dest_[i]);
    }
}

//
// utils::multiply(double *src_dest, double *x, int n)
//
typedef RealMultiplyCommonSrcDestTest<double, 100> RealMultiplyCommonSrcDestTest_Double_100;
TEST_F(RealMultiplyCommonSrcDestTest_Double_100, multiply)
{
    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<double>(i);
        x_[i] = i * 0.1;
    }

    prepare_src_dest();
    utils::multiply(&src_dest_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_EQ(src_[i] * x_[i], src_dest_[i]);
    }
}

//
// utils::multiply(float *dest, float *src, float *x, int n)
//
typedef RealMultiplySeparateSrcDestTest<float, 100> RealMultiplySeparateSrcDestTest_Float_100;
TEST_F(RealMultiplySeparateSrcDestTest_Float_100, multiply)
{
    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<float>(i);
        x_[i] = i * 0.1f;
    }

    utils::multiply(&dest_[0], &src_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_EQ(src_[i] * x_[i], dest_[i]);
    }
}

//
// utils::multiply(double *dest, double *src, double *x, int n)
//
typedef RealMultiplySeparateSrcDestTest<double, 100> RealMultiplySeparateSrcDestTest_Double_100;
TEST_F(RealMultiplySeparateSrcDestTest_Double_100, multiply)
{
    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<double>(i);
        x_[i] = i * 0.1;
    }

    utils::multiply(&dest_[0], &src_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_EQ(src_[i] * x_[i], dest_[i]);
    }
}

//
// utils::multiply_aligned(float *src_dest, float *x, int n)
//
typedef RealMultiplyCommonSrcDestTest<float, 100> RealMultiplyAlignedCommonSrcDestTest_Float_100;
TEST_F(RealMultiplyAlignedCommonSrcDestTest_Float_100, multiply_aligned)
{
    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<float>(i);
        x_[i] = i * 0.1f;
    }

    prepare_src_dest();
    utils::multiply_aligned(&src_dest_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_EQ(src_[i] * x_[i], src_dest_[i]);
    }
}

//
// utils::multiply_aligned(double *src_dest, double *x, int n)
//
typedef RealMultiplyCommonSrcDestTest<double, 100> RealMultiplyAlignedCommonSrcDestTest_Double_100;
TEST_F(RealMultiplyAlignedCommonSrcDestTest_Double_100, multiply_aligned)
{
    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<double>(i);
        x_[i] = i * 0.1;
    }

    prepare_src_dest();
    utils::multiply_aligned(&src_dest_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_EQ(src_[i] * x_[i], src_dest_[i]);
    }
}

//
// utils::multiply_aligned(float *dest, float *src, float *x, int n)
//
typedef RealMultiplySeparateSrcDestTest<float, 100> RealMultiplyAlignedSeparateSrcDestTest_Float_100;
TEST_F(RealMultiplyAlignedSeparateSrcDestTest_Float_100, multiply_aligned)
{
    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<float>(i);
        x_[i] = i * 0.1f;
    }

    utils::multiply_aligned(&dest_[0], &src_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_EQ(src_[i] * x_[i], dest_[i]);
    }
}

//
// utils::multiply_aligned(double *dest, double *src, double *x, int n)
//
typedef RealMultiplySeparateSrcDestTest<double, 100> RealMultiplyAlignedSeparateSrcDestTest_Double_100;
TEST_F(RealMultiplyAlignedSeparateSrcDestTest_Double_100, multiply_aligned)
{
    for (int i = 0; i < n_; ++i) {
        src_[i] = static_cast<double>(i);
        x_[i] = i * 0.1;
    }

    utils::multiply_aligned(&dest_[0], &src_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_EQ(src_[i] * x_[i], dest_[i]);
    }
}
