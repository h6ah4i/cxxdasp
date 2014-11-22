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
#include <cxxdasp/resampler/smart/smart_resampler.hpp>
#include <cxxdasp/resampler/smart/smart_resampler_params_factory.hpp>
#include <cxxdasp/utils/stopwatch.hpp>

#define USE_CXXDASP
#include "example_common.hpp"

#ifdef USE_ANDROID_NDK_PROFILER
#include <prof.h>
#endif

// #define RANDOMIZE_PROCESS_SIZE

using namespace cxxdasp;

static resampler::smart_resampler_params_factory::quality_spec_t conv_to_smart_resampler_quality(int q)
{
    // NOTE:
    switch (q) {
    case 1:
        return resampler::smart_resampler_params_factory::LowQuality;
    case 2:
        return resampler::smart_resampler_params_factory::MidQuality;
    case 3: // == synonym to q4
    case 4:
        return resampler::smart_resampler_params_factory::HighQuality;
    case 5:
        return resampler::smart_resampler_params_factory::HighQuality; // also requires double-precision FFT
    default:
        return resampler::smart_resampler_params_factory::HighQuality;
    }
}

template <class TResampler, typename TSrc, typename TDest>
static void resampler_process(TResampler &r, const TSrc *src, int n_src, int *idone, TDest *dest, int n_dest,
                              int *odone)
{
    int src_index = 0;
    int dest_index = 0;

#ifdef RANDOMIZE_PROCESS_SIZE
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    std::uniform_int_distribution<int> dist(0, 10000);
#endif

    while (src_index < n_src && dest_index < n_dest) {
        {
            const int src_remains = (n_src - src_index);

            if (src_remains > 0) {
                int n = (std::min)(r.num_can_put(), src_remains);
#ifdef RANDOMIZE_PROCESS_SIZE
                n = std::min(n, dist(engine));
#endif
                r.put_n(&src[src_index], n);
                src_index += n;
            } else {
                break;
            }
        }

        {
            const int dest_remains = (n_dest - dest_index);
            int n = (std::min)(r.num_can_get(), dest_remains);
#ifdef RANDOMIZE_PROCESS_SIZE
            n = std::min(n, dist(engine));
#endif
            r.get_n(&dest[dest_index], n);
            dest_index += n;
        }
    }

    r.flush();

    while (true) {
        const int dest_remains = (n_dest - dest_index);
        int n = (std::min)(r.num_can_get(), dest_remains);

        if (n == 0) {
            break;
        }

#ifdef RANDOMIZE_PROCESS_SIZE
        n = std::min(n, dist(engine));
#endif

        r.get_n(&dest[dest_index], n);
        dest_index += n;
    }

    if (idone) {
        (*idone) = src_index;
    }
    if (odone) {
        (*odone) = dest_index;
    }
}

template <typename TSrc, typename TDest, class THalfbandOperator, class TFFTBackend, class TPolyphaseOperator>
static void resample(const std::vector<TSrc> &src, std::vector<TDest> &dest, int ch, int src_freq, int dest_freq,
                     int quality)
{
    // create resampler
    const resampler::smart_resampler_params_factory factory(src_freq, dest_freq,
                                                            conv_to_smart_resampler_quality(quality));

    if (!factory) {
        std::cerr << "Requested operation is not supported." << std::endl;
        return;
    }

    resampler::smart_resampler<TSrc, TDest, THalfbandOperator, TFFTBackend, TPolyphaseOperator> r(factory.params());
    const int g = utils::gcd(src_freq, dest_freq);
    const int m = dest_freq / g;
    const int l = src_freq / g;
    const int n_src = static_cast<int>(src.size());
    const int n_dest = ((n_src - 1) * m + 1) / l;

    int odone;

    dest.resize(n_dest);

    // process resampling & measure elapsed time
    utils::stopwatch sw;
    sw.start();
    resampler_process(r, &src[0], n_src, NULL, &dest[0], n_dest, &odone);
    sw.stop();

    dest.resize(odone);

    // print elapsed time
    std::cout << "Resampling ("
              << "SIMPLE, "
              << "CH=" << ch << ", "
              << "SRC=" << src_freq << ", "
              << "DEST=" << dest_freq << ", "
              << "M=" << m << ", "
              << "L=" << l << ", "
              << "Q=" << quality << ") "
              << ": " << sw.get_elapsed_time_us() << " [us]" << std::endl;
}

template <typename TSrc, typename TDest, class TFastHalfbandOperator, class TFallbackHalfbandOperator,
          class TFFTBackend, class TFastPolyphaseOperator, class TFallbackPolyphaseOperator>
static void resample(const std::vector<TSrc> &src, std::vector<TDest> &dest, int ch, int src_freq, int dest_freq,
                     int quality)
{
    const bool fast_halfband = TFastHalfbandOperator::is_supported();
    const bool fast_polyphase = TFastPolyphaseOperator::is_supported();

    if (fast_halfband && fast_polyphase) {
        resample<TSrc, TDest, TFastHalfbandOperator, TFFTBackend, TFastPolyphaseOperator>(src, dest, ch, src_freq,
                                                                                          dest_freq, quality);
    } else if (fast_halfband && !fast_polyphase) {
        resample<TSrc, TDest, TFastHalfbandOperator, TFFTBackend, TFallbackPolyphaseOperator>(src, dest, ch, src_freq,
                                                                                              dest_freq, quality);
    } else if (!fast_halfband && fast_polyphase) {
        resample<TSrc, TDest, TFallbackHalfbandOperator, TFFTBackend, TFastPolyphaseOperator>(src, dest, ch, src_freq,
                                                                                              dest_freq, quality);
    } else {
        resample<TSrc, TDest, TFallbackHalfbandOperator, TFFTBackend, TFallbackPolyphaseOperator>(
            src, dest, ch, src_freq, dest_freq, quality);
    }
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
    //
    // typedefs
    //
    typedef resampler::f32_mono_basic_polyphase_core_operator f32_mono_app_fallback_polyphase_core_operator;

// f32_mono_app_fast_polyphase_core_operator
#if CXXPH_COMPILER_SUPPORTS_X86_SSE3
    typedef resampler::f32_mono_sse3_polyphase_core_operator f32_mono_app_fast_polyphase_core_operator;
#elif CXXPH_COMPILER_SUPPORTS_X86_SSE
    typedef resampler::f32_mono_sse_polyphase_core_operator f32_mono_app_fast_polyphase_core_operator;
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON
    typedef resampler::f32_mono_neon_polyphase_core_operator f32_mono_app_fast_polyphase_core_operator;
#else
    typedef f32_mono_app_fallback_polyphase_core_operator f32_mono_app_fast_polyphase_core_operator;
#endif

    typedef resampler::f32_stereo_basic_polyphase_core_operator f32_stereo_app_fallback_polyphase_core_operator;

// f32_stereo_app_fast_polyphase_core_operator
#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    typedef resampler::f32_stereo_sse_polyphase_core_operator f32_stereo_app_fast_polyphase_core_operator;
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON
    typedef resampler::f32_stereo_neon_polyphase_core_operator f32_stereo_app_fast_polyphase_core_operator;
#else
    typedef f32_stereo_app_fallback_polyphase_core_operator f32_stereo_app_fast_polyphase_core_operator;
#endif

    typedef resampler::f32_mono_basic_halfband_x2_resampler_core_operator f32_mono_app_fallback_halfband_core_operator;

// f32_mono_app_fast_halfband_core_operator
#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    typedef resampler::f32_mono_sse_halfband_x2_resampler_core_operator f32_mono_app_fast_halfband_core_operator;
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON
    typedef resampler::f32_mono_neon_halfband_x2_resampler_core_operator f32_mono_app_fast_halfband_core_operator;
#else
    typedef f32_mono_app_fallback_halfband_core_operator f32_mono_app_fast_halfband_core_operator;
#endif

    typedef resampler::f32_stereo_basic_halfband_x2_resampler_core_operator
    f32_stereo_app_fallback_halfband_core_operator;

// f32_stereo_app_fast_halfband_core_operator
#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    typedef resampler::f32_stereo_sse_halfband_x2_resampler_core_operator f32_stereo_app_fast_halfband_core_operator;
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON
    typedef resampler::f32_stereo_neon_halfband_x2_resampler_core_operator f32_stereo_app_fast_halfband_core_operator;
#else
    typedef f32_stereo_app_fallback_halfband_core_operator f32_stereo_app_fast_halfband_core_operator;
#endif

// app_fft_backend_f
#if CXXDASP_USE_FFT_BACKEND_PFFFT
    typedef fft::backend::f::pffft app_fft_backend_f;
#elif CXXDASP_USE_FFT_BACKEND_FFTS
    typedef fft::backend::f::ffts app_fft_backend_f;
#elif CXXDASP_USE_FFT_BACKEND_KISS_FFT
    typedef fft::backend::f::kiss_fft app_fft_backend_f;
#elif CXXDASP_USE_FFT_BACKEND_NE10
    typedef fft::backend::f::ne10 app_fft_backend_f;
#elif CXXDASP_USE_FFT_BACKEND_FFTWF
    typedef fft::backend::f::fftw app_fft_backend_f;
#else
#error No FFT library available
#endif

// app_fft_backend_d
#if CXXDASP_USE_FFT_BACKEND_GP_FFT
    typedef fft::backend::d::gp_fft app_fft_backend_d;
#elif CXXDASP_USE_FFT_BACKEND_FFTW
    typedef fft::backend::d::fftw app_fft_backend_d;
#else
#if CXXPH_COMPILER_IS_MSVC
#pragma message("Double - precision FFT library is not available.")
#else
#warning Double-precision FFT library is not available.
#endif
    typedef app_fft_backend_f app_fft_backend_d; // fall-back  use single-precision
#endif

    typedef datatype::f32_mono_frame_t app_mono_frame_t;
    typedef datatype::f32_stereo_frame_t app_stereo_frame_t;

    if (argc < 7) {
        print_usage(argv[0]);
        return 1;
    }

#ifdef USE_ANDROID_NDK_PROFILER
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON
    ::monstartup("simple-resampler");
#else
    ::monstartup("simple-resampler-no-neon");
#endif
#endif

    // initialize
    cxxdasp_init();

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

        if (quality == 5) {
            // NOTE: Quality 5 requires double precision FFT
            resample<app_mono_frame_t, app_mono_frame_t, f32_mono_app_fast_halfband_core_operator,
                     f32_mono_app_fallback_halfband_core_operator, app_fft_backend_d,
                     f32_mono_app_fast_polyphase_core_operator, f32_mono_app_fallback_polyphase_core_operator>(
                src_data, resampled, 1, src_freq, dest_freq, quality);
        } else {
            // NOTE: Quality 4 or below uses single precision FFT
            resample<app_mono_frame_t, app_mono_frame_t, f32_mono_app_fast_halfband_core_operator,
                     f32_mono_app_fallback_halfband_core_operator, app_fft_backend_f,
                     f32_mono_app_fast_polyphase_core_operator, f32_mono_app_fallback_polyphase_core_operator>(
                src_data, resampled, 1, src_freq, dest_freq, quality);
        }
        write_raw_file(dest_filename, resampled);
    } break;
    case 2: {
        std::vector<app_stereo_frame_t> src_data;
        std::vector<app_stereo_frame_t> resampled;

        read_raw_file(src_filename, src_data);
        if (quality == 5) {
            // NOTE: Quality 5 requires double precision FFT
            resample<app_stereo_frame_t, app_stereo_frame_t, f32_stereo_app_fast_halfband_core_operator,
                     f32_stereo_app_fallback_halfband_core_operator, app_fft_backend_d,
                     f32_stereo_app_fast_polyphase_core_operator, f32_stereo_app_fallback_polyphase_core_operator>(
                src_data, resampled, 2, src_freq, dest_freq, quality);
        } else {
            // NOTE: Quality 4 or below uses single precision FFT
            resample<app_stereo_frame_t, app_stereo_frame_t, f32_stereo_app_fast_halfband_core_operator,
                     f32_stereo_app_fallback_halfband_core_operator, app_fft_backend_f,
                     f32_stereo_app_fast_polyphase_core_operator, f32_stereo_app_fallback_polyphase_core_operator>(
                src_data, resampled, 2, src_freq, dest_freq, quality);
        }
        write_raw_file(dest_filename, resampled);
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
