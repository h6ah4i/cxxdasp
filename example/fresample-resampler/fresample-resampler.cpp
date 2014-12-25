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

// NOTE:
// This code is NOT an example of the audio-resampler library.
// It uses libfresample instead of the audio-resampler
// and used for benchmark purpose.

#include <iostream>
#include <limits>
#include <fresample.h>

#include "example_common.hpp"

#include <cxxdasp/utils/stopwatch.hpp>

static int gcd(int x, int y)
{
    if (!y)
        return x;
    else
        return gcd(y, x % y);
}

static int conv_to_fresample_quality(int q)
{
    switch (q) {
    case 1:
        return LFR_QUALITY_LOW;
    case 2:
        return LFR_QUALITY_LOW;
    case 3:
        return LFR_QUALITY_MEDIUM;
    case 4:
        return LFR_QUALITY_HIGH;
    case 5:
        return LFR_QUALITY_ULTRA;
    default:
        return (q > 5) ? LFR_QUALITY_ULTRA : LFR_QUALITY_LOW;
    }
}

template <typename T1, typename T2>
static void resample(const std::vector<T1> &src, std::vector<T2> &dest, int ch, int src_freq, int dest_freq, int quality)
{
    // create resampler
    const int g = gcd(src_freq, dest_freq);
    const int m = (g > 0) ? (dest_freq / g) : 0;
    const int l = (g > 0) ? (src_freq / g) : 0;

    int ilen = (int)src.size();
    int olen = (int)((src.size() * m + (l - 1)) / l);

    lfr_param *param = NULL;
    lfr_filter *filter = NULL;

    param = lfr_param_new();

    lfr_param_seti(param, LFR_PARAM_QUALITY, conv_to_fresample_quality(quality));
    lfr_param_seti(param, LFR_PARAM_INRATE, src_freq);
    lfr_param_seti(param, LFR_PARAM_OUTRATE, dest_freq);

    lfr_filter_new(&filter, param);

    const lfr_fixed_t inv_ratio = (((lfr_fixed_t) src_freq << 32) + dest_freq / 2) / dest_freq;
    const lfr_fmt_t dest_fmt = ((sizeof(T2) / ch) == (sizeof(int16_t))) ? LFR_FMT_S16_NATIVE : LFR_FMT_F32_NATIVE;

    unsigned int dither = 12345;
    lfr_fixed_t pos = 0;

    dest.resize(olen);

    cxxdasp::utils::stopwatch sw;
    sw.start();
    lfr_resample(
        &pos, inv_ratio, &dither, ch,
        &dest[0], dest_fmt, olen,
        &src[0], LFR_FMT_S16_NATIVE, ilen,
        filter);
    sw.stop();

    lfr_filter_free(filter);

    lfr_param_free(param);

    std::cout << "Resampling ("
              << "fresample, "
              << "CH=" << ch << ", "
              << "SRC=" << src_freq << ", "
              << "DEST=" << dest_freq << ", "
              << "M=" << m << ", "
              << "L=" << l << ", "
              << "Q=" << quality << ") "
              << ": " << sw.get_elapsed_time_us() << " [us]" << std::endl;
}

static void print_usage(const char *exe_name)
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    " << exe_name << " src_raw_file dest_raw_file num_channels src_freq dest_freq quality"
              << std::endl;
    std::cout << std::endl;
    std::cout << "Example (1ch, 32000 Hz -> 48000 Hz, quality = 1):" << std::endl;
    std::cout << "    " << exe_name << " 32000hz.raw 48000hz.raw 1 32000 48000 1" << std::endl;
    std::cout << std::endl;
    std::cout << "Example (2ch, 44100 Hz -> 48000 Hz, quality = 1):" << std::endl;
    std::cout << "    " << exe_name << " 44100hz.raw 48000hz.raw 2 44100 48000 1" << std::endl;
    std::cout << std::endl;
}

struct app_mono_frame_s16_t {
    int16_t v1;
};
struct app_stereo_frame_s16_t {
    int16_t v1;
    int16_t v2;
};
struct app_mono_frame_f32_t {
    float v1;
};
struct app_stereo_frame_f32_t {
    float v1;
    float v2;
};

static void convert(const std::vector<app_mono_frame_s16_t> &src, std::vector<app_mono_frame_f32_t> &dest) {
    size_t n = src.size();
    dest.resize(n);

    for(size_t i = 0; i < n; i++) {
        dest[i].v1 = src[i].v1 * (1.0f / (std::numeric_limits<int16_t>::max)());
    }
}

static void convert(const std::vector<app_mono_frame_f32_t> &src, std::vector<app_mono_frame_s16_t> &dest) {
    size_t n = src.size();
    dest.resize(n);

    for(size_t i = 0; i < n; i++) {
        dest[i].v1 = (int16_t) (src[i].v1 * (std::numeric_limits<int16_t>::max)());
    }
}

static void convert(const std::vector<app_stereo_frame_s16_t> &src, std::vector<app_stereo_frame_f32_t> &dest) {
    size_t n = src.size();
    dest.resize(n);

    for(size_t i = 0; i < n; i++) {
        dest[i].v1 = src[i].v1 * (1.0f / (std::numeric_limits<int16_t>::max)());
        dest[i].v2 = src[i].v2 * (1.0f / (std::numeric_limits<int16_t>::max)());
    }
}

static void convert(const std::vector<app_stereo_frame_f32_t> &src, std::vector<app_stereo_frame_s16_t> &dest) {
    size_t n = src.size();
    dest.resize(n);

    for(size_t i = 0; i < n; i++) {
        dest[i].v1 = (int16_t) (src[i].v1 * (std::numeric_limits<int16_t>::max)());
        dest[i].v2 = (int16_t) (src[i].v2 * (std::numeric_limits<int16_t>::max)());
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 7) {
        print_usage(argv[0]);
        return 1;
    }

    const char *src_filename = argv[1];
    const char *dest_filename = argv[2];
    const int num_channels = atoi(argv[3]);
    const int src_freq = atoi(argv[4]);
    const int dest_freq = atoi(argv[5]);
    const int quality = atoi(argv[6]);

    switch (num_channels) {
    case 1: {
        std::vector<app_mono_frame_s16_t> src_data_s16;
        std::vector<app_mono_frame_s16_t> resampled_s16;
        std::vector<app_mono_frame_f32_t> src_data_f32;
        std::vector<app_mono_frame_f32_t> resampled_f32;

        read_raw_file(src_filename, src_data_f32);
        convert(src_data_f32, src_data_s16);
#if 1
        resample(src_data_s16, resampled_s16, 1, src_freq, dest_freq, quality);
        convert(resampled_s16, resampled_f32);
#else
        // does not work...
        resample(src_data_s16, resampled_f32, 1, src_freq, dest_freq, quality);
#endif
        write_raw_file(dest_filename, resampled_f32);
    } break;
    case 2: {
        std::vector<app_stereo_frame_s16_t> src_data_s16;
        std::vector<app_stereo_frame_s16_t> resampled_s16;
        std::vector<app_stereo_frame_f32_t> src_data_f32;
        std::vector<app_stereo_frame_f32_t> resampled_f32;

        read_raw_file(src_filename, src_data_f32);
        convert(src_data_f32, src_data_s16);
#if 1
        resample(src_data_s16, resampled_s16, 2, src_freq, dest_freq, quality);
        convert(resampled_s16, resampled_f32);
#else
        // does not work...
        resample(src_data_s16, resampled_f32, 2, src_freq, dest_freq, quality);
#endif
        write_raw_file(dest_filename, resampled_f32);
    } break;
    default:
        std::cerr << "Unsupported channels (= " << num_channels << ")" << std::endl;
        break;
    }

    return 0;
}
