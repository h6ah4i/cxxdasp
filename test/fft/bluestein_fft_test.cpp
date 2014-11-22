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
#include <cxxdasp/fft/bluestein_fft.hpp>

using namespace cxxdasp;

// #undef CXXDASP_USE_FFT_BACKEND_PFFFT
#undef CXXDASP_USE_FFT_BACKEND_FFTS
#undef CXXDASP_USE_FFT_BACKEND_KISS_FFT
#undef CXXDASP_USE_FFT_BACKEND_FFTWF
#undef CXXDASP_USE_FFT_BACKEND_NE10
#undef CXXDASP_USE_FFT_BACKEND_FFTW
#undef CXXDASP_USE_FFT_BACKEND_GP_FFT

#if CXXDASP_USE_FFT_BACKEND_PFFFT
typedef fft::backend::f::pffft sp_fft_backend_class;
#elif CXXDASP_USE_FFT_BACKEND_FFTS
typedef fft::backend::f::ffts sp_fft_backend_class;
#elif CXXDASP_USE_FFT_BACKEND_KISS_FFT
typedef fft::backend::f::kiss_fft sp_fft_backend_class;
#elif CXXDASP_USE_FFT_BACKEND_FFTWF
typedef fft::backend::f::fftw sp_fft_backend_class;
#elif CXXDASP_USE_FFT_BACKEND_NE10
typedef fft::backend::f::ne10 sp_fft_backend_class;
#else
#define SINGLE_PRECISION_FFT_IS_NOT_AVAILABLE
#endif

#if CXXDASP_USE_FFT_BACKEND_FFTW
typedef fft::backend::d::fftw dp_fft_backend_class;
#elif CXXDASP_USE_FFT_BACKEND_GP_FFT
typedef fft::backend::d::gp_fft dp_fft_backend_class;
#else
#define DOUBLE_PRECISION_FFT_IS_NOT_AVAILABLE
#endif

template <typename TFFTBackend, typename T>
class BluesteinForwardFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef fft::bluestein::forward<std::complex<T>, std::complex<T>, TFFTBackend> fft_backend_type;
    typedef T data_type;

    fft::fft<std::complex<T>, std::complex<T>, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<std::complex<T>> in_;
    cxxporthelper::aligned_memory<std::complex<T>> out_;
};

template <typename TFFTBackend, typename T>
class BluesteinBackwardFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef fft::bluestein::backward<std::complex<T>, std::complex<T>, TFFTBackend> fft_backend_type;
    typedef T data_type;

    fft::fft<std::complex<T>, std::complex<T>, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<std::complex<T>> in_;
    cxxporthelper::aligned_memory<std::complex<T>> out_;
};

template <typename TFFTBackend, typename T>
class BluesteinForwardRealFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef fft::bluestein::forward_real<T, std::complex<T>, TFFTBackend> fft_backend_type;
    typedef T data_type;

    fft::fft<T, std::complex<T>, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<T> in_;
    cxxporthelper::aligned_memory<std::complex<T>> out_;
};

template <typename TFFTBackend, typename T>
class BluesteinBackwardRealFFTTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}

public:
    typedef fft::bluestein::backward_real<std::complex<T>, T, TFFTBackend> fft_backend_type;
    typedef T data_type;

    fft::fft<std::complex<T>, T, fft_backend_type> fft_;
    cxxporthelper::aligned_memory<std::complex<T>> in_;
    cxxporthelper::aligned_memory<T> out_;
};

template <typename TBluesteinForwardFFTTest>
void do_pot_forward_fft_test(TBluesteinForwardFFTTest *thiz)
{
    typedef typename TBluesteinForwardFFTTest::data_type data_type;
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
    ASSERT_EQ(1, thiz->fft_.scale());

#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    expected.push_back(COMPLEX_VALUE(136.0, 136.0));
    expected.push_back(COMPLEX_VALUE(-48.218715937006785, 32.218715937006785));
    expected.push_back(COMPLEX_VALUE(-27.31370849898476, 11.313708498984761));
    expected.push_back(COMPLEX_VALUE(-19.97284610132391, 3.9728461013239116));
    expected.push_back(COMPLEX_VALUE(-16.0, 0.0));
    expected.push_back(COMPLEX_VALUE(-13.34542910335439, -2.6545708966456107));
    expected.push_back(COMPLEX_VALUE(-11.313708498984761, -4.686291501015239));
    expected.push_back(COMPLEX_VALUE(-9.591298939037264, -6.408701060962734));
    expected.push_back(COMPLEX_VALUE(-8.0, -8.0));
    expected.push_back(COMPLEX_VALUE(-6.408701060962734, -9.591298939037264));
    expected.push_back(COMPLEX_VALUE(-4.686291501015239, -11.313708498984761));
    expected.push_back(COMPLEX_VALUE(-2.6545708966456107, -13.34542910335439));
    expected.push_back(COMPLEX_VALUE(0.0, -16.0));
    expected.push_back(COMPLEX_VALUE(3.9728461013239116, -19.97284610132391));
    expected.push_back(COMPLEX_VALUE(11.313708498984761, -27.31370849898476));
    expected.push_back(COMPLEX_VALUE(32.218715937006785, -48.218715937006785));
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
        ASSERT_AUTO_COMPLEX_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0005));
    }
}

template <typename TBluesteinBackwardFFTTest>
void do_pot_backward_fft_test(TBluesteinBackwardFFTTest *thiz)
{
    typedef typename TBluesteinBackwardFFTTest::fft_backend_type fft_backend_type;
    typedef typename TBluesteinBackwardFFTTest::data_type data_type;
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
        ASSERT_AUTO_COMPLEX_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0005));
    }
}

template <typename TBluesteinForwardFFTTest>
void do_pot_forward_real_fft_test(TBluesteinForwardFFTTest *thiz)
{
    typedef typename TBluesteinForwardFFTTest::data_type data_type;
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
    ASSERT_EQ(1, thiz->fft_.scale());
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    expected.push_back(COMPLEX_VALUE(528.0, 0.0));
    expected.push_back(COMPLEX_VALUE(-16.0, 162.45072620174176));
    expected.push_back(COMPLEX_VALUE(-16.0, 80.43743187401357));
    expected.push_back(COMPLEX_VALUE(-16.0, 52.74493134301312));
    expected.push_back(COMPLEX_VALUE(-16.0, 38.62741699796952));
    expected.push_back(COMPLEX_VALUE(-16.0, 29.933894588630228));
    expected.push_back(COMPLEX_VALUE(-16.0, 23.945692202647823));
    expected.push_back(COMPLEX_VALUE(-16.0, 19.496056409407625));
    expected.push_back(COMPLEX_VALUE(-16.0, 16.0));
    expected.push_back(COMPLEX_VALUE(-16.0, 13.130860653258562));
    expected.push_back(COMPLEX_VALUE(-16.0, 10.690858206708779));
    expected.push_back(COMPLEX_VALUE(-16.0, 8.55217817521267));
    expected.push_back(COMPLEX_VALUE(-16.0, 6.627416997969522));
    expected.push_back(COMPLEX_VALUE(-16.0, 4.8535469377174785));
    expected.push_back(COMPLEX_VALUE(-16.0, 3.1825978780745316));
    expected.push_back(COMPLEX_VALUE(-16.0, 1.5758624537146346));
    expected.push_back(COMPLEX_VALUE(-16.0, 0.0));
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
        ASSERT_AUTO_COMPLEX_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0005));
    }
}

template <typename TBluesteinBackwardFFTTest>
void do_pot_backward_real_fft_test(TBluesteinBackwardFFTTest *thiz)
{
    typedef typename TBluesteinBackwardFFTTest::fft_backend_type fft_backend_type;
    typedef typename TBluesteinBackwardFFTTest::data_type data_type;
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
        ASSERT_AUTO_FLOATING_POINT_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0005));
    }
}

template <typename TBluesteinForwardFFTTest>
void do_npot_forward_fft_test(TBluesteinForwardFFTTest *thiz)
{
    typedef typename TBluesteinForwardFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 17;
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
    ASSERT_EQ(1, thiz->fft_.scale());
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    expected.push_back(COMPLEX_VALUE(153.0, 153.0));
    expected.push_back(COMPLEX_VALUE(-53.9709837968331, 36.9709837968331));
    expected.push_back(COMPLEX_VALUE(-30.441029210707647, 13.441029210707654));
    expected.push_back(COMPLEX_VALUE(-22.227971357603728, 5.22797135760373));
    expected.push_back(COMPLEX_VALUE(-17.824055833840482, 0.8240558338404878));
    expected.push_back(COMPLEX_VALUE(-14.918902041352606, -2.0810979586473826));
    expected.push_back(COMPLEX_VALUE(-12.732497088174021, -4.2675029118259715));
    expected.push_back(COMPLEX_VALUE(-10.918458513893814, -6.081541486106182));
    expected.push_back(COMPLEX_VALUE(-9.28764099305725, -7.712359006942743));
    expected.push_back(COMPLEX_VALUE(-7.712359006942743, -9.28764099305725));
    expected.push_back(COMPLEX_VALUE(-6.081541486106182, -10.918458513893814));
    expected.push_back(COMPLEX_VALUE(-4.2675029118259715, -12.732497088174021));
    expected.push_back(COMPLEX_VALUE(-2.0810979586473826, -14.918902041352606));
    expected.push_back(COMPLEX_VALUE(0.8240558338404878, -17.824055833840482));
    expected.push_back(COMPLEX_VALUE(5.22797135760373, -22.227971357603728));
    expected.push_back(COMPLEX_VALUE(13.441029210707654, -30.441029210707647));
    expected.push_back(COMPLEX_VALUE(36.9709837968331, -53.9709837968331));
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
        ASSERT_AUTO_COMPLEX_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0005));
    }
}

template <typename TBluesteinBackwardFFTTest>
void do_npot_backward_fft_test(TBluesteinBackwardFFTTest *thiz)
{
    typedef typename TBluesteinBackwardFFTTest::fft_backend_type fft_backend_type;
    typedef typename TBluesteinBackwardFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 17;
    // setup
    thiz->in_.allocate(n);
    thiz->out_.allocate(n);
    thiz->fft_.setup(n, &(thiz->in_[0]), &(thiz->out_[0]));

    std::vector<complex_type> expected;
    std::vector<complex_type> in_data;

// in_data
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    in_data.push_back(COMPLEX_VALUE(153.0, 153.0));
    in_data.push_back(COMPLEX_VALUE(-53.9709837968331, 36.9709837968331));
    in_data.push_back(COMPLEX_VALUE(-30.441029210707647, 13.441029210707654));
    in_data.push_back(COMPLEX_VALUE(-22.227971357603728, 5.22797135760373));
    in_data.push_back(COMPLEX_VALUE(-17.824055833840482, 0.8240558338404878));
    in_data.push_back(COMPLEX_VALUE(-14.918902041352606, -2.0810979586473826));
    in_data.push_back(COMPLEX_VALUE(-12.732497088174021, -4.2675029118259715));
    in_data.push_back(COMPLEX_VALUE(-10.918458513893814, -6.081541486106182));
    in_data.push_back(COMPLEX_VALUE(-9.28764099305725, -7.712359006942743));
    in_data.push_back(COMPLEX_VALUE(-7.712359006942743, -9.28764099305725));
    in_data.push_back(COMPLEX_VALUE(-6.081541486106182, -10.918458513893814));
    in_data.push_back(COMPLEX_VALUE(-4.2675029118259715, -12.732497088174021));
    in_data.push_back(COMPLEX_VALUE(-2.0810979586473826, -14.918902041352606));
    in_data.push_back(COMPLEX_VALUE(0.8240558338404878, -17.824055833840482));
    in_data.push_back(COMPLEX_VALUE(5.22797135760373, -22.227971357603728));
    in_data.push_back(COMPLEX_VALUE(13.441029210707654, -30.441029210707647));
    in_data.push_back(COMPLEX_VALUE(36.9709837968331, -53.9709837968331));
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
        ASSERT_AUTO_COMPLEX_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0005));
    }
}

template <typename TBluesteinForwardFFTTest>
void do_npot_forward_real_fft_test(TBluesteinForwardFFTTest *thiz)
{
    typedef typename TBluesteinForwardFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 33;
    const int n2 = utils::forward_fft_real_num_outputs(n);

    ASSERT_EQ(((33 + 1) / 2), n2);

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
    ASSERT_EQ(1, thiz->fft_.scale());
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    expected.push_back(COMPLEX_VALUE(561.0, 0));
    expected.push_back(COMPLEX_VALUE(-16.500000000000004, 172.79581761986876));
    expected.push_back(COMPLEX_VALUE(-16.500000000000014, 85.6101293261771));
    expected.push_back(COMPLEX_VALUE(-16.500000000000007, 56.19383944167262));
    expected.push_back(COMPLEX_VALUE(-16.5, 41.215007492617985));
    expected.push_back(COMPLEX_VALUE(-16.5, 32.00553483355687));
    expected.push_back(COMPLEX_VALUE(-16.5, 25.674501154094784));
    expected.push_back(COMPLEX_VALUE(-16.499999999999993, 20.981456797309058));
    expected.push_back(COMPLEX_VALUE(-16.500000000000007, 17.304701968961716));
    expected.push_back(COMPLEX_VALUE(-16.499999999999996, 14.297331386960533));
    expected.push_back(COMPLEX_VALUE(-16.500000000000004, 11.749596810259547));
    expected.push_back(COMPLEX_VALUE(-16.5, 9.526279441628825));
    expected.push_back(COMPLEX_VALUE(-16.500000000000004, 7.535297515408658));
    expected.push_back(COMPLEX_VALUE(-16.50000000000001, 5.710705523067798));
    expected.push_back(COMPLEX_VALUE(-16.500000000000007, 4.002856688170704));
    expected.push_back(COMPLEX_VALUE(-16.500000000000014, 2.372341850917053));
    expected.push_back(COMPLEX_VALUE(-16.50000000000002, 0.7859918732890918));
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
        ASSERT_AUTO_COMPLEX_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0005));
    }
}

template <typename TBluesteinBackwardFFTTest>
void do_npot_backward_real_fft_test(TBluesteinBackwardFFTTest *thiz)
{
    typedef typename TBluesteinBackwardFFTTest::fft_backend_type fft_backend_type;
    typedef typename TBluesteinBackwardFFTTest::data_type data_type;
    typedef std::complex<data_type> complex_type;

    const int n = 33;
    const int n2 = utils::forward_fft_real_num_outputs(n);

    ASSERT_EQ(((33 + 1) / 2), n2);

    // setup
    thiz->in_.allocate(n2);
    thiz->out_.allocate(n);
    thiz->fft_.setup(n, &(thiz->in_[0]), &(thiz->out_[0]));

    std::vector<data_type> expected;
    std::vector<complex_type> in_data;

// in_data
#define COMPLEX_VALUE(real, imag) complex_type(static_cast<data_type>(real), static_cast<data_type>(imag))
    in_data.push_back(COMPLEX_VALUE(561.0, 0));
    in_data.push_back(COMPLEX_VALUE(-16.500000000000004, 172.79581761986876));
    in_data.push_back(COMPLEX_VALUE(-16.500000000000014, 85.6101293261771));
    in_data.push_back(COMPLEX_VALUE(-16.500000000000007, 56.19383944167262));
    in_data.push_back(COMPLEX_VALUE(-16.5, 41.215007492617985));
    in_data.push_back(COMPLEX_VALUE(-16.5, 32.00553483355687));
    in_data.push_back(COMPLEX_VALUE(-16.5, 25.674501154094784));
    in_data.push_back(COMPLEX_VALUE(-16.499999999999993, 20.981456797309058));
    in_data.push_back(COMPLEX_VALUE(-16.500000000000007, 17.304701968961716));
    in_data.push_back(COMPLEX_VALUE(-16.499999999999996, 14.297331386960533));
    in_data.push_back(COMPLEX_VALUE(-16.500000000000004, 11.749596810259547));
    in_data.push_back(COMPLEX_VALUE(-16.5, 9.526279441628825));
    in_data.push_back(COMPLEX_VALUE(-16.500000000000004, 7.535297515408658));
    in_data.push_back(COMPLEX_VALUE(-16.50000000000001, 5.710705523067798));
    in_data.push_back(COMPLEX_VALUE(-16.500000000000007, 4.002856688170704));
    in_data.push_back(COMPLEX_VALUE(-16.500000000000014, 2.372341850917053));
    in_data.push_back(COMPLEX_VALUE(-16.50000000000002, 0.7859918732890918));
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
        ASSERT_AUTO_FLOATING_POINT_NEAR(expected[i], thiz->out_[i], static_cast<data_type>(0.0005));
    }
}

//
// PFFFT
//
#if CXXDASP_USE_FFT_BACKEND_PFFFT
typedef BluesteinForwardFFTTest<fft::backend::f::pffft, float> BluesteinForwardFFTTest_PFFFT_Float;
TEST_F(BluesteinForwardFFTTest_PFFFT_Float, forward_pot) { do_pot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::f::pffft, float> BluesteinBackwardFFTTest_PFFFT_Float;
TEST_F(BluesteinBackwardFFTTest_PFFFT_Float, backward_pot) { do_pot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::f::pffft, float> BluesteinForwardRealFFTTest_PFFFT_Float;
TEST_F(BluesteinForwardRealFFTTest_PFFFT_Float, forward_real_pot) { do_pot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::f::pffft, float> BluesteinBackwardRealFFTTest_PFFFT_Float;
TEST_F(BluesteinBackwardRealFFTTest_PFFFT_Float, backward_real_pot) { do_pot_backward_real_fft_test(this); }

typedef BluesteinForwardFFTTest<fft::backend::f::pffft, float> BluesteinForwardFFTTest_PFFFT_Float;
TEST_F(BluesteinForwardFFTTest_PFFFT_Float, forward_npot) { do_npot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::f::pffft, float> BluesteinBackwardFFTTest_PFFFT_Float;
TEST_F(BluesteinBackwardFFTTest_PFFFT_Float, backward_npot) { do_npot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::f::pffft, float> BluesteinForwardRealFFTTest_PFFFT_Float;
TEST_F(BluesteinForwardRealFFTTest_PFFFT_Float, forward_real_npot) { do_npot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::f::pffft, float> BluesteinBackwardRealFFTTest_PFFFT_Float;
TEST_F(BluesteinBackwardRealFFTTest_PFFFT_Float, backward_real_npot) { do_npot_backward_real_fft_test(this); }
#endif

//
// FFTS
//
#if CXXDASP_USE_FFT_BACKEND_FFTS
typedef BluesteinForwardFFTTest<fft::backend::f::ffts, float> BluesteinForwardFFTTest_FFTS_Float;
TEST_F(BluesteinForwardFFTTest_FFTS_Float, forward_pot) { do_pot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::f::ffts, float> BluesteinBackwardFFTTest_FFTS_Float;
TEST_F(BluesteinBackwardFFTTest_FFTS_Float, backward_pot) { do_pot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::f::ffts, float> BluesteinForwardRealFFTTest_FFTS_Float;
TEST_F(BluesteinForwardRealFFTTest_FFTS_Float, forward_real_pot) { do_pot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::f::ffts, float> BluesteinBackwardRealFFTTest_FFTS_Float;
TEST_F(BluesteinBackwardRealFFTTest_FFTS_Float, backward_real_pot) { do_pot_backward_real_fft_test(this); }
typedef BluesteinForwardFFTTest<fft::backend::f::ffts, float> BluesteinForwardFFTTest_FFTS_Float;
TEST_F(BluesteinForwardFFTTest_FFTS_Float, forward_npot) { do_npot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::f::ffts, float> BluesteinBackwardFFTTest_FFTS_Float;
TEST_F(BluesteinBackwardFFTTest_FFTS_Float, backward_npot) { do_npot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::f::ffts, float> BluesteinForwardRealFFTTest_FFTS_Float;
TEST_F(BluesteinForwardRealFFTTest_FFTS_Float, forward_real_npot) { do_npot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::f::ffts, float> BluesteinBackwardRealFFTTest_FFTS_Float;
TEST_F(BluesteinBackwardRealFFTTest_FFTS_Float, backward_real_npot) { do_npot_backward_real_fft_test(this); }
#endif

//
// Kiss FFT
//
#if CXXDASP_USE_FFT_BACKEND_KISS_FFT
typedef BluesteinForwardFFTTest<fft::backend::f::kiss_fft, float> BluesteinForwardFFTTest_KissFFT_Float;
TEST_F(BluesteinForwardFFTTest_KissFFT_Float, forward_pot) { do_pot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::f::kiss_fft, float> BluesteinBackwardFFTTest_KissFFT_Float;
TEST_F(BluesteinBackwardFFTTest_KissFFT_Float, backward_pot) { do_pot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::f::kiss_fft, float> BluesteinForwardRealFFTTest_KissFFT_Float;
TEST_F(BluesteinForwardRealFFTTest_KissFFT_Float, forward_real_pot) { do_pot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::f::kiss_fft, float> BluesteinBackwardRealFFTTest_KissFFT_Float;
TEST_F(BluesteinBackwardRealFFTTest_KissFFT_Float, backward_real_pot) { do_pot_backward_real_fft_test(this); }
typedef BluesteinForwardFFTTest<fft::backend::f::kiss_fft, float> BluesteinForwardFFTTest_KissFFT_Float;
TEST_F(BluesteinForwardFFTTest_KissFFT_Float, forward_npot) { do_npot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::f::kiss_fft, float> BluesteinBackwardFFTTest_KissFFT_Float;
TEST_F(BluesteinBackwardFFTTest_KissFFT_Float, backward_npot) { do_npot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::f::kiss_fft, float> BluesteinForwardRealFFTTest_KissFFT_Float;
TEST_F(BluesteinForwardRealFFTTest_KissFFT_Float, forward_real_npot) { do_npot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::f::kiss_fft, float> BluesteinBackwardRealFFTTest_KissFFT_Float;
TEST_F(BluesteinBackwardRealFFTTest_KissFFT_Float, backward_real_npot) { do_npot_backward_real_fft_test(this); }
#endif

//
// FFTWF
//
#if CXXDASP_USE_FFT_BACKEND_FFTWF
typedef BluesteinForwardFFTTest<fft::backend::f::fftw, float> BluesteinForwardFFTTest_FFTWF_Float;
TEST_F(BluesteinForwardFFTTest_FFTWF_Float, forward_pot) { do_pot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::f::fftw, float> BluesteinBackwardFFTTest_FFTWF_Float;
TEST_F(BluesteinBackwardFFTTest_FFTWF_Float, backward_pot) { do_pot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::f::fftw, float> BluesteinForwardRealFFTTest_FFTWF_Float;
TEST_F(BluesteinForwardRealFFTTest_FFTWF_Float, forward_real_pot) { do_pot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::f::fftw, float> BluesteinBackwardRealFFTTest_FFTWF_Float;
TEST_F(BluesteinBackwardRealFFTTest_FFTWF_Float, backward_real_pot) { do_pot_backward_real_fft_test(this); }
typedef BluesteinForwardFFTTest<fft::backend::f::fftw, float> BluesteinForwardFFTTest_FFTWF_Float;
TEST_F(BluesteinForwardFFTTest_FFTWF_Float, forward_npot) { do_npot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::f::fftw, float> BluesteinBackwardFFTTest_FFTWF_Float;
TEST_F(BluesteinBackwardFFTTest_FFTWF_Float, backward_npot) { do_npot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::f::fftw, float> BluesteinForwardRealFFTTest_FFTWF_Float;
TEST_F(BluesteinForwardRealFFTTest_FFTWF_Float, forward_real_npot) { do_npot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::f::fftw, float> BluesteinBackwardRealFFTTest_FFTWF_Float;
TEST_F(BluesteinBackwardRealFFTTest_FFTWF_Float, backward_real_npot) { do_npot_backward_real_fft_test(this); }
#endif

//
// Ne10
//
#if CXXDASP_USE_FFT_BACKEND_NE10
typedef BluesteinForwardFFTTest<fft::backend::f::ne10, float> BluesteinForwardFFTTest_Ne10_Float;
TEST_F(BluesteinForwardFFTTest_Ne10_Float, forward_pot) { do_pot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::f::ne10, float> BluesteinBackwardFFTTest_Ne10_Float;
TEST_F(BluesteinBackwardFFTTest_Ne10_Float, backward_pot) { do_pot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::f::ne10, float> BluesteinForwardRealFFTTest_Ne10_Float;
TEST_F(BluesteinForwardRealFFTTest_Ne10_Float, forward_real_pot) { do_pot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::f::ne10, float> BluesteinBackwardRealFFTTest_Ne10_Float;
TEST_F(BluesteinBackwardRealFFTTest_Ne10_Float, backward_real_pot) { do_pot_backward_real_fft_test(this); }
typedef BluesteinForwardFFTTest<fft::backend::f::ne10, float> BluesteinForwardFFTTest_Ne10_Float;
TEST_F(BluesteinForwardFFTTest_Ne10_Float, forward_npot) { do_npot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::f::ne10, float> BluesteinBackwardFFTTest_Ne10_Float;
TEST_F(BluesteinBackwardFFTTest_Ne10_Float, backward_npot) { do_npot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::f::ne10, float> BluesteinForwardRealFFTTest_Ne10_Float;
TEST_F(BluesteinForwardRealFFTTest_Ne10_Float, forward_real_npot) { do_npot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::f::ne10, float> BluesteinBackwardRealFFTTest_Ne10_Float;
TEST_F(BluesteinBackwardRealFFTTest_Ne10_Float, backward_real_npot) { do_npot_backward_real_fft_test(this); }
#endif

//
// FFTW
//
#if CXXDASP_USE_FFT_BACKEND_FFTW
typedef BluesteinForwardFFTTest<fft::backend::d::fftw, double> BluesteinForwardFFTTest_FFTW_Double;
TEST_F(BluesteinForwardFFTTest_FFTW_Double, forward_pot) { do_pot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::d::fftw, double> BluesteinBackwardFFTTest_FFTW_Double;
TEST_F(BluesteinBackwardFFTTest_FFTW_Double, backward_pot) { do_pot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::d::fftw, double> BluesteinForwardRealFFTTest_FFTW_Double;
TEST_F(BluesteinForwardRealFFTTest_FFTW_Double, forward_real_pot) { do_pot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::d::fftw, double> BluesteinBackwardRealFFTTest_FFTW_Double;
TEST_F(BluesteinBackwardRealFFTTest_FFTW_Double, backward_real_pot) { do_pot_backward_real_fft_test(this); }
typedef BluesteinForwardFFTTest<fft::backend::d::fftw, double> BluesteinForwardFFTTest_FFTW_Double;
TEST_F(BluesteinForwardFFTTest_FFTW_Double, forward_npot) { do_npot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::d::fftw, double> BluesteinBackwardFFTTest_FFTW_Double;
TEST_F(BluesteinBackwardFFTTest_FFTW_Double, backward_npot) { do_npot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::d::fftw, double> BluesteinForwardRealFFTTest_FFTW_Double;
TEST_F(BluesteinForwardRealFFTTest_FFTW_Double, forward_real_npot) { do_npot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::d::fftw, double> BluesteinBackwardRealFFTTest_FFTW_Double;
TEST_F(BluesteinBackwardRealFFTTest_FFTW_Double, backward_real_npot) { do_npot_backward_real_fft_test(this); }
#endif

//
// General purpose FFT package
//
#if CXXDASP_USE_FFT_BACKEND_GP_FFT
typedef BluesteinForwardFFTTest<fft::backend::d::gp_fft, double> BluesteinForwardFFTTest_GP_FFT_Double;
TEST_F(BluesteinForwardFFTTest_GP_FFT_Double, forward_pot) { do_pot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::d::gp_fft, double> BluesteinBackwardFFTTest_GP_FFT_Double;
TEST_F(BluesteinBackwardFFTTest_GP_FFT_Double, backward_pot) { do_pot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::d::gp_fft, double> BluesteinForwardRealFFTTest_GP_FFT_Double;
TEST_F(BluesteinForwardRealFFTTest_GP_FFT_Double, forward_real_pot) { do_pot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::d::gp_fft, double> BluesteinBackwardRealFFTTest_GP_FFT_Double;
TEST_F(BluesteinBackwardRealFFTTest_GP_FFT_Double, backward_real_pot) { do_pot_backward_real_fft_test(this); }
typedef BluesteinForwardFFTTest<fft::backend::d::gp_fft, double> BluesteinForwardFFTTest_GP_FFT_Double;
TEST_F(BluesteinForwardFFTTest_GP_FFT_Double, forward_npot) { do_npot_forward_fft_test(this); }

typedef BluesteinBackwardFFTTest<fft::backend::d::gp_fft, double> BluesteinBackwardFFTTest_GP_FFT_Double;
TEST_F(BluesteinBackwardFFTTest_GP_FFT_Double, backward_npot) { do_npot_backward_fft_test(this); }

typedef BluesteinForwardRealFFTTest<fft::backend::d::gp_fft, double> BluesteinForwardRealFFTTest_GP_FFT_Double;
TEST_F(BluesteinForwardRealFFTTest_GP_FFT_Double, forward_real_npot) { do_npot_forward_real_fft_test(this); }

typedef BluesteinBackwardRealFFTTest<fft::backend::d::gp_fft, double> BluesteinBackwardRealFFTTest_GP_FFT_Double;
TEST_F(BluesteinBackwardRealFFTTest_GP_FFT_Double, backward_real_npot) { do_npot_backward_real_fft_test(this); }
#endif
