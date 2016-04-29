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
#include <string>
#include <random>

#include <cxxdasp/cxxdasp.hpp>
#include <cxxdasp/resampler/halfband/halfband_x2_resampler.hpp>
#include <cxxdasp/resampler/halfband/halfband_x2_core_operators.hpp>
#include <cxxdasp/resampler/fft/fft_x2_resampler.hpp>
#include <cxxdasp/utils/stopwatch.hpp>

#define USE_CXXDASP
#include "example_common.hpp"

#ifdef USE_ANDROID_NDK_PROFILER
#include <prof.h>
#endif

// #define RANDOMIZE_PROCESS_SIZE
// #define VERIFY_RESULT

using namespace cxxdasp;

// coefficient data
static const float g_x2_halfband_coeffs_[] = {
#include "halfband.inc"
};

static const float g_x2_fft_coeffs_[] = {
#include "fft.inc"
};

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
        // std::cout << "src_index: " << src_index << std::endl;
        // std::cout << "dest_index: " << dest_index << std::endl;

        {
            const int src_remains = (n_src - src_index);

            if (src_remains > 0) {
                int n = (std::min)(r.num_can_put(), src_remains);
#ifdef RANDOMIZE_PROCESS_SIZE
                n = std::min(n, dist(engine));
#endif
                // std::cout << "num_can_put: " << n << std::endl;
                r.put_n(&src[src_index], n);
                src_index += n;
            } else {
                break;
            }
        }

        {
            const int dest_remains = (n_dest - dest_index);
            int n = (std::min)(r.num_can_get(), dest_remains);
// std::cout << "num_can_get: " << n << std::endl;
#ifdef RANDOMIZE_PROCESS_SIZE
            n = std::min(n, dist(engine));
#endif
            r.get_n(&dest[dest_index], n);
            dest_index += n;
        }
    }

    // std::cout << "flush()" << std::endl;

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
        // std::cout << "dest_remains = " << dest_remains << std::endl;
        // std::cout << "n = " << n << std::endl;
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

template <typename TSrc, typename TDest, typename TCoreOperator>
static void resample_halfband(const std::vector<TSrc> &src, std::vector<TDest> &dest, int ch)
{
    const int n_src = src.size();
    const int n_dest = n_src * 2;

    typedef resampler::halfband_x2_resampler<TSrc, TDest, float, TCoreOperator> halfband_resmpler_type;
    halfband_resmpler_type r(g_x2_halfband_coeffs_, sizeof(g_x2_halfband_coeffs_) / sizeof(float), 4);

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
              << "Halfband x2, "
              << "CH=" << ch << ") "
              << ": " << sw.get_elapsed_time_us() << " [us]" << std::endl;
}

template <typename TSrc, typename TDest, typename TFFTBackend>
static void resample_fft(const std::vector<TSrc> &src, std::vector<TDest> &dest, int ch)
{
    typedef resampler::fft_x2_resampler<TSrc, TDest, float, TFFTBackend> fft_resampler_type;

    const int n_src = src.size();
    const int n_dest = n_src * 2;

    fft_resampler_type r(g_x2_fft_coeffs_, sizeof(g_x2_fft_coeffs_) / sizeof(float), 2);

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
              << "FFT x2[" << (std::is_same<float, typename TFFTBackend::fft_real_t>::value ? "sp" : "dp") << "], "
              << "CH=" << ch << ") "
              << ": " << sw.get_elapsed_time_us() << " [us]" << std::endl;
}

template <typename TSrc, typename TDest, int Nch>
bool compare(const datatype::audio_frame<TSrc, Nch> &src, const datatype::audio_frame<TDest, Nch> &dest)
{
    for (int i = 0; i < Nch; ++i) {
        if (src.c(i) != dest.c(i)) {
            return false;
        }
    }

    return true;
}

template <typename T, int Nch>
std::ostream &operator<<(std::ostream &os, const datatype::audio_frame<T, Nch> &t)
{

    os << "(";

    for (int i = 0; i < Nch; ++i) {
        if (i > 0) {
            os << ", ";
        }

        os << t.c(i);
    }

    os << ")";

    return os;
}

#ifdef VERIFY_RESULT
template <typename TSrc, typename TDest>
static void verify_halfband_x2_result(const std::vector<TSrc> &src, const std::vector<TDest> &dest, int ch)
{
    int fail_cnt = 0;

    // check size
    if (!(dest.size() == (2 * src.size()))) {
        fail_cnt += 1;
        std::cout << "FAILED: "
                  << "dest.size() = " << dest.size() << ", "
                  << "src.size() = " << src.size() << std::endl;
    }

    // check even position data
    const int n = src.size();
    for (int i = 0; i < n; ++i) {
        if (!(compare(src[i], dest[2 * i]))) {
            fail_cnt += 1;
            std::cout << "FAILED: position = " << (2 * i) << ", src = " << src[i] << ", dest = " << dest[2 * i]
                      << std::endl;
        }
    }

    std::cout << std::endl;
    std::cout << "=================" << std::endl;
    std::cout << "Failures: " << fail_cnt << std::endl;
    std::cout << "=================" << std::endl;
}
#endif

static void print_usage(const char *exe_name)
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    " << exe_name << " resampler_type src_raw_file dest_raw_file num_channels" << std::endl;
    std::cout << std::endl;
    std::cout << "resampler_type: halfband|fft_sp|fft_dp" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char const *argv[])
{
    typedef datatype::f32_mono_frame_t app_mono_frame_t;
    typedef datatype::f32_stereo_frame_t app_stereo_frame_t;

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    typedef resampler::f32_mono_sse_halfband_x2_resampler_core_operator halfband_mono_core_operator_type;
    typedef resampler::f32_stereo_sse_halfband_x2_resampler_core_operator halfband_stereo_core_operator_type;
#elif CXXPH_COMPILER_SUPPORTS_ARM_NEON
    typedef resampler::f32_mono_neon_halfband_x2_resampler_core_operator halfband_mono_core_operator_type;
    typedef resampler::f32_stereo_neon_halfband_x2_resampler_core_operator halfband_stereo_core_operator_type;
#else
    typedef resampler::f32_mono_basic_halfband_x2_resampler_core_operator halfband_mono_core_operator_type;
    typedef resampler::f32_stereo_basic_halfband_x2_resampler_core_operator halfband_stereo_core_operator_type;
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
#elif CXXDASP_USE_FFT_BACKEND_CKFFT
    typedef fft::backend::f::ckfft app_fft_backend_f;
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

    if (argc < 5) {
        print_usage(argv[0]);
        return 1;
    }

#ifdef USE_ANDROID_NDK_PROFILER
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON
    ::monstartup("x2-resampler");
#else
    ::monstartup("x2-resampler-no-neon");
#endif
#endif

    // initialize
    cxxdasp_init();

    const std::string resampler_type = argv[1];
    const char *src_filename = argv[2];
    const char *dest_filename = argv[3];
    const int num_channels = atoi(argv[4]);

    if (!(resampler_type == "halfband" || resampler_type == "fft_sp" || resampler_type == "fft_dp")) {
        std::cerr << "Unsuported resampler type" << std::endl;
        return 1;
    }

    switch (num_channels) {
    case 1: {
        std::vector<app_mono_frame_t> src_data;
        std::vector<app_mono_frame_t> resampled;

        read_raw_file(src_filename, src_data);

        if (resampler_type == "halfband") {
            resample_halfband<app_mono_frame_t, app_mono_frame_t, halfband_mono_core_operator_type>(src_data, resampled,
                                                                                                    1);

#ifdef VERIFY_RESULT
            verify_halfband_x2_result(src_data, resampled, 1);
#endif
        } else if (resampler_type == "fft_sp") {
            resample_fft<app_mono_frame_t, app_mono_frame_t, app_fft_backend_f>(src_data, resampled, 1);
        } else if (resampler_type == "fft_sp") {
            resample_fft<app_mono_frame_t, app_mono_frame_t, app_fft_backend_d>(src_data, resampled, 1);
        }

        write_raw_file(dest_filename, resampled);
    } break;
    case 2: {
        std::vector<app_stereo_frame_t> src_data;
        std::vector<app_stereo_frame_t> resampled;

        read_raw_file(src_filename, src_data);

        if (resampler_type == "halfband") {
            resample_halfband<app_stereo_frame_t, app_stereo_frame_t, halfband_stereo_core_operator_type>(src_data,
                                                                                                          resampled, 2);

#ifdef VERIFY_RESULT
            verify_halfband_x2_result(src_data, resampled, 2);
#endif
        } else if (resampler_type == "fft_sp") {
            resample_fft<app_stereo_frame_t, app_stereo_frame_t, app_fft_backend_f>(src_data, resampled, 2);
        } else if (resampler_type == "fft_dp") {
            resample_fft<app_stereo_frame_t, app_stereo_frame_t, app_fft_backend_d>(src_data, resampled, 2);
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
