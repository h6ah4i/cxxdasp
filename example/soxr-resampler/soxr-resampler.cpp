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
// It uses soxr (The SoX Resampling library) instead of the audio-resampler
// and used for benchmark purpose.

#include <iostream>
#include <soxr.h>

#include "example_common.hpp"

#include <cxxdasp/utils/stopwatch.hpp>

static int gcd(int x, int y)
{
    if (!y)
        return x;
    else
        return gcd(y, x % y);
}

static int conv_to_soxr_quality(int q)
{
    switch (q) {
    case 1:
        return SOXR_QQ;
    case 2:
        return SOXR_LQ;
    case 3:
        return SOXR_MQ;
    case 4:
        return SOXR_HQ;
    case 5:
        return SOXR_VHQ;
    default:
        return (q > 5) ? SOXR_VHQ : SOXR_QQ;
    }
}

template <typename T>
static void resample(const std::vector<T> &src, std::vector<T> &dest, int ch, int src_freq, int dest_freq, int quality)
{
    // create resampler
    const int g = gcd(src_freq, dest_freq);
    const int m = (g > 0) ? (dest_freq / g) : 0;
    const int l = (g > 0) ? (src_freq / g) : 0;

    size_t ilen = src.size();
    size_t olen = (size_t)((src.size() * m + (l - 1)) / l);

    soxr_io_spec_t io_spec = soxr_io_spec(SOXR_FLOAT32_I, SOXR_FLOAT32_I);
    soxr_quality_spec_t q_spec = soxr_quality_spec(conv_to_soxr_quality(quality), SOXR_LINEAR_PHASE);
    soxr_runtime_spec_t r_spec = soxr_runtime_spec(1);
    size_t odone;
    soxr_error_t error;

    soxr_t r = soxr_create(l, m, ch, &error, &io_spec, &q_spec, &r_spec);

    dest.resize(olen);

    // process resampling & measure elapsed time
    cxxdasp::utils::stopwatch sw;
    sw.start();
    error = soxr_process(r, &src[0], ~ilen, NULL, &dest[0], olen, &odone);
    sw.stop();

    dest.resize(odone);

    soxr_delete(r);

    // print elapsed time
    std::cout << "Resampling ("
              << "SOXR, "
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

int main(int argc, char const *argv[])
{
    struct app_mono_frame_t {
        float v1;
    };
    struct app_stereo_frame_t {
        float v1;
        float v2;
    };

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
        std::vector<app_mono_frame_t> src_data;
        std::vector<app_mono_frame_t> resampled;

        read_raw_file(src_filename, src_data);
        resample(src_data, resampled, 1, src_freq, dest_freq, quality);
        write_raw_file(dest_filename, resampled);
    } break;
    case 2: {
        std::vector<app_stereo_frame_t> src_data;
        std::vector<app_stereo_frame_t> resampled;

        read_raw_file(src_filename, src_data);
        resample(src_data, resampled, 2, src_freq, dest_freq, quality);
        write_raw_file(dest_filename, resampled);
    } break;
    default:
        std::cerr << "Unsupported channels (= " << num_channels << ")" << std::endl;
        break;
    }

    return 0;
}
