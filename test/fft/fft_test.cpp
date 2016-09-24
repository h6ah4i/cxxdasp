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

#include <cxxdasp/utils/utils.hpp>
#include <cxxdasp/fft/fft.hpp>

using namespace cxxdasp;

// #undef CXXDASP_USE_FFT_BACKEND_PFFFT
// #undef CXXDASP_USE_FFT_BACKEND_FFTS
// #undef CXXDASP_USE_FFT_BACKEND_KISS_FFT
// #undef CXXDASP_USE_FFT_BACKEND_FFTWF
// #undef CXXDASP_USE_FFT_BACKEND_NE10
// #undef CXXDASP_USE_FFT_BACKEND_CKFFT
// #undef CXXDASP_USE_FFT_BACKEND_MUFFT
// #undef CXXDASP_USE_FFT_BACKEND_KFR_F
// #undef CXXDASP_USE_FFT_BACKEND_FFTW
// #undef CXXDASP_USE_FFT_BACKEND_GP_FFT
// #undef CXXDASP_USE_FFT_BACKEND_KFR_D

template <typename TFFTBackend, typename T>
class ForwardFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef typename TFFTBackend::forward fft_backend_type;
    typedef T data_type;

    fft::fft<std::complex<T>, std::complex<T>, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<std::complex<T>> in_;
    cxxporthelper::aligned_memory<std::complex<T>> out_;
};

template <typename TFFTBackend, typename T>
class InverseFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef typename TFFTBackend::inverse fft_backend_type;
    typedef T data_type;

    fft::fft<std::complex<T>, std::complex<T>, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<std::complex<T>> in_;
    cxxporthelper::aligned_memory<std::complex<T>> out_;
};

template <typename TFFTBackend, typename T>
class ForwardRealFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef typename TFFTBackend::forward_real fft_backend_type;
    typedef T data_type;

    fft::fft<T, std::complex<T>, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<T> in_;
    cxxporthelper::aligned_memory<std::complex<T>> out_;
};

template <typename TFFTBackend, typename T>
class InverseRealFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef typename TFFTBackend::inverse_real fft_backend_type;
    typedef T data_type;

    fft::fft<std::complex<T>, T, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<std::complex<T>> in_;
    cxxporthelper::aligned_memory<T> out_;
};

template <typename TForwardFFTTest>
void do_forward_fft_test(TForwardFFTTest *thiz)
{
    typedef typename TForwardFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 16;
    // setup
    thiz->in_.allocate(n);
    thiz->out_.allocate(n);
    thiz->fft_.setup(n, &(thiz->in_[0]), &(thiz->out_[0]));

    std::vector<complex_type> expected;
    std::vector<complex_type> in_data;

    // in_data
    for (int i = 1; i <= n; ++i) {
        in_data.push_back(std::complex<data_type>(static_cast<data_type>(i), static_cast<data_type>(i)));
    }

    // expected
    const int scale = thiz->fft_.scale();
    ASSERT_EQ(1, scale);
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    expected.push_back(COMPLEX_VALUE(136.0 * scale, 136.0 * scale));
    expected.push_back(COMPLEX_VALUE(-48.218715937006785 * scale, 32.218715937006785 * scale));
    expected.push_back(COMPLEX_VALUE(-27.31370849898476 * scale, 11.313708498984761 * scale));
    expected.push_back(COMPLEX_VALUE(-19.97284610132391 * scale, 3.9728461013239116 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 0.0 * scale));
    expected.push_back(COMPLEX_VALUE(-13.34542910335439 * scale, -2.6545708966456107 * scale));
    expected.push_back(COMPLEX_VALUE(-11.313708498984761 * scale, -4.686291501015239 * scale));
    expected.push_back(COMPLEX_VALUE(-9.591298939037264 * scale, -6.408701060962734 * scale));
    expected.push_back(COMPLEX_VALUE(-8.0 * scale, -8.0 * scale));
    expected.push_back(COMPLEX_VALUE(-6.408701060962734 * scale, -9.591298939037264 * scale));
    expected.push_back(COMPLEX_VALUE(-4.686291501015239 * scale, -11.313708498984761 * scale));
    expected.push_back(COMPLEX_VALUE(-2.6545708966456107 * scale, -13.34542910335439 * scale));
    expected.push_back(COMPLEX_VALUE(0.0 * scale, -16.0 * scale));
    expected.push_back(COMPLEX_VALUE(3.9728461013239116 * scale, -19.97284610132391 * scale));
    expected.push_back(COMPLEX_VALUE(11.313708498984761 * scale, -27.31370849898476 * scale));
    expected.push_back(COMPLEX_VALUE(32.218715937006785 * scale, -48.218715937006785 * scale));
#undef COMPLEX_VALUE

    // perform FFT
    ::memcpy(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n);
    thiz->fft_.execute();

    //
    // check
    //

    // input data array have to be preserved
    ASSERT_EQ(0, ::memcmp(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n));

    // for (int i = 0; i < n; ++i) {
    //     std::cout << i << " : " << expected[i] << " - " << thiz->out_[i] << std::endl;
    // }

    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_COMPLEX_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(1e-5));
        // ASSERT_AUTO_COMPLEX_EQ(expected[i], thiz->out_[i]);
    }
}

template <typename TInverseFFTTest>
void do_inverse_fft_test(TInverseFFTTest *thiz)
{
    typedef typename TInverseFFTTest::fft_backend_type fft_backend_type;
    typedef typename TInverseFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 16;
    // setup
    thiz->in_.allocate(n);
    thiz->out_.allocate(n);
    thiz->fft_.setup(n, &(thiz->in_[0]), &(thiz->out_[0]));

    std::vector<complex_type> expected;
    std::vector<complex_type> in_data;

// in_data
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    in_data.push_back(COMPLEX_VALUE(136.0, 136.0));
    in_data.push_back(COMPLEX_VALUE(-48.218715937006785, 32.218715937006785));
    in_data.push_back(COMPLEX_VALUE(-27.31370849898476, 11.313708498984761));
    in_data.push_back(COMPLEX_VALUE(-19.97284610132391, 3.9728461013239116));
    in_data.push_back(COMPLEX_VALUE(-16.0, 0.0));
    in_data.push_back(COMPLEX_VALUE(-13.34542910335439, -2.6545708966456107));
    in_data.push_back(COMPLEX_VALUE(-11.313708498984761, -4.686291501015239));
    in_data.push_back(COMPLEX_VALUE(-9.591298939037264, -6.408701060962734));
    in_data.push_back(COMPLEX_VALUE(-8.0, -8.0));
    in_data.push_back(COMPLEX_VALUE(-6.408701060962734, -9.591298939037264));
    in_data.push_back(COMPLEX_VALUE(-4.686291501015239, -11.313708498984761));
    in_data.push_back(COMPLEX_VALUE(-2.6545708966456107, -13.34542910335439));
    in_data.push_back(COMPLEX_VALUE(0.0, -16.0));
    in_data.push_back(COMPLEX_VALUE(3.9728461013239116, -19.97284610132391));
    in_data.push_back(COMPLEX_VALUE(11.313708498984761, -27.31370849898476));
    in_data.push_back(COMPLEX_VALUE(32.218715937006785, -48.218715937006785));
#undef COMPLEX_VALUE

    // expected
    const int scale = thiz->fft_.scale();
    for (int i = 1; i <= n; ++i) {
        expected.push_back(complex_type(static_cast<data_type>(i * scale), static_cast<data_type>(i * scale)));
    }

    // perform FFT
    ::memcpy(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n);
    thiz->fft_.execute();

    //
    // check
    //

    // input data array have to be preserved
    ASSERT_EQ(0, ::memcmp(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n));

    // for (int i = 0; i < n; ++i) {
    //     std::cout << i << " : " << expected[i] << " - " << thiz->out_[i] << std::endl;
    // }

    for (int i = 0; i < n; ++i) {
        ASSERT_AUTO_COMPLEX_EQ(expected[i], thiz->out_[i]);
    }
}

template <typename TForwardFFTTest>
void do_forward_real_fft_test(TForwardFFTTest *thiz)
{
    typedef typename TForwardFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 32;
    const int n2 = utils::forward_fft_real_num_outputs(n);

    ASSERT_EQ((32 / 2 + 1), n2);

    // setup
    thiz->in_.allocate(n);
    thiz->out_.allocate(n2);
    thiz->fft_.setup(n, &(thiz->in_[0]), &(thiz->out_[0]));

    std::vector<complex_type> expected;
    std::vector<data_type> in_data;

    // in_data
    for (int i = 1; i <= n; ++i) {
        in_data.push_back(static_cast<data_type>(i));
    }

    // expected
    const int scale = thiz->fft_.scale();
    // ASSERT_EQ(1, scale);
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    expected.push_back(COMPLEX_VALUE(528.0 * scale, 0.0 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 162.45072620174176 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 80.43743187401357 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 52.74493134301312 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 38.62741699796952 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 29.933894588630228 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 23.945692202647823 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 19.496056409407625 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 16.0 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 13.130860653258562 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 10.690858206708779 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 8.55217817521267 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 6.627416997969522 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 4.8535469377174785 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 3.1825978780745316 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 1.5758624537146346 * scale));
    expected.push_back(COMPLEX_VALUE(-16.0 * scale, 0.0 * scale));
#undef COMPLEX_VALUE

    // perform FFT
    ::memcpy(&(thiz->in_[0]), &in_data[0], sizeof(data_type) * n);
    thiz->fft_.execute();

    //
    // check
    //

    // input data array have to be preserved
    ASSERT_EQ(0, ::memcmp(&(thiz->in_[0]), &in_data[0], sizeof(data_type) * n));

    // for (int i = 0; i < n2; ++i) {
    //     std::cout << i << " : " << expected[i] << " - " << thiz->out_[i] << std::endl;
    // }

    for (int i = 0; i < n2; ++i) {
        ASSERT_AUTO_COMPLEX_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0001));
    }
}

template <typename TInverseFFTTest>
void do_inverse_real_fft_test(TInverseFFTTest *thiz)
{
    typedef typename TInverseFFTTest::fft_backend_type fft_backend_type;
    typedef typename TInverseFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 32;
    const int n2 = utils::forward_fft_real_num_outputs(n);

    ASSERT_EQ((32 / 2 + 1), n2);

    // setup
    thiz->in_.allocate(n2);
    thiz->out_.allocate(n);
    thiz->fft_.setup(n, &(thiz->in_[0]), &(thiz->out_[0]));

    std::vector<data_type> expected;
    std::vector<complex_type> in_data;

// in_data
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    in_data.push_back(COMPLEX_VALUE(528.0, 0.0));
    in_data.push_back(COMPLEX_VALUE(-16.0, 162.45072620174176));
    in_data.push_back(COMPLEX_VALUE(-16.0, 80.43743187401357));
    in_data.push_back(COMPLEX_VALUE(-16.0, 52.74493134301312));
    in_data.push_back(COMPLEX_VALUE(-16.0, 38.62741699796952));
    in_data.push_back(COMPLEX_VALUE(-16.0, 29.933894588630228));
    in_data.push_back(COMPLEX_VALUE(-16.0, 23.945692202647823));
    in_data.push_back(COMPLEX_VALUE(-16.0, 19.496056409407625));
    in_data.push_back(COMPLEX_VALUE(-16.0, 16.0));
    in_data.push_back(COMPLEX_VALUE(-16.0, 13.130860653258562));
    in_data.push_back(COMPLEX_VALUE(-16.0, 10.690858206708779));
    in_data.push_back(COMPLEX_VALUE(-16.0, 8.55217817521267));
    in_data.push_back(COMPLEX_VALUE(-16.0, 6.627416997969522));
    in_data.push_back(COMPLEX_VALUE(-16.0, 4.8535469377174785));
    in_data.push_back(COMPLEX_VALUE(-16.0, 3.1825978780745316));
    in_data.push_back(COMPLEX_VALUE(-16.0, 1.5758624537146346));
    in_data.push_back(COMPLEX_VALUE(-16.0, 0.0));
#undef COMPLEX_VALUE

    // expected
    const int scale = thiz->fft_.scale();
    for (int i = 1; i <= n2; ++i) {
        expected.push_back(static_cast<data_type>(i * scale));
    }

    // perform FFT
    ::memcpy(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n2);
    thiz->fft_.execute();

    //
    // check
    //

    // input data array have to be preserved
    ASSERT_EQ(0, ::memcmp(&(thiz->in_[0]), &in_data[0], sizeof(complex_type) * n2));

    // for (int i = 0; i < n2; ++i) {
    //     std::cout << i << " : " << expected[i] << " - " << thiz->out_[i] << std::endl;
    // }

    for (int i = 0; i < n2; ++i) {
        ASSERT_AUTO_FLOATING_POINT_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0001));
    }
}

//
// PFFFT
//
#if CXXDASP_USE_FFT_BACKEND_PFFFT
typedef ForwardFFTTest<fft::backend::f::pffft, float> ForwardFFTTest_PFFFT_Float;
TEST_F(ForwardFFTTest_PFFFT_Float, forward) { do_forward_fft_test(this); }

typedef InverseFFTTest<fft::backend::f::pffft, float> InverseFFTTest_PFFFT_Float;
TEST_F(InverseFFTTest_PFFFT_Float, inverse) { do_inverse_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::pffft, float> ForwardRealFFTTest_PFFFT_Float;
TEST_F(ForwardRealFFTTest_PFFFT_Float, forward_real) { do_forward_real_fft_test(this); }

typedef InverseRealFFTTest<fft::backend::f::pffft, float> InverseRealFFTTest_PFFFT_Float;
TEST_F(InverseRealFFTTest_PFFFT_Float, inverse_real) { do_inverse_real_fft_test(this); }
#endif

//
// FFTS
//
#if CXXDASP_USE_FFT_BACKEND_FFTS
typedef ForwardFFTTest<fft::backend::f::ffts, float> ForwardFFTTest_FFTS_Float;
TEST_F(ForwardFFTTest_FFTS_Float, forward) { do_forward_fft_test(this); }

typedef InverseFFTTest<fft::backend::f::ffts, float> InverseFFTTest_FFTS_Float;
TEST_F(InverseFFTTest_FFTS_Float, inverse) { do_inverse_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::ffts, float> ForwardRealFFTTest_FFTS_Float;
TEST_F(ForwardRealFFTTest_FFTS_Float, forward_real) { do_forward_real_fft_test(this); }

typedef InverseRealFFTTest<fft::backend::f::ffts, float> InverseRealFFTTest_FFTS_Float;
TEST_F(InverseRealFFTTest_FFTS_Float, inverse_real) { do_inverse_real_fft_test(this); }
#endif

//
// Kiss FFT
//
#if CXXDASP_USE_FFT_BACKEND_KISS_FFT
typedef ForwardFFTTest<fft::backend::f::kiss_fft, float> ForwardFFTTest_KissFFT_Float;
TEST_F(ForwardFFTTest_KissFFT_Float, forward) { do_forward_fft_test(this); }

typedef InverseFFTTest<fft::backend::f::kiss_fft, float> InverseFFTTest_KissFFT_Float;
TEST_F(InverseFFTTest_KissFFT_Float, inverse) { do_inverse_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::kiss_fft, float> ForwardRealFFTTest_KissFFT_Float;
TEST_F(ForwardRealFFTTest_KissFFT_Float, forward_real) { do_forward_real_fft_test(this); }

typedef InverseRealFFTTest<fft::backend::f::kiss_fft, float> InverseRealFFTTest_KissFFT_Float;
TEST_F(InverseRealFFTTest_KissFFT_Float, inverse_real) { do_inverse_real_fft_test(this); }
#endif

//
// FFTWF
//
#if CXXDASP_USE_FFT_BACKEND_FFTWF
typedef ForwardFFTTest<fft::backend::f::fftw, float> ForwardFFTTest_FFTWF_Float;
TEST_F(ForwardFFTTest_FFTWF_Float, forward) { do_forward_fft_test(this); }

typedef InverseFFTTest<fft::backend::f::fftw, float> InverseFFTTest_FFTWF_Float;
TEST_F(InverseFFTTest_FFTWF_Float, inverse) { do_inverse_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::fftw, float> ForwardRealFFTTest_FFTWF_Float;
TEST_F(ForwardRealFFTTest_FFTWF_Float, forward_real) { do_forward_real_fft_test(this); }

typedef InverseRealFFTTest<fft::backend::f::fftw, float> InverseRealFFTTest_FFTWF_Float;
TEST_F(InverseRealFFTTest_FFTWF_Float, inverse_real) { do_inverse_real_fft_test(this); }
#endif

//
// Ne10
//
#if CXXDASP_USE_FFT_BACKEND_NE10
typedef ForwardFFTTest<fft::backend::f::ne10, float> ForwardFFTTest_Ne10_Float;
TEST_F(ForwardFFTTest_Ne10_Float, forward) { do_forward_fft_test(this); }

typedef InverseFFTTest<fft::backend::f::ne10, float> InverseFFTTest_Ne10_Float;
TEST_F(InverseFFTTest_Ne10_Float, inverse) { do_inverse_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::ne10, float> ForwardRealFFTTest_Ne10_Float;
TEST_F(ForwardRealFFTTest_Ne10_Float, forward_real) { do_forward_real_fft_test(this); }

typedef InverseRealFFTTest<fft::backend::f::ne10, float> InverseRealFFTTest_Ne10_Float;
TEST_F(InverseRealFFTTest_Ne10_Float, inverse_real) { do_inverse_real_fft_test(this); }
#endif

//
// Cricket FFT
//
#if CXXDASP_USE_FFT_BACKEND_CKFFT
typedef ForwardFFTTest<fft::backend::f::ckfft, float> ForwardFFTTest_CKFFT_Float;
TEST_F(ForwardFFTTest_CKFFT_Float, forward) { do_forward_fft_test(this); }

typedef InverseFFTTest<fft::backend::f::ckfft, float> InverseFFTTest_CKFFT_Float;
TEST_F(InverseFFTTest_CKFFT_Float, inverse) { do_inverse_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::ckfft, float> ForwardRealFFTTest_CKFFT_Float;
TEST_F(ForwardRealFFTTest_CKFFT_Float, forward_real) { do_forward_real_fft_test(this); }

typedef InverseRealFFTTest<fft::backend::f::ckfft, float> InverseRealFFTTest_CKFFT_Float;
TEST_F(InverseRealFFTTest_CKFFT_Float, inverse_real) { do_inverse_real_fft_test(this); }
#endif

//
// muFFT
//
#if CXXDASP_USE_FFT_BACKEND_MUFFT
typedef ForwardFFTTest<fft::backend::f::mufft, float> ForwardFFTTest_MUFFT_Float;
TEST_F(ForwardFFTTest_MUFFT_Float, forward) { do_forward_fft_test(this); }

typedef InverseFFTTest<fft::backend::f::mufft, float> InverseFFTTest_MUFFT_Float;
TEST_F(InverseFFTTest_MUFFT_Float, inverse) { do_inverse_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::mufft, float> ForwardRealFFTTest_MUFFT_Float;
TEST_F(ForwardRealFFTTest_MUFFT_Float, forward_real) { do_forward_real_fft_test(this); }

typedef InverseRealFFTTest<fft::backend::f::mufft, float> InverseRealFFTTest_MUFFT_Float;
TEST_F(InverseRealFFTTest_MUFFT_Float, inverse_real) { do_inverse_real_fft_test(this); }
#endif

//
// KFR
//
#if CXXDASP_USE_FFT_BACKEND_KFR_F
typedef ForwardFFTTest<fft::backend::f::kfr, float> ForwardFFTTest_KFR_Float;
TEST_F(ForwardFFTTest_KFR_Float, forward) { do_forward_fft_test(this); }

typedef InverseFFTTest<fft::backend::f::kfr, float> InverseFFTTest_KFR_Float;
TEST_F(InverseFFTTest_KFR_Float, inverse) { do_inverse_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::f::kfr, float> ForwardRealFFTTest_KFR_Float;
TEST_F(ForwardRealFFTTest_KFR_Float, forward_real) { do_forward_real_fft_test(this); }

typedef InverseRealFFTTest<fft::backend::f::kfr, float> InverseRealFFTTest_KFR_Float;
TEST_F(InverseRealFFTTest_KFR_Float, inverse_real) { do_inverse_real_fft_test(this); }
#endif

//
// FFTW
//
#if CXXDASP_USE_FFT_BACKEND_FFTW
typedef ForwardFFTTest<fft::backend::d::fftw, double> ForwardFFTTest_FFTW_Double;
TEST_F(ForwardFFTTest_FFTW_Double, forward) { do_forward_fft_test(this); }

typedef InverseFFTTest<fft::backend::d::fftw, double> InverseFFTTest_FFTW_Double;
TEST_F(InverseFFTTest_FFTW_Double, inverse) { do_inverse_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::d::fftw, double> ForwardRealFFTTest_FFTW_Double;
TEST_F(ForwardRealFFTTest_FFTW_Double, forward_real) { do_forward_real_fft_test(this); }

typedef InverseRealFFTTest<fft::backend::d::fftw, double> InverseRealFFTTest_FFTW_Double;
TEST_F(InverseRealFFTTest_FFTW_Double, inverse_real) { do_inverse_real_fft_test(this); }
#endif

//
// General purpose FFT package
//
#if CXXDASP_USE_FFT_BACKEND_GP_FFT
typedef ForwardFFTTest<fft::backend::d::gp_fft, double> ForwardFFTTest_GP_FFT_Double;
TEST_F(ForwardFFTTest_GP_FFT_Double, forward) { do_forward_fft_test(this); }

typedef InverseFFTTest<fft::backend::d::gp_fft, double> InverseFFTTest_GP_FFT_Double;
TEST_F(InverseFFTTest_GP_FFT_Double, inverse) { do_inverse_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::d::gp_fft, double> ForwardRealFFTTest_GP_FFT_Double;
TEST_F(ForwardRealFFTTest_GP_FFT_Double, forward_real) { do_forward_real_fft_test(this); }

typedef InverseRealFFTTest<fft::backend::d::gp_fft, double> InverseRealFFTTest_GP_FFT_Double;
TEST_F(InverseRealFFTTest_GP_FFT_Double, inverse_real) { do_inverse_real_fft_test(this); }
#endif

//
// KFR (double precision)
//
#if CXXDASP_USE_FFT_BACKEND_KFR_D
typedef ForwardFFTTest<fft::backend::d::kfr, double> ForwardFFTTest_KFR_Double;
TEST_F(ForwardFFTTest_KFR_Double, forward) { do_forward_fft_test(this); }

typedef InverseFFTTest<fft::backend::d::kfr, double> InverseFFTTest_KFR_Double;
TEST_F(InverseFFTTest_KFR_Double, inverse) { do_inverse_fft_test(this); }

typedef ForwardRealFFTTest<fft::backend::d::kfr, double> ForwardRealFFTTest_KFR_Double;
TEST_F(ForwardRealFFTTest_KFR_Double, forward_real) { do_forward_real_fft_test(this); }

typedef InverseRealFFTTest<fft::backend::d::kfr, double> InverseRealFFTTest_KFR_Double;
TEST_F(InverseRealFFTTest_KFR_Double, inverse_real) { do_inverse_real_fft_test(this); }
#endif
