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

#include <iostream>
#include <algorithm>
#include <limits>
#include <cstdlib>

#include <cxxporthelper/cmath>
#include <cxxporthelper/aligned_memory.hpp>

#include <cxxdasp/cxxdasp.hpp>
#include <cxxdasp/datatype/audio_frame.hpp>
#include <cxxdasp/converter/sample_format_converter.hpp>
#include <cxxdasp/converter/sample_format_converter_core_operators.hpp>
#include <cxxdasp/utils/stopwatch.hpp>

#define USE_CXXDASP
#include "example_common.hpp"

#ifdef USE_ANDROID_NDK_PROFILER
#include <prof.h>
#endif

using namespace cxxdasp;

void print_failed_init_filter_msg() { std::cerr << "Failed to initialize filter." << std::endl; }

static void print_usage(const char *exe_name)
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    " << exe_name
              << " src_raw_file dest_raw_file num_channels samplingrate filter_type filter_f0 filter_db_gain filter_q"
              << std::endl;
    std::cout << "type = lowpass|highpass|bandpass|notch|peak|bell|lowshelf|highshelf|allpass" << std::endl;
    std::cout << std::endl;
    std::cout << "Example (1ch, 48000 Hz, \"low pass\", fc: 1000 Hz, gain: -10dB, Q: 1.0):" << std::endl;
    std::cout << "    " << exe_name << " 32000hz.raw 48000hz.raw 1 48000 lowpass 1000 -10.0 1.0" << std::endl;
    std::cout << std::endl;
}

template <typename TSrcFrame, typename TDestFrame, typename TConverter>
void convert_and_save(const cxxporthelper::aligned_memory<TSrcFrame> &src,
                      cxxporthelper::aligned_memory<TDestFrame> &dest, TConverter &c, const char *dest_file_name)
{

    // perform & measure elapsed time
    utils::stopwatch sw;
    sw.start();
#if 1
    // oneshot
    c.perform(&src[0], &dest[0], static_cast<int>(src.size()));
#else
    // randomize
    int offset = 0;
    int remains = static_cast<int>(static_cast<int>(src.size()));
    srand(1);
    while (remains > 0) {
        const int n = (std::min)(rand() % 100, remains);
        c.perform(&src[offset], &dest[offset], n);
        remains -= n;
        offset += n;
    }
#endif
    sw.stop();

    // print elapsed time
    std::cout << "Convert (" << dest_file_name << "): " << sw.get_elapsed_time_us() << " [us]" << std::endl;

    // save
    write_raw_file(dest_file_name, dest);
}

template <typename TSrcFrame, typename TDestFrame>
void convert_and_save(const cxxporthelper::aligned_memory<TSrcFrame> &src,
                      cxxporthelper::aligned_memory<TDestFrame> &dest, const char *dest_file_name)
{
    converter::sample_format_converter<
        TSrcFrame, TDestFrame, converter::general_sample_format_converter_core_operator<TSrcFrame, TDestFrame>> c;

    convert_and_save(src, dest, c, dest_file_name);
}

int main(int argc, char const *argv[])
{
    typedef datatype::audio_frame<double, 1> f64_mono_audio_frame_t;
    typedef datatype::audio_frame<float, 1> f32_mono_audio_frame_t;
    typedef datatype::audio_frame<int32_t, 1> s32_mono_audio_frame_t;
    typedef datatype::audio_frame<int16_t, 1> s16_mono_audio_frame_t;

    typedef datatype::audio_frame<double, 2> f64_stereo_audio_frame_t;
    typedef datatype::audio_frame<float, 2> f32_stereo_audio_frame_t;
    typedef datatype::audio_frame<int32_t, 2> s32_stereo_audio_frame_t;
    typedef datatype::audio_frame<int16_t, 2> s16_stereo_audio_frame_t;

#if CXXPH_TARGET_PLATFORM == CXXPH_PLATFORM_ANDROID
    const int n = 44100 * 10;
#else
    const int n = 44100 * 30;
#endif
    cxxporthelper::aligned_memory<f64_mono_audio_frame_t> f64_mono_orig(n);
    cxxporthelper::aligned_memory<f32_mono_audio_frame_t> f32_mono_orig(n);
    cxxporthelper::aligned_memory<s32_mono_audio_frame_t> s32_mono_orig(n);
    cxxporthelper::aligned_memory<s16_mono_audio_frame_t> s16_mono_orig(n);

    cxxporthelper::aligned_memory<f64_mono_audio_frame_t> f64_mono_converted(n);
    cxxporthelper::aligned_memory<f32_mono_audio_frame_t> f32_mono_converted(n);
    cxxporthelper::aligned_memory<s32_mono_audio_frame_t> s32_mono_converted(n);
    cxxporthelper::aligned_memory<s16_mono_audio_frame_t> s16_mono_converted(n);

    cxxporthelper::aligned_memory<f64_stereo_audio_frame_t> f64_stereo_orig(n);
    cxxporthelper::aligned_memory<f32_stereo_audio_frame_t> f32_stereo_orig(n);
    cxxporthelper::aligned_memory<s32_stereo_audio_frame_t> s32_stereo_orig(n);
    cxxporthelper::aligned_memory<s16_stereo_audio_frame_t> s16_stereo_orig(n);

    cxxporthelper::aligned_memory<f64_stereo_audio_frame_t> f64_stereo_converted(n);
    cxxporthelper::aligned_memory<f32_stereo_audio_frame_t> f32_stereo_converted(n);
    cxxporthelper::aligned_memory<s32_stereo_audio_frame_t> s32_stereo_converted(n);
    cxxporthelper::aligned_memory<s16_stereo_audio_frame_t> s16_stereo_converted(n);

    // generate test tone
    for (int i = 0; i < n; ++i) {
        const double l = sin(0.01 * i);
        const double r = sin(0.03 * i);

        f64_mono_orig[i].c(0) = l;

        f32_mono_orig[i].c(0) = static_cast<float>(l);

        s32_mono_orig[i].c(0) = static_cast<int32_t>(l * (std::numeric_limits<int32_t>::max)());

        s16_mono_orig[i].c(0) = static_cast<int16_t>(l * (std::numeric_limits<int16_t>::max)());

        f64_stereo_orig[i].c(0) = l;
        f64_stereo_orig[i].c(1) = r;

        f32_stereo_orig[i].c(0) = static_cast<float>(l);
        f32_stereo_orig[i].c(1) = static_cast<float>(r);

        s32_stereo_orig[i].c(0) = static_cast<int32_t>(l * (std::numeric_limits<int32_t>::max)());
        s32_stereo_orig[i].c(1) = static_cast<int32_t>(r * (std::numeric_limits<int32_t>::max)());

        s16_stereo_orig[i].c(0) = static_cast<int16_t>(l * (std::numeric_limits<int16_t>::max)());
        s16_stereo_orig[i].c(1) = static_cast<int16_t>(r * (std::numeric_limits<int16_t>::max)());
    }

#ifdef USE_ANDROID_NDK_PROFILER
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON
    ::monstartup("sample-format-converter");
#else
    ::monstartup("sample-format-converter-no-neon");
#endif
#endif

    // initialize
    cxxdasp_init();

#if 1
    // to double
    convert_and_save(f64_mono_orig, f64_mono_converted, "f64_to_f64_mono.raw");
    convert_and_save(f32_mono_orig, f64_mono_converted, "f32_to_f64_mono.raw");
    convert_and_save(s32_mono_orig, f64_mono_converted, "s32_to_f64_mono.raw");
    convert_and_save(s16_mono_orig, f64_mono_converted, "s16_to_f64_mono.raw");
    convert_and_save(f64_stereo_orig, f64_stereo_converted, "f64_to_f64_stereo.raw");
    convert_and_save(f32_stereo_orig, f64_stereo_converted, "f32_to_f64_stereo.raw");
    convert_and_save(s32_stereo_orig, f64_stereo_converted, "s32_to_f64_stereo.raw");
    convert_and_save(s16_stereo_orig, f64_stereo_converted, "s16_to_f64_stereo.raw");

    // to float
    convert_and_save(f64_mono_orig, f32_mono_converted, "f64_to_f32_mono.raw");
    convert_and_save(f32_mono_orig, f32_mono_converted, "f32_to_f32_mono.raw");
    convert_and_save(s32_mono_orig, f32_mono_converted, "s32_to_f32_mono.raw");
    convert_and_save(s16_mono_orig, f32_mono_converted, "s16_to_f32_mono.raw");
    convert_and_save(f64_stereo_orig, f32_stereo_converted, "f64_to_f32_stereo.raw");
    convert_and_save(f32_stereo_orig, f32_stereo_converted, "f32_to_f32_stereo.raw");
    convert_and_save(s32_stereo_orig, f32_stereo_converted, "s32_to_f32_stereo.raw");
    convert_and_save(s16_stereo_orig, f32_stereo_converted, "s16_to_f32_stereo.raw");

    // to int32_t
    convert_and_save(f64_mono_orig, s32_mono_converted, "f64_to_s32_mono.raw");
    convert_and_save(f32_mono_orig, s32_mono_converted, "f32_to_s32_mono.raw");
    convert_and_save(s32_mono_orig, s32_mono_converted, "s32_to_s32_mono.raw");
    convert_and_save(s16_mono_orig, s32_mono_converted, "s16_to_s32_mono.raw");
    convert_and_save(f64_stereo_orig, s32_stereo_converted, "f64_to_s32_stereo.raw");
    convert_and_save(f32_stereo_orig, s32_stereo_converted, "f32_to_s32_stereo.raw");
    convert_and_save(s32_stereo_orig, s32_stereo_converted, "s32_to_s32_stereo.raw");
    convert_and_save(s16_stereo_orig, s32_stereo_converted, "s16_to_s32_stereo.raw");

    // to int16_t
    convert_and_save(f64_mono_orig, s16_mono_converted, "f64_to_s16_mono.raw");
    convert_and_save(f32_mono_orig, s16_mono_converted, "f32_to_s16_mono.raw");
    convert_and_save(s32_mono_orig, s16_mono_converted, "s32_to_s16_mono.raw");
    convert_and_save(s16_mono_orig, s16_mono_converted, "s16_to_s16_mono.raw");
    convert_and_save(f64_stereo_orig, s16_stereo_converted, "f64_to_s16_stereo.raw");
    convert_and_save(f32_stereo_orig, s16_stereo_converted, "f32_to_s16_stereo.raw");
    convert_and_save(s32_stereo_orig, s16_stereo_converted, "s32_to_s16_stereo.raw");
    convert_and_save(s16_stereo_orig, s16_stereo_converted, "s16_to_s16_stereo.raw");
#endif

#if ((CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64))
    if (converter::s16_to_f32_mono_sse_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<s16_mono_audio_frame_t, f32_mono_audio_frame_t,
                                           converter::s16_to_f32_mono_sse_sample_format_converter_core_operator> c;
        convert_and_save(s16_mono_orig, f32_mono_converted, c, "s16_to_f32_mono_sse.raw");
    }
    if (converter::s16_to_f32_stereo_sse_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<s16_stereo_audio_frame_t, f32_stereo_audio_frame_t,
                                           converter::s16_to_f32_stereo_sse_sample_format_converter_core_operator> c;
        convert_and_save(s16_stereo_orig, f32_stereo_converted, c, "s16_to_f32_stereo_sse.raw");
    }
    if (converter::s16_mono_to_f32_stereo_sse_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<s16_mono_audio_frame_t, f32_stereo_audio_frame_t,
                                           converter::s16_mono_to_f32_stereo_sse_sample_format_converter_core_operator>
        c;
        convert_and_save(s16_mono_orig, f32_stereo_converted, c, "s16_mono_to_f32_stereo_sse.raw");
    }
    if (converter::f32_to_s16_mono_sse_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<f32_mono_audio_frame_t, s16_mono_audio_frame_t,
                                           converter::f32_to_s16_mono_sse_sample_format_converter_core_operator> c;
        convert_and_save(f32_mono_orig, s16_mono_converted, c, "f32_to_s16_mono_sse.raw");
    }
    if (converter::f32_to_s16_stereo_sse_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<f32_stereo_audio_frame_t, s16_stereo_audio_frame_t,
                                           converter::f32_to_s16_stereo_sse_sample_format_converter_core_operator> c;
        convert_and_save(f32_stereo_orig, s16_stereo_converted, c, "f32_to_s16_stereo_sse.raw");
    }
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON
    if (converter::s16_to_f32_mono_neon_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<s16_mono_audio_frame_t, f32_mono_audio_frame_t,
                                           converter::s16_to_f32_mono_neon_sample_format_converter_core_operator> c;
        convert_and_save(s16_mono_orig, f32_mono_converted, c, "s16_to_f32_mono_neon.raw");
    }
    if (converter::s16_to_f32_stereo_neon_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<s16_stereo_audio_frame_t, f32_stereo_audio_frame_t,
                                           converter::s16_to_f32_stereo_neon_sample_format_converter_core_operator> c;
        convert_and_save(s16_stereo_orig, f32_stereo_converted, c, "s16_to_f32_stereo_neon.raw");
    }
    if (converter::s16_mono_to_f32_stereo_neon_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<s16_mono_audio_frame_t, f32_stereo_audio_frame_t,
                                           converter::s16_mono_to_f32_stereo_neon_sample_format_converter_core_operator>
        c;
        convert_and_save(s16_mono_orig, f32_stereo_converted, c, "s16_mono_to_f32_stereo_neon.raw");
    }
    if (converter::f32_to_s16_mono_neon_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<f32_mono_audio_frame_t, s16_mono_audio_frame_t,
                                           converter::f32_to_s16_mono_neon_sample_format_converter_core_operator> c;
        convert_and_save(f32_mono_orig, s16_mono_converted, c, "f32_to_s16_mono_neon.raw");
    }
    if (converter::f32_to_s16_stereo_neon_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<f32_stereo_audio_frame_t, s16_stereo_audio_frame_t,
                                           converter::f32_to_s16_stereo_neon_sample_format_converter_core_operator> c;
        convert_and_save(f32_stereo_orig, s16_stereo_converted, c, "f32_to_s16_stereo_neon.raw");
    }

    if (converter::s16_to_f32_mono_neon_fast_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<s16_mono_audio_frame_t, f32_mono_audio_frame_t,
                                           converter::s16_to_f32_mono_neon_fast_sample_format_converter_core_operator>
        c;
        convert_and_save(s16_mono_orig, f32_mono_converted, c, "s16_to_f32_mono_neon_fast.raw");
    }
    if (converter::s16_to_f32_stereo_neon_fast_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<s16_stereo_audio_frame_t, f32_stereo_audio_frame_t,
                                           converter::s16_to_f32_stereo_neon_fast_sample_format_converter_core_operator>
        c;
        convert_and_save(s16_stereo_orig, f32_stereo_converted, c, "s16_to_f32_stereo_neon_fast.raw");
    }
    if (converter::s16_mono_to_f32_stereo_neon_fast_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<
            s16_mono_audio_frame_t, f32_stereo_audio_frame_t,
            converter::s16_mono_to_f32_stereo_neon_fast_sample_format_converter_core_operator> c;
        convert_and_save(s16_mono_orig, f32_stereo_converted, c, "s16_mono_to_f32_stereo_neon_fast.raw");
    }
    if (converter::f32_to_s16_mono_neon_fast_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<f32_mono_audio_frame_t, s16_mono_audio_frame_t,
                                           converter::f32_to_s16_mono_neon_fast_sample_format_converter_core_operator>
        c;
        convert_and_save(f32_mono_orig, s16_mono_converted, c, "f32_to_s16_mono_neon_fast.raw");
    }
    if (converter::f32_to_s16_stereo_neon_fast_sample_format_converter_core_operator::is_supported()) {
        converter::sample_format_converter<f32_stereo_audio_frame_t, s16_stereo_audio_frame_t,
                                           converter::f32_to_s16_stereo_neon_sample_format_converter_core_operator> c;
        convert_and_save(f32_stereo_orig, s16_stereo_converted, c, "f32_to_s16_stereo_neon_fast.raw");
    }
#endif

#ifdef USE_ANDROID_NDK_PROFILER
    ::moncleanup();
#endif

    return 0;
}
