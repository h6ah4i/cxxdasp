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
#include <samplerate.h>

#include "example_common.hpp"

#include <cxxdasp/utils/stopwatch.hpp>

static int gcd(int x, int y)
{
    if (!y)
        return x;
    else
        return gcd(y, x % y);
}

static int conv_to_src_converter(int q)
{
    switch (q) {
    case 1:
        return SRC_ZERO_ORDER_HOLD;
    case 2:
        return SRC_LINEAR;
    case 3:
        return SRC_SINC_FASTEST;
    case 4:
        return SRC_SINC_MEDIUM_QUALITY;
    case 5:
        return SRC_SINC_BEST_QUALITY;
    default:
        return (q > 5) ? SRC_SINC_BEST_QUALITY : SRC_ZERO_ORDER_HOLD;
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

    dest.resize(olen);

    int src_converter = conv_to_src_converter(quality);

    SRC_STATE *src_state;
    SRC_DATA src_data;

    src_data.data_in = reinterpret_cast<float *>(const_cast<T*>(&(src[0])));
    src_data.data_out = reinterpret_cast<float *>(&(dest[0]));
    src_data.input_frames = static_cast<long>(ilen);
    src_data.output_frames = static_cast<long>(olen);
    src_data.input_frames_used = 0;
    src_data.output_frames_gen = 0;
    src_data.end_of_input = 1;
    src_data.src_ratio = (double) m / l;

    int error;

    if ((src_state = src_new(src_converter, ch, &error)) == NULL)
        return;

    // process resampling & measure elapsed time
    cxxdasp::utils::stopwatch sw;
    sw.start();

    error = src_process(src_state, &src_data) ;

    sw.stop();

    dest.resize(src_data.output_frames_gen);

    src_state = src_delete(src_state);

    // print elapsed time
    std::cout << "Resampling ("
              << "libsamplerate, "
              << "CH=" << ch << ", "
              << "SRC=" << src_freq << ", "
              << "DEST=" << dest_freq << ", "
              << "M=" << m << ", "
              << "L=" << l << ", "
              << "Q=" << quality << ") "
              << "result=" << src_strerror(error)
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
