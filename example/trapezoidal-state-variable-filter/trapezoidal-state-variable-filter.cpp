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

#include <cxxdasp/cxxdasp.hpp>
#include <cxxdasp/datatype/audio_frame.hpp>
#include <cxxdasp/filter/tsvf/tsvf.hpp>
#include <cxxdasp/filter/tsvf/tsvf_core_operators.hpp>
#include <cxxdasp/utils/stopwatch.hpp>

#define USE_CXXDASP
#include "example_common.hpp"

#ifdef USE_ANDROID_NDK_PROFILER
#include <prof.h>
#endif

using namespace cxxdasp;

template <typename TFrame, class TFilterOperator>
bool apply_filter(const std::vector<TFrame> &src, std::vector<TFrame> &dest, const filter::filter_params_t &f_params)
{

    filter::trapezoidal_state_variable_filter<TFrame, TFilterOperator> flt;

    if (!flt.init(f_params)) {
        return false;
    }

    const int n = static_cast<int>(src.size());

    dest.resize(n);

    flt.reset();

    // perform filtering & measure elapsed time
    utils::stopwatch sw;
    sw.start();
    flt.perform(&src[0], &dest[0], n);
    sw.stop();

    // print elapsed time
    std::cout << "Filter: " << sw.get_elapsed_time_us() << " [us]" << std::endl;

    return true;
}

template <typename TFrame, class TFastFilterOperator, class TFallbackFilterOperator>
bool apply_filter(const std::vector<TFrame> &src, std::vector<TFrame> &dest, const filter::filter_params_t &f_params)
{

    if (TFastFilterOperator::is_supported()) {
        return apply_filter<TFrame, TFastFilterOperator>(src, dest, f_params);
    } else {
        return apply_filter<TFrame, TFallbackFilterOperator>(src, dest, f_params);
    }

    return true;
}

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

int main(int argc, char const *argv[])
{
    typedef datatype::f32_mono_frame_t app_mono_frame_t;
    typedef datatype::f32_stereo_frame_t app_stereo_frame_t;

    typedef filter::general_tsvf_core_operator<app_mono_frame_t> app_fallback_mono_filter_operator_t;
    typedef filter::general_tsvf_core_operator<app_stereo_frame_t> app_fallback_stereo_filter_operator_t;

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON
    typedef filter::f32_mono_neon_tsvf_core_operator app_fast_mono_filter_operator_t;
    typedef filter::f32_stereo_neon_tsvf_core_operator app_fast_stereo_filter_operator_t;
#else
    typedef app_fallback_mono_filter_operator_t app_fast_mono_filter_operator_t;
    typedef app_fallback_stereo_filter_operator_t app_fast_stereo_filter_operator_t;
#endif

    if (argc < 9) {
        print_usage(argv[0]);
        return 1;
    }

#ifdef USE_ANDROID_NDK_PROFILER
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON
    ::monstartup("trapezoidal-state-variable-filter");
#else
    ::monstartup("trapezoidal-state-variable-filter-no-neon");
#endif
#endif

    // initialize
    cxxdasp_init();

    const char *src_filename = argv[1];
    const char *dest_filename = argv[2];
    const int num_channels = atoi(argv[3]);
    const double f_fs = strtod(argv[4], nullptr);
    const filter::filter_type_t f_type = parse_filter_type(argv[5]);
    const double f_f0 = strtod(argv[6], nullptr);
    const double f_db_gain = strtod(argv[7], nullptr);
    const double f_q = strtod(argv[8], nullptr);

    filter::filter_params_t f_params;

    f_params.type = f_type;
    f_params.fs = f_fs;
    f_params.f0 = f_f0;
    f_params.db_gain = f_db_gain;
    f_params.q = f_q;

    switch (num_channels) {
    case 1: {
        std::vector<app_mono_frame_t> src_data;
        std::vector<app_mono_frame_t> filtered;

        read_raw_file(src_filename, src_data);
        if (!apply_filter<app_mono_frame_t, app_fast_mono_filter_operator_t, app_fallback_mono_filter_operator_t>(
                src_data, filtered, f_params)) {
            print_failed_init_filter_msg();
            return -1;
        }
        write_raw_file(dest_filename, filtered);
    } break;
    case 2: {
        std::vector<app_stereo_frame_t> src_data;
        std::vector<app_stereo_frame_t> filtered;

        read_raw_file(src_filename, src_data);
        if (!apply_filter<app_stereo_frame_t, app_fast_stereo_filter_operator_t, app_fallback_stereo_filter_operator_t>(
                src_data, filtered, f_params)) {
            print_failed_init_filter_msg();
            return -1;
        }
        write_raw_file(dest_filename, filtered);
    } break;
    default:
        std::cerr << "Unsupported channels (= " << num_channels << ")" << std::endl;
        break;
    }

#ifdef USE_ANDROID_NDK_PROFILER
    ::moncleanup();
#endif

    return 0;
}
