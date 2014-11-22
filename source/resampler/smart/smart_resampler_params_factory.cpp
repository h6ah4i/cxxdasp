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

#include <cxxdasp/resampler/smart/smart_resampler_params_factory.hpp>
#include <cxxdasp/resampler/polyphase/polyphase_resampler.hpp>

#include <cxxporthelper/compiler.hpp>

//
// helper macros
//
#define IS_POW_OF_TWO(x) (((x) & ((x) - 1)) == 0)

#define VERIFY_FFT_FIR_COEFFS_SIZE(filter_var_name)                                                                    \
    static_assert(IS_POW_OF_TWO((sizeof(filter_var_name) / sizeof(float))), "filter size is not power of two");

#define VERIFY_POLY_FIR_COEFFS_SIZE(filter_var_name, m)                                                                \
    static_assert((((sizeof(filter_var_name) / sizeof(float)) % (m)) == 0),                                            \
                  "filter size is not multiple of M(" #m ")");                                                         \
    /* To keep 16 byte alignment (for SSE ops) */                                                                      \
    static_assert((((sizeof(filter_var_name) / (m)) % (sizeof(float) * 4)) == 0),                                      \
                  "filter size is not multiple of (M * 4)");

#define DECL_HALFBAND_FIR_INFO(var_name)                                                                               \
    {                                                                                                                  \
        true, (var_name), (sizeof(var_name) / sizeof((var_name)[0])), false                                            \
    }

#define DECL_FFT_FIR_INFO(var_name)                                                                                    \
    {                                                                                                                  \
        true, (var_name), (sizeof(var_name) / sizeof((var_name)[0])), true                                             \
    }

#define DECL_POLY_FIR_INFO(var_name, m, l)                                                                             \
    {                                                                                                                  \
        true, (var_name), (sizeof(var_name) / sizeof((var_name)[0])), (m), (l)                                         \
    }

#define DECL_NULL_POLY_FIR_INFO()                                                                                      \
    {                                                                                                                  \
        false, nullptr, 0, 0, 0                                                                                        \
    }

#if CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_LOW_QUALITY
#define DECL_LQ_POLY_FIR_INFO(var_name, m, l) DECL_POLY_FIR_INFO(var_name, m, l)
#else
#define DECL_LQ_POLY_FIR_INFO(var_name, m, l) DECL_NULL_POLY_FIR_INFO()
#endif

#if CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_HIGH_QUALITY
#define DECL_HQ_POLY_FIR_INFO(var_name, m, l) DECL_POLY_FIR_INFO(var_name, m, l)
#else
#define DECL_HQ_POLY_FIR_INFO(var_name, m, l) DECL_NULL_POLY_FIR_INFO()
#endif

#define DECL_ALIGNED_COEFFS CXXPH_ALIGNAS(CXXDASP_POLYPHASE_RESAMPLER_COEFFS_ARRAY_ALIGN_SIZE)

namespace cxxdasp {
namespace resampler {

typedef smart_resampler_params::stage1_x2_fir_info stage1_x2_fir_info;
typedef smart_resampler_params::stage2_poly_fir_info stage2_poly_fir_info;

//
// stage 1 (FFT) filter coefficients
//
DECL_ALIGNED_COEFFS static const float stage1_fft_coeffs_nyq[] = {
#include "resampler/smart/coeffs/stage1_fft_nyq.inc"
};

DECL_ALIGNED_COEFFS static const float stage1_fft_coeffs_half_nyq[] = {
#include "resampler/smart/coeffs/stage1_fft_half_nyq.inc"
};

DECL_ALIGNED_COEFFS static const float stage1_fft_coeffs_48000_44100[] = {
#include "resampler/smart/coeffs/stage1_fft_48000_44100.inc"
};

DECL_ALIGNED_COEFFS static const float stage1_fft_coeffs_88200_48000[] = {
#include "resampler/smart/coeffs/stage1_fft_88200_48000.inc"
};

DECL_ALIGNED_COEFFS static const float stage1_fft_coeffs_96000_44100[] = {
#include "resampler/smart/coeffs/stage1_fft_96000_44100.inc"
};

//
// stage 1 (halfband) filter coefficients
//
DECL_ALIGNED_COEFFS static const float stage1_halfband_coeffs_lq[] = {
#include "resampler/smart/coeffs/stage1_halfband_lq.inc"
};

DECL_ALIGNED_COEFFS static const float stage1_halfband_coeffs_mq[] = {
#include "resampler/smart/coeffs/stage1_halfband_mq.inc"
};

//
// stage 2 filter coefficients
//
DECL_ALIGNED_COEFFS static const float stage2_coeffs_m1[] = { 1.0f };

#if CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_LOW_QUALITY
DECL_ALIGNED_COEFFS static const float stage2_coeffs_lq_m3[] = {
#include "resampler/smart/coeffs/stage2_lq_m3.inc"
};

DECL_ALIGNED_COEFFS static const float stage2_coeffs_lq_m80[] = {
#include "resampler/smart/coeffs/stage2_lq_m80.inc"
};

DECL_ALIGNED_COEFFS static const float stage2_coeffs_lq_m441[] = {
#include "resampler/smart/coeffs/stage2_lq_m441.inc"
};
#endif

#if CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_HIGH_QUALITY
DECL_ALIGNED_COEFFS static const float stage2_coeffs_hq_m3[] = {
#include "resampler/smart/coeffs/stage2_hq_m3.inc"
};

DECL_ALIGNED_COEFFS static const float stage2_coeffs_hq_m80[] = {
#include "resampler/smart/coeffs/stage2_hq_m80.inc"
};

DECL_ALIGNED_COEFFS static const float stage2_coeffs_hq_m441[] = {
#include "resampler/smart/coeffs/stage2_hq_m441.inc"
};
#endif

// verify filter coefficients
VERIFY_FFT_FIR_COEFFS_SIZE(stage1_fft_coeffs_nyq);
VERIFY_FFT_FIR_COEFFS_SIZE(stage1_fft_coeffs_half_nyq);
VERIFY_FFT_FIR_COEFFS_SIZE(stage1_fft_coeffs_48000_44100);
VERIFY_FFT_FIR_COEFFS_SIZE(stage1_fft_coeffs_88200_48000);
VERIFY_FFT_FIR_COEFFS_SIZE(stage1_fft_coeffs_96000_44100);

#if CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_LOW_QUALITY
VERIFY_POLY_FIR_COEFFS_SIZE(stage2_coeffs_lq_m3, 3);
VERIFY_POLY_FIR_COEFFS_SIZE(stage2_coeffs_lq_m80, 80);
VERIFY_POLY_FIR_COEFFS_SIZE(stage2_coeffs_lq_m441, 441);
#endif

#if CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_HIGH_QUALITY
VERIFY_POLY_FIR_COEFFS_SIZE(stage2_coeffs_hq_m3, 3);
VERIFY_POLY_FIR_COEFFS_SIZE(stage2_coeffs_hq_m80, 80);
VERIFY_POLY_FIR_COEFFS_SIZE(stage2_coeffs_hq_m441, 441);
#endif

//
// filter info
//
static const stage1_x2_fir_info stage1_halfband_lq = DECL_HALFBAND_FIR_INFO(stage1_halfband_coeffs_lq);
static const stage1_x2_fir_info stage1_halfband_mq = DECL_HALFBAND_FIR_INFO(stage1_halfband_coeffs_mq);

static const stage1_x2_fir_info stage1_fft_nyq = DECL_FFT_FIR_INFO(stage1_fft_coeffs_nyq);
static const stage1_x2_fir_info stage1_fft_half_nyq = DECL_FFT_FIR_INFO(stage1_fft_coeffs_half_nyq);
static const stage1_x2_fir_info stage1_fft_48000_44100 = DECL_FFT_FIR_INFO(stage1_fft_coeffs_48000_44100);
static const stage1_x2_fir_info stage1_fft_88200_48000 = DECL_FFT_FIR_INFO(stage1_fft_coeffs_88200_48000);
static const stage1_x2_fir_info stage1_fft_96000_44100 = DECL_FFT_FIR_INFO(stage1_fft_coeffs_96000_44100);

static const stage2_poly_fir_info stage2_m1_l1[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_m1, 1, 1),
                                                     DECL_HQ_POLY_FIR_INFO(stage2_coeffs_m1, 1, 1), };
static const stage2_poly_fir_info stage2_m1_l4[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_m1, 1, 4),
                                                     DECL_HQ_POLY_FIR_INFO(stage2_coeffs_m1, 1, 4), };
static const stage2_poly_fir_info stage2_m3_l1[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_lq_m3, 3, 1),
                                                     DECL_HQ_POLY_FIR_INFO(stage2_coeffs_hq_m3, 3, 1), };
static const stage2_poly_fir_info stage2_m3_l2[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_lq_m3, 3, 2),
                                                     DECL_HQ_POLY_FIR_INFO(stage2_coeffs_hq_m3, 3, 2), };
static const stage2_poly_fir_info stage2_m3_l4[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_lq_m3, 3, 4),
                                                     DECL_HQ_POLY_FIR_INFO(stage2_coeffs_hq_m3, 3, 4), };
static const stage2_poly_fir_info stage2_m80_l147[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_lq_m80, 80, 147),
                                                        DECL_HQ_POLY_FIR_INFO(stage2_coeffs_hq_m80, 80, 147), };
static const stage2_poly_fir_info stage2_m80_l294[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_lq_m80, 80, 294),
                                                        DECL_HQ_POLY_FIR_INFO(stage2_coeffs_hq_m80, 80, 294), };
static const stage2_poly_fir_info stage2_m441_l160[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_lq_m441, 441, 160),
                                                         DECL_HQ_POLY_FIR_INFO(stage2_coeffs_hq_m441, 441, 160), };
static const stage2_poly_fir_info stage2_m441_l320[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_lq_m441, 441, 320),
                                                         DECL_HQ_POLY_FIR_INFO(stage2_coeffs_hq_m441, 441, 320), };
static const stage2_poly_fir_info stage2_m441_l640[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_lq_m441, 441, 640),
                                                         DECL_HQ_POLY_FIR_INFO(stage2_coeffs_hq_m441, 441, 640), };
static const stage2_poly_fir_info stage2_m441_l960[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_lq_m441, 441, 960),
                                                         DECL_HQ_POLY_FIR_INFO(stage2_coeffs_hq_m441, 441, 960), };
static const stage2_poly_fir_info stage2_m441_l1920[] = { DECL_LQ_POLY_FIR_INFO(stage2_coeffs_lq_m441, 441, 1920),
                                                          DECL_HQ_POLY_FIR_INFO(stage2_coeffs_hq_m441, 441, 1920), };

//
// utilities
//
static const stage1_x2_fir_info *get_stage1_filter_info(int input_freq, int output_freq,
                                                        smart_resampler_params_factory::quality_spec_t quality)
{
    if (output_freq == input_freq) {
        return nullptr;
    }

    if (quality == smart_resampler_params_factory::LowQuality) {
        return &stage1_halfband_lq;
    }

    if (quality == smart_resampler_params_factory::MidQuality) {
        if (output_freq > input_freq) {
            return &stage1_halfband_mq;
        }
    }

    if ((output_freq * 2) == input_freq) {
        return &stage1_fft_half_nyq;
    } else if (output_freq > input_freq) {
        return &stage1_fft_nyq;
    } else if ((input_freq == 48000) && (output_freq == 44100)) {
        return &stage1_fft_48000_44100;
    } else if ((input_freq == 88200) && (output_freq == 48000)) {
        return &stage1_fft_88200_48000;
    } else if ((input_freq == 96000) && (output_freq == 44100)) {
        return &stage1_fft_96000_44100;
    }

    return nullptr;
}

static const stage2_poly_fir_info *get_stage2_filter_info(int input_freq, int output_freq,
                                                          smart_resampler_params_factory::quality_spec_t quality)
{
    const stage2_poly_fir_info *info = nullptr;

    switch (output_freq) {
    case 44100:
        switch (input_freq) {
        case 8000:
            info = stage2_m441_l160;
            break;
        case 16000:
            info = stage2_m441_l320;
            break;
        case 32000:
            info = stage2_m441_l640;
            break;
        case 44100:
            info = stage2_m1_l1;
            break;
        case 48000:
            info = stage2_m441_l960;
            break;
        case 88200:
            info = stage2_m1_l4;
            break;
        case 96000:
            info = stage2_m441_l1920;
            break;
        default:
            break;
        }
        break;
    case 48000:
        switch (input_freq) {
        case 8000:
            info = stage2_m3_l1;
            break;
        case 16000:
            info = stage2_m3_l2;
            break;
        case 32000:
            info = stage2_m3_l4;
            break;
        case 44100:
            info = stage2_m80_l147;
            break;
        case 48000:
            info = stage2_m1_l1;
            break;
        case 88200:
            info = stage2_m80_l294;
            break;
        case 96000:
            info = stage2_m1_l4;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    if (!info)
        return nullptr;

    const int q_index = (quality == smart_resampler_params_factory::LowQuality) ? 0 : 1;

    if (!(info[q_index].coeffs))
        return nullptr;

    return &(info[q_index]);
}

//
// smart_resampler_params_factory
//
smart_resampler_params_factory::smart_resampler_params_factory(int input_freq, int output_freq, quality_spec_t quality)
    : is_valid_(false), params_()
{
    const stage1_x2_fir_info *s1 = get_stage1_filter_info(input_freq, output_freq, quality);
    const stage2_poly_fir_info *s2 = get_stage2_filter_info(input_freq, output_freq, quality);

    if (!((s1 && s2) || (s2))) {
        return;
    }

    if (s1) {
        params_.stage1 = (*s1);
        params_.have_stage1 = true;
    }

    if (s2) {
        params_.stage2 = *s2;
        params_.have_stage2 = true;
    }

    is_valid_ = true;
}

} // namespace resampler
} // namespace cxxdasp
