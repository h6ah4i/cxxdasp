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

#include <limits>
#include <random>

#include "test_common.hpp"

#include <cxxporthelper/cstdint>
#include <cxxdasp/converter/sample_format_converter.hpp>
#include <cxxdasp/converter/sample_format_converter_core_operators.hpp>

using namespace cxxdasp;

class SampleFormatConverterTest : public ::testing::Test {
protected:
    virtual void SetUp() { cxxdasp_init(); }
    virtual void TearDown() {}
};

template <typename TSrc, typename TDest>
struct sample_converter {
    // static TDest convert(TSrc x);
};

// F64 to F64
template <>
struct sample_converter<double, double> {
    static double convert(double x) { return x; }

    static double convert_fast(double x) { return x; }
};

// F64 to F32
template <>
struct sample_converter<double, float> {
    static float convert(double x) { return static_cast<float>(x); }

    static float convert_fast(double x) { return static_cast<float>(x); }
};

// F64 to S32
template <>
struct sample_converter<double, int32_t> {
    static int32_t convert(double x)
    {
        const double scale = static_cast<double>((std::numeric_limits<int32_t>::max)());
        return static_cast<int32_t>((std::min)((std::max)(x, -1.0), 1.0) * scale);
    }

    static int32_t convert_fast(double x)
    {
        const double scale = static_cast<double>((std::numeric_limits<int32_t>::max)()) + 1;
        const double min_val = static_cast<double>((std::numeric_limits<int32_t>::min)());
        const double max_val = static_cast<double>((std::numeric_limits<int32_t>::max)());
        return static_cast<int32_t>((std::min)((std::max)((x * scale), min_val), max_val));
    }
};

// F64 to S16
template <>
struct sample_converter<double, int16_t> {
    static int16_t convert(double x)
    {
        const double scale = (std::numeric_limits<int16_t>::max)();
        return static_cast<int16_t>((std::min)((std::max)(x, -1.0), 1.0) * scale);
    }

    static int32_t convert_fast(double x)
    {
        const double scale = static_cast<double>((std::numeric_limits<int16_t>::max)()) + 1;
        const double min_val = static_cast<double>((std::numeric_limits<int16_t>::min)());
        const double max_val = static_cast<double>((std::numeric_limits<int16_t>::max)());
        return static_cast<int16_t>((std::min)((std::max)((x * scale), min_val), max_val));
    }
};

// F32 to S32
template <>
struct sample_converter<float, int32_t> {
    static int32_t convert(float x)
    {
        const float scale = (std::numeric_limits<int32_t>::max)();
        return static_cast<int32_t>((std::min)((std::max)(x, -1.0f), 1.0f) * scale);
    }

    static int32_t convert_fast(float x)
    {
        const double scale = static_cast<double>((std::numeric_limits<int32_t>::max)()) + 1;
        const double min_val = static_cast<double>((std::numeric_limits<int32_t>::min)());
        const double max_val = static_cast<double>((std::numeric_limits<int32_t>::max)());
        return static_cast<int32_t>((std::min)((std::max)((x * scale), min_val), max_val));
    }
};

// F32 to S16
template <>
struct sample_converter<float, int16_t> {
    static int16_t convert(float x)
    {
        const float scale = (std::numeric_limits<int16_t>::max)();
        return static_cast<int16_t>((std::min)((std::max)(x, -1.0f), 1.0f) * scale);
    }

    static int16_t convert_fast(float x)
    {
        const float scale = static_cast<float>((std::numeric_limits<int16_t>::max)()) + 1;
        const float min_val = static_cast<float>((std::numeric_limits<int16_t>::min)());
        const float max_val = static_cast<float>((std::numeric_limits<int16_t>::max)());
        return static_cast<int16_t>((std::min)((std::max)((x * scale), min_val), max_val));
    }
};

// S32 to F32
template <>
struct sample_converter<int32_t, float> {
    static float convert(int32_t x)
    {
        const double inv_scale = static_cast<double>((std::numeric_limits<int32_t>::max)());
        const float scale = static_cast<float>(1.0 / inv_scale);
        return static_cast<float>(x * scale);
    }

    static float convert_fast(int32_t x)
    {
        const double inv_scale = static_cast<double>((std::numeric_limits<int32_t>::max)()) + 1;
        const float scale = static_cast<float>(1.0 / inv_scale);
        return static_cast<float>(x * scale);
    }
};

// S16 to F32
template <>
struct sample_converter<int16_t, float> {
    static float convert(int16_t x)
    {
        const double inv_scale = static_cast<double>((std::numeric_limits<int16_t>::max)());
        const float scale = static_cast<float>(1.0 / inv_scale);
        return static_cast<float>(x * scale);
    }

    static float convert_fast(int16_t x)
    {
        const double inv_scale = static_cast<double>((std::numeric_limits<int16_t>::max)()) + 1;
        const float scale = static_cast<float>(1.0 / inv_scale);
        return static_cast<float>(x * scale);
    }
};

// S32 to S16
template <>
struct sample_converter<int32_t, int16_t> {
    static int16_t convert(int32_t x) { return static_cast<int16_t>(x / 65536); }

    static int16_t convert_fast(int32_t x) { return static_cast<int16_t>(x / 65536); }
};

// S16 to S32
template <>
struct sample_converter<int16_t, int32_t> {
    static int32_t convert(int16_t x) { return static_cast<int32_t>(x * 65536); }

    static int32_t convert_fast(int16_t x) { return static_cast<int32_t>(x * 65536); }
};

template <typename TSrc, int NSrcChannels, typename TDest, int NDestChannels>
datatype::audio_frame<TDest, NDestChannels> convert_audio_frame(const datatype::audio_frame<TSrc, NSrcChannels> &x,
                                                                bool use_fast_mode_conversion)
{
    datatype::audio_frame<TDest, NDestChannels> t;

    for (int ch = 0; ch < NDestChannels; ch++) {
        if (!use_fast_mode_conversion) {
            t.c(ch) = sample_converter<TSrc, TDest>::convert(x.c((std::min)(ch, NSrcChannels - 1)));
        } else {
            t.c(ch) = sample_converter<TSrc, TDest>::convert_fast(x.c((std::min)(ch, NSrcChannels - 1)));
        }
    }

    return t;
}

template <typename TSrc, int NSrcChannels, typename TDest, int NDestChannels>
void convert_audio_frames(const std::vector<datatype::audio_frame<TSrc, NSrcChannels>> &src,
                          std::vector<datatype::audio_frame<TDest, NDestChannels>> &dest, int n,
                          bool use_fast_mode_conversion)
{

    dest.resize(n);
    for (int i = 0; i < n; i++) {
        dest[i] = convert_audio_frame<TSrc, NSrcChannels, TDest, NDestChannels>(src[i], use_fast_mode_conversion);
    }
}

template <int NChannels>
void make_double_precision_random_data(std::vector<datatype::audio_frame<double, NChannels>> &data, int n)
{
    std::mt19937 mt(0);
    std::uniform_real_distribution<double> gen(-1.0, 1.0);

    data.resize(n);
    for (int i = 0; i < n; ++i) {
        for (int ch = 0; ch < NChannels; ++ch) {
            data[i].c(ch) = gen(mt);
        }
    }
}

template <typename T>
struct sample_comparator {
    static bool compare(const T &a, const T &b) { return a == b; }
};

template <>
struct sample_comparator<double> {
    static bool compare(const double &a, const double &b)
    {
        const double tolerance = 1e-12;
        return (std::abs(a - b) <= tolerance);
    }
};

template <>
struct sample_comparator<float> {
    static bool compare(const float &a, const float &b)
    {
        const float tolerance = 1e-6;
        return (std::abs(a - b) <= tolerance);
    }
};

template <>
struct sample_comparator<int32_t> {
    static bool compare(const int32_t &a, const int32_t &b)
    {
        const int32_t tolerance = 1;
        return (std::abs(a - b) <= tolerance);
    }
};

template <>
struct sample_comparator<int16_t> {
    static bool compare(const int16_t &a, const int16_t &b)
    {
        const int16_t tolerance = 1;
        return (std::abs(a - b) <= tolerance);
    }
};

template <typename TSrc, typename TDest>
struct sample_clipping_checker {
    // static bool is_positive_clipped(const TSrc &x);
    // static bool is_negative_clipped(const TSrc &x);
};

template <>
struct sample_clipping_checker<float, int32_t> {
    static bool is_positive_clipped(const int32_t &x)
    {
        return (x >= ((std::numeric_limits<int32_t>::max)() - (1 << 8))); // 2^8 = 2^(32 - 24)
    }

    static bool is_negative_clipped(const int32_t &x)
    {
        return (x <= ((std::numeric_limits<int32_t>::min)() + 1 + (1 << 8))); // 2^8 = 2^(32 - 24)
    }
};

template <>
struct sample_clipping_checker<float, int16_t> {
    static bool is_positive_clipped(const int16_t &x) { return (x == (std::numeric_limits<int16_t>::max)()); }

    static bool is_negative_clipped(const int16_t &x) { return (x <= ((std::numeric_limits<int16_t>::min)() + 1)); }
};

template <>
struct sample_clipping_checker<int32_t, int16_t> {
    static bool is_positive_clipped(const int16_t &x) { return (x == (std::numeric_limits<int16_t>::max)()); }

    static bool is_negative_clipped(const int16_t &x) { return (x <= ((std::numeric_limits<int16_t>::min)() + 1)); }
};

template <typename T, int NSrcChannels, int NDestChannels>
bool compare_audio_frame(const datatype::audio_frame<T, NSrcChannels> &a,
                         const datatype::audio_frame<T, NDestChannels> &b)
{
    static_assert(NSrcChannels == 1 || NSrcChannels == NDestChannels, "");

    for (int ch = 0; ch < NDestChannels; ++ch) {
        const T ta = a.c(std::min(ch, NSrcChannels - 1));
        const T tb = b.c(ch);

        if (!sample_comparator<T>::compare(ta, tb)) {
            return false;
        }
    }

    return true;
}

template <typename TSrc, typename TDest, int NChannels>
bool check_is_audio_frame_positive_clipped(const datatype::audio_frame<TDest, NChannels> &s)
{
    for (int ch = 0; ch < NChannels; ++ch) {
        if (!sample_clipping_checker<TSrc, TDest>::is_positive_clipped(s.c(ch))) {
            return false;
        }
    }

    return true;
}

template <typename TSrc, typename TDest, int NChannels>
bool check_is_audio_frame_negative_clipped(const datatype::audio_frame<TDest, NChannels> &s)
{
    for (int ch = 0; ch < NChannels; ++ch) {
        if (!sample_clipping_checker<TSrc, TDest>::is_negative_clipped(s.c(ch))) {
            return false;
        }
    }

    return true;
}

template <typename TSrc, int NSrcChannels, typename TDest, int NDestChannels, typename TConverterCoreOperator>
void do_oneshot_random_data_test(int n, bool use_fast_mode_conversion)
{
    static_assert(NSrcChannels == 1 || NSrcChannels == NDestChannels, "");

    typedef datatype::audio_frame<double, NSrcChannels> dp_frame_t;
    typedef datatype::audio_frame<TSrc, NSrcChannels> src_frame_t;
    typedef datatype::audio_frame<TDest, NDestChannels> dest_frame_t;

    std::vector<dp_frame_t> dp_data;
    std::vector<src_frame_t> src_data;
    std::vector<dest_frame_t> dest_data_excepted;
    std::vector<dest_frame_t> dest_data_actual;

    converter::sample_format_converter<src_frame_t, dest_frame_t, TConverterCoreOperator> converter;

    make_double_precision_random_data(dp_data, n);
    convert_audio_frames(dp_data, src_data, n, use_fast_mode_conversion);
    convert_audio_frames(src_data, dest_data_excepted, n, use_fast_mode_conversion);

    dest_data_actual.resize(n);
    converter.perform(&src_data[0], &dest_data_actual[0], n);

    // for (int i = 0; i < n; i++) {
    //     std::cout << i << " " << dest_data_excepted[i] << " - " << dest_data_actual[i] << std::endl;
    // }

    for (int i = 0; i < n; i++) {
        bool is_same = compare_audio_frame(dest_data_excepted[i], dest_data_actual[i]);
        ASSERT_TRUE(is_same);
    }
}

template <typename TSrc, int NSrcChannels, typename TDest, int NDestChannels, typename TConverterCoreOperator>
void do_min_max_value_data_test(bool use_fast_mode_conversion)
{
    static_assert(NSrcChannels == 1 || NSrcChannels == NDestChannels, "");

    typedef datatype::audio_frame<TSrc, NSrcChannels> src_frame_t;
    typedef datatype::audio_frame<TDest, NDestChannels> dest_frame_t;

    const int n = 64; // NOTE: requires enough size to check loop-unrolling code properly
    std::vector<src_frame_t> src_data;
    std::vector<dest_frame_t> dest_data_excepted;
    std::vector<dest_frame_t> dest_data_actual;

    converter::sample_format_converter<src_frame_t, dest_frame_t, TConverterCoreOperator> converter;

    if (std::is_floating_point<TSrc>::value) {
        src_data.push_back(src_frame_t(static_cast<TSrc>(1.0)));
        src_data.push_back(src_frame_t(static_cast<TSrc>(-1.0)));
    } else {
        auto t1 = src_frame_t((std::numeric_limits<TSrc>::max)());
        src_data.push_back(src_frame_t((std::numeric_limits<TSrc>::max)()));
        src_data.push_back(src_frame_t((std::numeric_limits<TSrc>::min)() + 1));
    }
    for (int i = 2; i < n; ++i) {
        src_data.push_back(src_data[i % 2]);
    }

    convert_audio_frames(src_data, dest_data_excepted, n, use_fast_mode_conversion);

    dest_data_actual.resize(n);
    converter.perform(&src_data[0], &dest_data_actual[0], n);

    // for (int i = 0; i < n; i++) {
    //     std::cout << i << " " << dest_data_excepted[i] << " - " << dest_data_actual[i] << std::endl;
    // }

    for (int i = 0; i < n; i++) {
        bool is_same = compare_audio_frame(dest_data_excepted[i], dest_data_actual[i]);
        ASSERT_TRUE(is_same);
    }
}

template <typename TSrc, int NSrcChannels, typename TDest, int NDestChannels, typename TConverterCoreOperator>
void do_clipping_test()
{
    static_assert(NSrcChannels == 1 || NSrcChannels == NDestChannels, "");

#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR && (CXXPH_TARGET_PLATFORM != CXXPH_PLATFORM_ANDROID)
    static_assert(std::is_floating_point<TSrc>::value ||
                      (std::numeric_limits<TSrc>::max)() > (std::numeric_limits<TDest>::max)(),
                  "TSrc requires floating point type or larger integral type");

    static_assert(std::is_integral<TDest>::value, "TDest requires integral type");
#endif

    typedef datatype::audio_frame<TSrc, NSrcChannels> src_frame_t;
    typedef datatype::audio_frame<TDest, NDestChannels> dest_frame_t;

    const int n = 64; // NOTE: requires enough size to check loop-unrolling code properly
    std::vector<src_frame_t> src_data;
    std::vector<dest_frame_t> dest_data_actual;

    converter::sample_format_converter<src_frame_t, dest_frame_t, TConverterCoreOperator> converter;

    if (std::is_floating_point<TSrc>::value) {
        src_data.push_back(src_frame_t(static_cast<TSrc>(1.01)));
        src_data.push_back(src_frame_t(static_cast<TSrc>(-1.01)));
        src_data.push_back(src_frame_t(static_cast<TSrc>(1.5)));
        src_data.push_back(src_frame_t(static_cast<TSrc>(-1.5)));
        src_data.push_back(src_frame_t(static_cast<TSrc>(2.0)));
        src_data.push_back(src_frame_t(static_cast<TSrc>(-2.0)));
        src_data.push_back(src_frame_t(static_cast<TSrc>(4.0)));
        src_data.push_back(src_frame_t(static_cast<TSrc>(-4.0)));
        src_data.push_back(src_frame_t(static_cast<TSrc>(8.0)));
        src_data.push_back(src_frame_t(static_cast<TSrc>(-8.0)));

        const int n1 = static_cast<int>(src_data.size());
        for (int i = n1; i < n; ++i) {
            src_data.push_back(src_data[i % n1]);
        }
    } else {
        src_data.push_back(
            src_frame_t(static_cast<TSrc>(std::numeric_limits<TSrc>::max() - (std::numeric_limits<TDest>::max() + 1))));
        src_data.push_back(
            src_frame_t(static_cast<TSrc>(std::numeric_limits<TSrc>::min() + (std::numeric_limits<TDest>::max() + 1))));
        src_data.push_back(src_frame_t(static_cast<TSrc>(std::numeric_limits<TSrc>::max())));
        src_data.push_back(src_frame_t(static_cast<TSrc>(std::numeric_limits<TSrc>::min())));

        const int n1 = static_cast<int>(src_data.size());
        for (int i = n1; i < n; ++i) {
            src_data.push_back(src_data[i % n1]);
        }
    }

    dest_data_actual.resize(n);
    converter.perform(&src_data[0], &dest_data_actual[0], n);

    // for (int i = 0; i < n; i++) {
    //     std::cout << i << " " << dest_data_actual[i] << std::endl;
    // }

    for (int i = 0; i < n; i++) {
        bool is_clipped;

        if (src_data[i].c(0) >= 0) {
            is_clipped = check_is_audio_frame_positive_clipped<TSrc, TDest, NDestChannels>(dest_data_actual[i]);
        } else {
            is_clipped = check_is_audio_frame_negative_clipped<TSrc, TDest, NDestChannels>(dest_data_actual[i]);
        }

        ASSERT_TRUE(is_clipped);
    }
}

template <typename TSrc, int NSrcChannels, typename TDest, int NDestChannels>
void do_general_converter_oneshot_random_data_test(int n, bool use_fast_mode_conversion)
{
    typedef datatype::audio_frame<TSrc, NSrcChannels> src_frame_t;
    typedef datatype::audio_frame<TDest, NDestChannels> dest_frame_t;
    typedef converter::general_sample_format_converter_core_operator<src_frame_t, dest_frame_t> core_operator_t;

    do_oneshot_random_data_test<TSrc, NSrcChannels, TDest, NDestChannels, core_operator_t>(n, use_fast_mode_conversion);
}

template <typename TSrc, int NSrcChannels, typename TDest, int NDestChannels>
void do_general_converter_min_max_value_test(bool use_fast_mode_conversion)
{
    typedef datatype::audio_frame<TSrc, NSrcChannels> src_frame_t;
    typedef datatype::audio_frame<TDest, NDestChannels> dest_frame_t;
    typedef converter::general_sample_format_converter_core_operator<src_frame_t, dest_frame_t> core_operator_t;

    do_min_max_value_data_test<TSrc, NSrcChannels, TDest, NDestChannels, core_operator_t>(use_fast_mode_conversion);
}

template <typename TSrc, int NSrcChannels, typename TDest, int NDestChannels>
void do_general_converter_clipping_test()
{
    typedef datatype::audio_frame<TSrc, NSrcChannels> src_frame_t;
    typedef datatype::audio_frame<TDest, NDestChannels> dest_frame_t;
    typedef converter::general_sample_format_converter_core_operator<src_frame_t, dest_frame_t> core_operator_t;

    do_clipping_test<TSrc, NSrcChannels, TDest, NDestChannels, core_operator_t>();
}

//
// General F64 to F32
//
class GeneralSampleFormatConverterF64toF32Test : public SampleFormatConverterTest {
};

TEST_F(GeneralSampleFormatConverterF64toF32Test, mono_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<double, 1, float, 1>(100, false);
}

TEST_F(GeneralSampleFormatConverterF64toF32Test, stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<double, 2, float, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterF64toF32Test, mono2stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<double, 1, float, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterF64toF32Test, mono_min_max_value)
{
    do_general_converter_min_max_value_test<double, 1, float, 1>(false);
}

TEST_F(GeneralSampleFormatConverterF64toF32Test, stereo_min_max_value)
{
    do_general_converter_min_max_value_test<double, 2, float, 2>(false);
}

TEST_F(GeneralSampleFormatConverterF64toF32Test, mono2stereo_min_max_value)
{
    do_general_converter_min_max_value_test<double, 1, float, 2>(false);
}

//
// General F32 to S32
//
class GeneralSampleFormatConverterF32toS32Test : public SampleFormatConverterTest {
};

TEST_F(GeneralSampleFormatConverterF32toS32Test, mono_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<float, 1, int32_t, 1>(100, false);
}

TEST_F(GeneralSampleFormatConverterF32toS32Test, stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<float, 2, int32_t, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterF32toS32Test, mono2stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<float, 1, int32_t, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterF32toS32Test, mono_min_max_value)
{
    do_general_converter_min_max_value_test<float, 1, int32_t, 1>(false);
}

TEST_F(GeneralSampleFormatConverterF32toS32Test, stereo_min_max_value)
{
    do_general_converter_min_max_value_test<float, 2, int32_t, 2>(false);
}

TEST_F(GeneralSampleFormatConverterF32toS32Test, mono2stereo_min_max_value)
{
    do_general_converter_min_max_value_test<float, 1, int32_t, 2>(false);
}

TEST_F(GeneralSampleFormatConverterF32toS32Test, mono_cliping)
{
    do_general_converter_clipping_test<float, 1, int32_t, 1>();
}

TEST_F(GeneralSampleFormatConverterF32toS32Test, stereo_cliping)
{
    do_general_converter_clipping_test<float, 2, int32_t, 2>();
}

TEST_F(GeneralSampleFormatConverterF32toS32Test, mono2stereo_cliping)
{
    do_general_converter_clipping_test<float, 1, int32_t, 2>();
}
//
// General F32 to S16
//
class GeneralSampleFormatConverterF32toS16Test : public SampleFormatConverterTest {
};

TEST_F(GeneralSampleFormatConverterF32toS16Test, mono_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<float, 1, int16_t, 1>(100, false);
}

TEST_F(GeneralSampleFormatConverterF32toS16Test, stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<float, 2, int16_t, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterF32toS16Test, mono2stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<float, 1, int16_t, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterF32toS16Test, mono_min_max_value)
{
    do_general_converter_min_max_value_test<float, 1, int16_t, 1>(false);
}

TEST_F(GeneralSampleFormatConverterF32toS16Test, stereo_min_max_value)
{
    do_general_converter_min_max_value_test<float, 2, int16_t, 2>(false);
}

TEST_F(GeneralSampleFormatConverterF32toS16Test, mono2stereo_min_max_value)
{
    do_general_converter_min_max_value_test<float, 1, int16_t, 2>(false);
}

TEST_F(GeneralSampleFormatConverterF32toS16Test, mono_clipping)
{
    do_general_converter_clipping_test<float, 1, int16_t, 1>();
}

TEST_F(GeneralSampleFormatConverterF32toS16Test, stereo_clipping)
{
    do_general_converter_clipping_test<float, 2, int16_t, 2>();
}

TEST_F(GeneralSampleFormatConverterF32toS16Test, mono2stereo_clipping)
{
    do_general_converter_clipping_test<float, 1, int16_t, 2>();
}

//
// General S32 to F32
//
class GeneralSampleFormatConverterS32toF32Test : public SampleFormatConverterTest {
};

TEST_F(GeneralSampleFormatConverterS32toF32Test, mono_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int32_t, 1, float, 1>(100, false);
}

TEST_F(GeneralSampleFormatConverterS32toF32Test, stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int32_t, 2, float, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterS32toF32Test, mono2stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int32_t, 1, float, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterS32toF32Test, mono_min_max_value)
{
    do_general_converter_min_max_value_test<int32_t, 1, float, 1>(false);
}

TEST_F(GeneralSampleFormatConverterS32toF32Test, stereo_min_max_value)
{
    do_general_converter_min_max_value_test<int32_t, 2, float, 2>(false);
}

TEST_F(GeneralSampleFormatConverterS32toF32Test, mono2stereo_min_max_value)
{
    do_general_converter_min_max_value_test<int32_t, 1, float, 2>(false);
}

//
// General S16 to F32
//
class GeneralSampleFormatConverterS16toF32Test : public SampleFormatConverterTest {
};

TEST_F(GeneralSampleFormatConverterS16toF32Test, mono_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int16_t, 1, float, 1>(100, false);
}

TEST_F(GeneralSampleFormatConverterS16toF32Test, stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int16_t, 2, float, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterS16toF32Test, mono2stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int16_t, 1, float, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterS16toF32Test, mono_min_max_value)
{
    do_general_converter_min_max_value_test<int16_t, 1, float, 1>(false);
}

TEST_F(GeneralSampleFormatConverterS16toF32Test, stereo_min_max_value)
{
    do_general_converter_min_max_value_test<int16_t, 2, float, 2>(false);
}

TEST_F(GeneralSampleFormatConverterS16toF32Test, mono2stereo_min_max_value)
{
    do_general_converter_min_max_value_test<int16_t, 1, float, 2>(false);
}

//
// General S32 to S16
//
class GeneralSampleFormatConverterS32toS16Test : public SampleFormatConverterTest {
};

TEST_F(GeneralSampleFormatConverterS32toS16Test, mono_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int32_t, 1, int16_t, 1>(100, false);
}

TEST_F(GeneralSampleFormatConverterS32toS16Test, stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int32_t, 2, int16_t, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterS32toS16Test, mono2stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int32_t, 1, int16_t, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterS32toS16Test, mono_min_max_value)
{
    do_general_converter_min_max_value_test<int32_t, 1, int16_t, 1>(false);
}

TEST_F(GeneralSampleFormatConverterS32toS16Test, stereo_min_max_value)
{
    do_general_converter_min_max_value_test<int32_t, 2, int16_t, 2>(false);
}

TEST_F(GeneralSampleFormatConverterS32toS16Test, mono2stereo_min_max_value)
{
    do_general_converter_min_max_value_test<int32_t, 1, int16_t, 2>(false);
}

TEST_F(GeneralSampleFormatConverterS32toS16Test, mono_clipping)
{
    do_general_converter_clipping_test<int32_t, 1, int16_t, 1>();
}

TEST_F(GeneralSampleFormatConverterS32toS16Test, stereo_clipping)
{
    do_general_converter_clipping_test<int32_t, 2, int16_t, 2>();
}

TEST_F(GeneralSampleFormatConverterS32toS16Test, mono2stereo_clipping)
{
    do_general_converter_clipping_test<int32_t, 1, int16_t, 2>();
}

//
// General S16 to S32
//
class GeneralSampleFormatConverterS16toS32Test : public SampleFormatConverterTest {
};

TEST_F(GeneralSampleFormatConverterS16toS32Test, mono_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int16_t, 1, int32_t, 1>(100, false);
}

TEST_F(GeneralSampleFormatConverterS16toS32Test, stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int16_t, 2, int32_t, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterS16toS32Test, mono2stereo_oneshot_random_data)
{
    do_general_converter_oneshot_random_data_test<int16_t, 1, int32_t, 2>(100, false);
}

TEST_F(GeneralSampleFormatConverterS16toS32Test, mono_min_max_value)
{
    do_general_converter_min_max_value_test<int16_t, 1, int32_t, 1>(false);
}

TEST_F(GeneralSampleFormatConverterS16toS32Test, stereo_min_max_value)
{
    do_general_converter_min_max_value_test<int16_t, 2, int32_t, 2>(false);
}

TEST_F(GeneralSampleFormatConverterS16toS32Test, mono2stereo_min_max_value)
{
    do_general_converter_min_max_value_test<int16_t, 1, int32_t, 2>(false);
}

#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
//
// SSE optimized S16 to F32
//
class SSEOptimizedSampleFormatConverterS16toF32Test : public SampleFormatConverterTest {
};

TEST_F(SSEOptimizedSampleFormatConverterS16toF32Test, mono_oneshot_random_data)
{
    typedef converter::s16_to_f32_mono_sse_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<int16_t, 1, float, 1, converter_t>(100, false);
}

TEST_F(SSEOptimizedSampleFormatConverterS16toF32Test, stereo_oneshot_random_data)
{
    typedef converter::s16_to_f32_stereo_sse_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<int16_t, 2, float, 2, converter_t>(100, false);
}

TEST_F(SSEOptimizedSampleFormatConverterS16toF32Test, mono2stereo_oneshot_random_data)
{
    typedef converter::s16_mono_to_f32_stereo_sse_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<int16_t, 1, float, 2, converter_t>(100, false);
}

TEST_F(SSEOptimizedSampleFormatConverterS16toF32Test, mono_min_max_value)
{
    typedef converter::s16_to_f32_mono_sse_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<int16_t, 1, float, 1, converter_t>(false);
}

TEST_F(SSEOptimizedSampleFormatConverterS16toF32Test, stereo_min_max_value)
{
    typedef converter::s16_to_f32_stereo_sse_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<int16_t, 2, float, 2, converter_t>(false);
}

TEST_F(SSEOptimizedSampleFormatConverterS16toF32Test, mono2stereo_min_max_value)
{
    typedef converter::s16_mono_to_f32_stereo_sse_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<int16_t, 1, float, 2, converter_t>(false);
}

//
// SSE optimized F32 to S16
//
class SSEOptimizedSampleFormatConverterF32toS16Test : public SampleFormatConverterTest {
};

TEST_F(SSEOptimizedSampleFormatConverterF32toS16Test, mono_oneshot_random_data)
{
    typedef converter::f32_to_s16_mono_sse_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<float, 1, int16_t, 1, converter_t>(100, false);
}

TEST_F(SSEOptimizedSampleFormatConverterF32toS16Test, stereo_oneshot_random_data)
{
    typedef converter::f32_to_s16_stereo_sse_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<float, 2, int16_t, 2, converter_t>(100, false);
}

TEST_F(SSEOptimizedSampleFormatConverterF32toS16Test, mono_min_max_value)
{
    typedef converter::f32_to_s16_mono_sse_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<float, 1, int16_t, 1, converter_t>(false);
}

TEST_F(SSEOptimizedSampleFormatConverterF32toS16Test, stereo_min_max_value)
{
    typedef converter::f32_to_s16_stereo_sse_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<float, 2, int16_t, 2, converter_t>(false);
}

TEST_F(SSEOptimizedSampleFormatConverterF32toS16Test, mono_clipping)
{
    typedef converter::f32_to_s16_mono_sse_sample_format_converter_core_operator converter_t;
    do_clipping_test<float, 1, int16_t, 1, converter_t>();
}

TEST_F(SSEOptimizedSampleFormatConverterF32toS16Test, stereo_clipping)
{
    typedef converter::f32_to_s16_stereo_sse_sample_format_converter_core_operator converter_t;
    do_clipping_test<float, 2, int16_t, 2, converter_t>();
}

#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON
//
// NEON optimized S16 to F32
//
class NEONOptimizedSampleFormatConverterS16toF32Test : public SampleFormatConverterTest {
};

TEST_F(NEONOptimizedSampleFormatConverterS16toF32Test, mono_oneshot_random_data)
{
    typedef converter::s16_to_f32_mono_neon_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<int16_t, 1, float, 1, converter_t>(100, false);
}

TEST_F(NEONOptimizedSampleFormatConverterS16toF32Test, stereo_oneshot_random_data)
{
    typedef converter::s16_to_f32_stereo_neon_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<int16_t, 2, float, 2, converter_t>(100, false);
}

TEST_F(NEONOptimizedSampleFormatConverterS16toF32Test, mono2stereo_oneshot_random_data)
{
    typedef converter::s16_mono_to_f32_stereo_neon_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<int16_t, 1, float, 2, converter_t>(100, false);
}

TEST_F(NEONOptimizedSampleFormatConverterS16toF32Test, mono_min_max_value)
{
    typedef converter::s16_to_f32_mono_neon_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<int16_t, 1, float, 1, converter_t>(false);
}

TEST_F(NEONOptimizedSampleFormatConverterS16toF32Test, stereo_min_max_value)
{
    typedef converter::s16_to_f32_stereo_neon_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<int16_t, 2, float, 2, converter_t>(false);
}

TEST_F(NEONOptimizedSampleFormatConverterS16toF32Test, mono2stereo_min_max_value)
{
    typedef converter::s16_mono_to_f32_stereo_neon_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<int16_t, 1, float, 2, converter_t>(false);
}

//
// NEON optimized F32 to S16
//
class NEONOptimizedSampleFormatConverterF32toS16Test : public SampleFormatConverterTest {
};

TEST_F(NEONOptimizedSampleFormatConverterF32toS16Test, mono_oneshot_random_data)
{
    typedef converter::f32_to_s16_mono_neon_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<float, 1, int16_t, 1, converter_t>(100, false);
}

TEST_F(NEONOptimizedSampleFormatConverterF32toS16Test, stereo_oneshot_random_data)
{
    typedef converter::f32_to_s16_stereo_neon_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<float, 2, int16_t, 2, converter_t>(100, false);
}

TEST_F(NEONOptimizedSampleFormatConverterF32toS16Test, mono_min_max_value)
{
    typedef converter::f32_to_s16_mono_neon_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<float, 1, int16_t, 1, converter_t>(false);
}

TEST_F(NEONOptimizedSampleFormatConverterF32toS16Test, stereo_min_max_value)
{
    typedef converter::f32_to_s16_stereo_neon_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<float, 2, int16_t, 2, converter_t>(false);
}

TEST_F(NEONOptimizedSampleFormatConverterF32toS16Test, mono_clipping)
{
    typedef converter::f32_to_s16_mono_neon_sample_format_converter_core_operator converter_t;
    do_clipping_test<float, 1, int16_t, 1, converter_t>();
}

TEST_F(NEONOptimizedSampleFormatConverterF32toS16Test, stereo_clipping)
{
    typedef converter::f32_to_s16_stereo_neon_sample_format_converter_core_operator converter_t;
    do_clipping_test<float, 2, int16_t, 2, converter_t>();
}

//
// NEON optimized S16 to F32 (more fast version)
//
class NEONOptimizedFastSampleFormatConverterS16toF32Test : public SampleFormatConverterTest {
};

TEST_F(NEONOptimizedFastSampleFormatConverterS16toF32Test, mono_oneshot_random_data)
{
    typedef converter::s16_to_f32_mono_neon_fast_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<int16_t, 1, float, 1, converter_t>(100, true);
}

TEST_F(NEONOptimizedFastSampleFormatConverterS16toF32Test, stereo_oneshot_random_data)
{
    typedef converter::s16_to_f32_stereo_neon_fast_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<int16_t, 2, float, 2, converter_t>(100, true);
}

TEST_F(NEONOptimizedFastSampleFormatConverterS16toF32Test, mono2stereo_oneshot_random_data)
{
    typedef converter::s16_mono_to_f32_stereo_neon_fast_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<int16_t, 1, float, 2, converter_t>(100, true);
}

TEST_F(NEONOptimizedFastSampleFormatConverterS16toF32Test, mono_min_max_value)
{
    typedef converter::s16_to_f32_mono_neon_fast_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<int16_t, 1, float, 1, converter_t>(true);
}

TEST_F(NEONOptimizedFastSampleFormatConverterS16toF32Test, stereo_min_max_value)
{
    typedef converter::s16_to_f32_stereo_neon_fast_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<int16_t, 2, float, 2, converter_t>(true);
}

TEST_F(NEONOptimizedFastSampleFormatConverterS16toF32Test, mono2stereo_min_max_value)
{
    typedef converter::s16_mono_to_f32_stereo_neon_fast_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<int16_t, 1, float, 2, converter_t>(true);
}

//
// NEON optimized F32 to S16 (more fast version)
//
class NEONOptimizedFastSampleFormatConverterF32toS16Test : public SampleFormatConverterTest {
};

TEST_F(NEONOptimizedFastSampleFormatConverterF32toS16Test, mono_oneshot_random_data)
{
    typedef converter::f32_to_s16_mono_neon_fast_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<float, 1, int16_t, 1, converter_t>(100, true);
}

TEST_F(NEONOptimizedFastSampleFormatConverterF32toS16Test, stereo_oneshot_random_data)
{
    typedef converter::f32_to_s16_stereo_neon_fast_sample_format_converter_core_operator converter_t;
    do_oneshot_random_data_test<float, 2, int16_t, 2, converter_t>(100, true);
}

TEST_F(NEONOptimizedFastSampleFormatConverterF32toS16Test, mono_min_max_value)
{
    typedef converter::f32_to_s16_mono_neon_fast_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<float, 1, int16_t, 1, converter_t>(true);
}

TEST_F(NEONOptimizedFastSampleFormatConverterF32toS16Test, stereo_min_max_value)
{
    typedef converter::f32_to_s16_stereo_neon_fast_sample_format_converter_core_operator converter_t;
    do_min_max_value_data_test<float, 2, int16_t, 2, converter_t>(true);
}

TEST_F(NEONOptimizedFastSampleFormatConverterF32toS16Test, mono_clipping)
{
    typedef converter::f32_to_s16_mono_neon_fast_sample_format_converter_core_operator converter_t;
    do_clipping_test<float, 1, int16_t, 1, converter_t>();
}

TEST_F(NEONOptimizedFastSampleFormatConverterF32toS16Test, stereo_clipping)
{
    typedef converter::f32_to_s16_stereo_neon_fast_sample_format_converter_core_operator converter_t;
    do_clipping_test<float, 2, int16_t, 2, converter_t>();
}
#endif
