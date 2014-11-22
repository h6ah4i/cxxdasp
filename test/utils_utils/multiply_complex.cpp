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
namespace compat = cxxporthelper::complex;

template <typename T, int N>
class ComplexMultiplyScalerCommonSrcDestTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        cxxdasp_init();
        src_.allocate(N);
        src_dest_.allocate(N);
    }
    virtual void TearDown() {}

    void prepare_src_dest() { ::memcpy(&src_dest_[0], &src_[0], sizeof(std::complex<T>) * N); }

    static const int n_ = N;
    cxxporthelper::aligned_memory<std::complex<T>> src_;
    cxxporthelper::aligned_memory<std::complex<T>> src_dest_;
};

template <typename T, int N>
class ComplexMultiplyScalerSeparateSrcDestTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        cxxdasp_init();
        src_.allocate(N);
        dest_.allocate(N);
    }
    virtual void TearDown() {}

    static const int n_ = N;
    cxxporthelper::aligned_memory<std::complex<T>> src_;
    cxxporthelper::aligned_memory<std::complex<T>> dest_;
};

template <typename T, int N>
class ComplexMultiplyCommonSrcDestTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        cxxdasp_init();
        src_.allocate(N);
        src_dest_.allocate(N);
        x_.allocate(N);
    }
    virtual void TearDown() {}

    void prepare_src_dest() { ::memcpy(&src_dest_[0], &src_[0], sizeof(std::complex<T>) * N); }

    static const int n_ = N;
    cxxporthelper::aligned_memory<std::complex<T>> src_;
    cxxporthelper::aligned_memory<std::complex<T>> src_dest_;
    cxxporthelper::aligned_memory<std::complex<T>> x_;
};

template <typename T, int N>
class ComplexMultiplySeparateSrcDestTest : public ::testing::Test {
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
    cxxporthelper::aligned_memory<std::complex<T>> src_;
    cxxporthelper::aligned_memory<std::complex<T>> dest_;
    cxxporthelper::aligned_memory<std::complex<T>> x_;
};

//
// utils::multiply_scaler(float *src_dest, float x, int n)
//
typedef ComplexMultiplyScalerCommonSrcDestTest<float, 100> ComplexMultiplyScalerCommonSrcDestTest_Float_100;
TEST_F(ComplexMultiplyScalerCommonSrcDestTest_Float_100, multiply_scaler)
{
    const float x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<float>(i));
        compat::set_imag(src_[i], static_cast<float>(i * 0.3f));
    }

    prepare_src_dest();
    utils::multiply_scaler(&src_dest_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_COMPLEX_EQ(src_[i] * x, src_dest_[i]);
    }
}

//
// utils::multiply_scaler(double *src_dest, double x, int n)
//
typedef ComplexMultiplyScalerCommonSrcDestTest<double, 100> ComplexMultiplyScalerCommonSrcDestTest_Double_100;
TEST_F(ComplexMultiplyScalerCommonSrcDestTest_Double_100, multiply_scaler)
{
    const double x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<double>(i));
        compat::set_imag(src_[i], static_cast<double>(i * 0.3));
    }

    prepare_src_dest();
    utils::multiply_scaler(&src_dest_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_COMPLEX_EQ(src_[i] * x, src_dest_[i]);
    }
}

//
// utils::multiply_scaler(float *dest, float *src, float x, int n)
//
typedef ComplexMultiplyScalerSeparateSrcDestTest<float, 100> ComplexMultiplyScalerSeparateSrcDestTest_Float_100;
TEST_F(ComplexMultiplyScalerSeparateSrcDestTest_Float_100, multiply_scaler)
{
    const float x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<float>(i));
        compat::set_imag(src_[i], static_cast<float>(i * 0.3f));
    }

    utils::multiply_scaler(&dest_[0], &src_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_COMPLEX_EQ(src_[i] * x, dest_[i]);
    }
}

//
// utils::multiply_scaler(double *dest, double *src, double x, int n)
//
typedef ComplexMultiplyScalerSeparateSrcDestTest<double, 100> ComplexMultiplyScalerSeparateSrcDestTest_Double_100;
TEST_F(ComplexMultiplyScalerSeparateSrcDestTest_Double_100, multiply_scaler)
{
    const double x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<double>(i));
        compat::set_imag(src_[i], static_cast<double>(i * 0.3));
    }

    utils::multiply_scaler(&dest_[0], &src_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_COMPLEX_EQ(src_[i] * x, dest_[i]);
    }
}

//
// utils::multiply_scaler_aligned(float *src_dest, float x, int n)
//
typedef ComplexMultiplyScalerCommonSrcDestTest<float, 100> ComplexMultiplyScalerAlignedCommonSrcDestTest_Float_100;
TEST_F(ComplexMultiplyScalerAlignedCommonSrcDestTest_Float_100, multiply_scaler_aligned)
{
    const float x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<float>(i));
        compat::set_imag(src_[i], static_cast<float>(i * 0.3f));
    }

    prepare_src_dest();
    utils::multiply_scaler_aligned(&src_dest_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_COMPLEX_EQ(src_[i] * x, src_dest_[i]);
    }
}

//
// utils::multiply_scaler_aligned(double *src_dest, double x, int n)
//
typedef ComplexMultiplyScalerCommonSrcDestTest<double, 100> ComplexMultiplyScalerAlignedCommonSrcDestTest_Double_100;
TEST_F(ComplexMultiplyScalerAlignedCommonSrcDestTest_Double_100, multiply_scaler_aligned)
{
    const double x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<double>(i));
        compat::set_imag(src_[i], static_cast<double>(i * 0.3));
    }

    prepare_src_dest();
    utils::multiply_scaler_aligned(&src_dest_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_COMPLEX_EQ(src_[i] * x, src_dest_[i]);
    }
}

//
// utils::multiply_scaler_aligned(float *dest, float *src, float x, int n)
//
typedef ComplexMultiplyScalerSeparateSrcDestTest<float, 100> ComplexMultiplyScalerAlignedSeparateSrcDestTest_Float_100;
TEST_F(ComplexMultiplyScalerAlignedSeparateSrcDestTest_Float_100, multiply_scaler_aligned)
{
    const float x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<float>(i));
        compat::set_imag(src_[i], static_cast<float>(i * 0.3f));
    }

    utils::multiply_scaler_aligned(&dest_[0], &src_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_COMPLEX_EQ(src_[i] * x, dest_[i]);
    }
}

//
// utils::multiply_scaler_aligned(double *dest, double *src, double x, int n)
//
typedef ComplexMultiplyScalerSeparateSrcDestTest<double, 100>
ComplexMultiplyScalerAlignedSeparateSrcDestTest_Double_100;
TEST_F(ComplexMultiplyScalerAlignedSeparateSrcDestTest_Double_100, multiply_scaler_aligned)
{
    const double x = 3.3f;

    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<double>(i));
        compat::set_imag(src_[i], static_cast<double>(i * 0.3));
    }

    utils::multiply_scaler_aligned(&dest_[0], &src_[0], x, n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_COMPLEX_EQ(src_[i] * x, dest_[i]);
    }
}

//
// utils::multiply(float *src_dest, float *x, int n)
//
typedef ComplexMultiplyCommonSrcDestTest<float, 100> ComplexMultiplyCommonSrcDestTest_Float_100;
TEST_F(ComplexMultiplyCommonSrcDestTest_Float_100, multiply)
{
    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<float>(i));
        compat::set_imag(src_[i], static_cast<float>(i * 0.3f));
        compat::set_real(x_[i], i * 0.1f);
        compat::set_imag(x_[i], i * 0.2f);
    }

    prepare_src_dest();
    utils::multiply(&src_dest_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_COMPLEX_EQ(src_[i] * x_[i], src_dest_[i]);
    }
}

//
// utils::multiply(double *src_dest, double *x, int n)
//
typedef ComplexMultiplyCommonSrcDestTest<double, 100> ComplexMultiplyCommonSrcDestTest_Double_100;
TEST_F(ComplexMultiplyCommonSrcDestTest_Double_100, multiply)
{
    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<double>(i));
        compat::set_imag(src_[i], static_cast<double>(i * 0.3));
        compat::set_real(x_[i], i * 0.1);
        compat::set_imag(x_[i], i * 0.2);
    }

    prepare_src_dest();
    utils::multiply(&src_dest_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_COMPLEX_EQ(src_[i] * x_[i], src_dest_[i]);
    }
}

//
// utils::multiply(float *dest, float *src, float *x, int n)
//
typedef ComplexMultiplySeparateSrcDestTest<float, 100> ComplexMultiplySeparateSrcDestTest_Float_100;
TEST_F(ComplexMultiplySeparateSrcDestTest_Float_100, multiply)
{
    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<float>(i));
        compat::set_imag(src_[i], static_cast<float>(i * 0.3f));
        compat::set_real(x_[i], i * 0.1f);
        compat::set_imag(x_[i], i * 0.2f);
    }

    utils::multiply(&dest_[0], &src_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_COMPLEX_EQ(src_[i] * x_[i], dest_[i]);
    }
}

//
// utils::multiply(double *dest, double *src, double *x, int n)
//
typedef ComplexMultiplySeparateSrcDestTest<double, 100> ComplexMultiplySeparateSrcDestTest_Double_100;
TEST_F(ComplexMultiplySeparateSrcDestTest_Double_100, multiply)
{
    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<double>(i));
        compat::set_imag(src_[i], static_cast<double>(i * 0.3));
        compat::set_real(x_[i], i * 0.1);
        compat::set_imag(x_[i], i * 0.2);
    }

    utils::multiply(&dest_[0], &src_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_COMPLEX_EQ(src_[i] * x_[i], dest_[i]);
    }
}

//
// utils::multiply_aligned(float *src_dest, float *x, int n)
//
typedef ComplexMultiplyCommonSrcDestTest<float, 100> ComplexMultiplyAlignedCommonSrcDestTest_Float_100;
TEST_F(ComplexMultiplyAlignedCommonSrcDestTest_Float_100, multiply_aligned)
{
    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<float>(i));
        compat::set_imag(src_[i], static_cast<float>(i * 0.3f));
        compat::set_real(x_[i], i * 0.1f);
        compat::set_imag(x_[i], i * 0.2f);
    }

    prepare_src_dest();
    utils::multiply_aligned(&src_dest_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_COMPLEX_EQ(src_[i] * x_[i], src_dest_[i]);
    }
}

//
// utils::multiply_aligned(double *src_dest, double *x, int n)
//
typedef ComplexMultiplyCommonSrcDestTest<double, 100> ComplexMultiplyAlignedCommonSrcDestTest_Double_100;
TEST_F(ComplexMultiplyAlignedCommonSrcDestTest_Double_100, multiply_aligned)
{
    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<double>(i));
        compat::set_imag(src_[i], static_cast<double>(i * 0.3));
        compat::set_real(x_[i], i * 0.1);
        compat::set_imag(x_[i], i * 0.2);
    }

    prepare_src_dest();
    utils::multiply_aligned(&src_dest_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_COMPLEX_EQ(src_[i] * x_[i], src_dest_[i]);
    }
}

//
// utils::multiply_aligned(float *dest, float *src, float *x, int n)
//
typedef ComplexMultiplySeparateSrcDestTest<float, 100> ComplexMultiplyAlignedAlignedSeparateSrcDestTest_Float_100;
TEST_F(ComplexMultiplyAlignedAlignedSeparateSrcDestTest_Float_100, multiply_aligned)
{

    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<float>(i));
        compat::set_imag(src_[i], static_cast<float>(i * 0.3f));
        compat::set_real(x_[i], i * 0.1f);
        compat::set_imag(x_[i], i * 0.2f);
    }

    utils::multiply_aligned(&dest_[0], &src_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_FLOAT_COMPLEX_EQ(src_[i] * x_[i], dest_[i]);
    }
}

//
// utils::multiply_aligned(double *dest, double *src, double *x, int n)
//
typedef ComplexMultiplySeparateSrcDestTest<double, 100> ComplexMultiplyAlignedSeparateSrcDestTest_Double_100;
TEST_F(ComplexMultiplyAlignedSeparateSrcDestTest_Double_100, multiply_aligned)
{
    for (int i = 0; i < n_; ++i) {
        compat::set_real(src_[i], static_cast<double>(i));
        compat::set_imag(src_[i], static_cast<double>(i * 0.3));
        compat::set_real(x_[i], i * 0.1);
        compat::set_imag(x_[i], i * 0.2);
    }

    utils::multiply_aligned(&dest_[0], &src_[0], &x_[0], n_);

    for (int i = 0; i < n_; ++i) {
        ASSERT_DOUBLE_COMPLEX_EQ(src_[i] * x_[i], dest_[i]);
    }
}
