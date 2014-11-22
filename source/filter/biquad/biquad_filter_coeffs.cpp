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

// Bi-quad filter
//
// ref.) "Cookbook formulae for audio EQ biquad filter coefficients"
//          by Robert Bristow-Johnson  <rbj@audioimagination.com>
//       http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt

#include <cxxdasp/filter/biquad/biquad_filter_coeffs.hpp>

#include <cxxporthelper/cmath>

// helper macros
#define CALC_W0(Fs, f0) ((2.0 * M_PI) * (f0) / (Fs))
#define CALC_A(dbGain) (pow(10.0, ((dbGain) * (1.0 / 40.0))))
#define CALC_ALPHA_Q(sin_w0, Q) (sin_w0 / (2.0 * (Q)))
//#define CALC_ALPHA_BW(sin_w0, BW)     (sin_w0 * sinh(log(2.0) / 2 * (BW) * (w0) / sin_w0))
//#define CALC_ALPHA_S(sin_w0, A, S)        (sin_w0 / 2.0 * sqrt(((A) + 1.0 / (A)) * (1.0 / (S) - 1.0) + 2.0))

namespace cxxdasp {
namespace filter {

class biquad_filter_coeffs_helper {
public:
    static bool make(biquad_filter_coeffs &c, double b0, double b1, double b2, double a0, double a1,
                     double a2) CXXPH_NOEXCEPT;
    static bool make_low_pass(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT;
    static bool make_high_pass(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT;
    static bool make_band_pass(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT;
    static bool make_notch(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT;
    static bool make_bell(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT;
    static bool make_low_shelf(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT;
    static bool make_high_shelf(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT;
    static bool make_all_pass(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT;
};

biquad_filter_coeffs::biquad_filter_coeffs() CXXPH_NOEXCEPT : b0(0.0), b1(0.0), b2(0.0), a1(0.0), a2(0.0) {}

biquad_filter_coeffs::~biquad_filter_coeffs() {}

bool biquad_filter_coeffs::make(const filter_params_t &params) CXXPH_NOEXCEPT
{
    switch (params.type) {
    case types::LowPass:
        return biquad_filter_coeffs_helper::make_low_pass(*this, params);
    case types::HighPass:
        return biquad_filter_coeffs_helper::make_high_pass(*this, params);
    case types::BandPass:
        return biquad_filter_coeffs_helper::make_band_pass(*this, params);
    case types::Notch:
        return biquad_filter_coeffs_helper::make_notch(*this, params);
    case types::Peak:
        return biquad_filter_coeffs_helper::make_bell(*this, params);
    case types::Bell:
        return biquad_filter_coeffs_helper::make_bell(*this, params);
    case types::LowShelf:
        return biquad_filter_coeffs_helper::make_low_shelf(*this, params);
    case types::HighShelf:
        return biquad_filter_coeffs_helper::make_high_shelf(*this, params);
    case types::AllPass:
        return biquad_filter_coeffs_helper::make_all_pass(*this, params);
    default:
        break;
    }

    return false;
}

bool biquad_filter_coeffs::make(double b0, double b1, double b2, double a1, double a2) CXXPH_NOEXCEPT
{
    return biquad_filter_coeffs_helper::make(*this, b0, b1, b2, 1.0, a1, a2);
}

bool biquad_filter_coeffs::make(double b0, double b1, double b2, double a0, double a1, double a2) CXXPH_NOEXCEPT
{
    return biquad_filter_coeffs_helper::make(*this, b0, b1, b2, a0, a1, a2);
}

bool biquad_filter_coeffs_helper::make(biquad_filter_coeffs &c, double b0, double b1, double b2, double a0, double a1,
                                       double a2) CXXPH_NOEXCEPT
{
    if (a0 == 0.0)
        return false;

    c.b0 = (b0 / a0);
    c.b1 = (b1 / a0);
    c.b2 = (b2 / a0);
    c.a1 = (a1 / a0);
    c.a2 = (a2 / a0);

    return true;
}

// Low Pass Filter
bool biquad_filter_coeffs_helper::make_low_pass(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT
{
    const double w0 = CALC_W0(params.fs, params.f0);
    const double sin_w0 = sin(w0);
    const double cos_w0 = cos(w0);
    const double alpha = CALC_ALPHA_Q(sin_w0, params.q);

    const double b0 = (1 - cos_w0) / 2;
    const double b1 = 1 - cos_w0;
    const double b2 = (1 - cos_w0) / 2;
    const double a0 = 1 + alpha;
    const double a1 = -2 * cos_w0;
    const double a2 = 1 - alpha;

    return make(c, b0, b1, b2, a0, a1, a2);
}

// High Pass Filter
bool biquad_filter_coeffs_helper::make_high_pass(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT
{
    const double w0 = CALC_W0(params.fs, params.f0);
    const double sin_w0 = sin(w0);
    const double cos_w0 = cos(w0);
    const double alpha = CALC_ALPHA_Q(sin_w0, params.q);

    const double b0 = (1 + cos_w0) / 2;
    const double b1 = -(1 + cos_w0);
    const double b2 = (1 + cos_w0) / 2;
    const double a0 = 1 + alpha;
    const double a1 = -2 * cos_w0;
    const double a2 = 1 - alpha;

    return make(c, b0, b1, b2, a0, a1, a2);
}

// Band Pass Filter (constant skirt gain, peak gain = Q)
bool biquad_filter_coeffs_helper::make_band_pass(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT
{
    const double w0 = CALC_W0(params.fs, params.f0);
    const double sin_w0 = sin(w0);
    const double cos_w0 = cos(w0);
    const double alpha = CALC_ALPHA_Q(sin_w0, params.q);

    const double b0 = params.q * alpha;
    const double b1 = 0;
    const double b2 = -params.q * alpha;
    const double a0 = 1 + alpha;
    const double a1 = -2 * cos_w0;
    const double a2 = 1 - alpha;

    return make(c, b0, b1, b2, a0, a1, a2);
}

// Notch Filter
bool biquad_filter_coeffs_helper::make_notch(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT
{
    const double w0 = CALC_W0(params.fs, params.f0);
    const double sin_w0 = sin(w0);
    const double cos_w0 = cos(w0);
    const double alpha = CALC_ALPHA_Q(sin_w0, params.q);

    const double b0 = 1;
    const double b1 = -2 * cos_w0;
    const double b2 = 1;
    const double a0 = 1 + alpha;
    const double a1 = -2 * cos_w0;
    const double a2 = 1 - alpha;

    return make(c, b0, b1, b2, a0, a1, a2);
}

// All Pass Filter
bool biquad_filter_coeffs_helper::make_all_pass(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT
{
    const double w0 = CALC_W0(params.fs, params.f0);
    const double sin_w0 = sin(w0);
    const double cos_w0 = cos(w0);
    const double alpha = CALC_ALPHA_Q(sin_w0, params.q);

    const double b0 = 1 - alpha;
    const double b1 = -2 * cos_w0;
    const double b2 = 1 + alpha;
    const double a0 = 1 + alpha;
    const double a1 = -2 * cos_w0;
    const double a2 = 1 - alpha;

    return make(c, b0, b1, b2, a0, a1, a2);
}

// Bell filter (Peaking EQ)
bool biquad_filter_coeffs_helper::make_bell(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT
{
    const double w0 = CALC_W0(params.fs, params.f0);
    const double sin_w0 = sin(w0);
    const double cos_w0 = cos(w0);
    const double A = CALC_A(params.db_gain);
    const double alpha = CALC_ALPHA_Q(sin_w0, params.q);
    const double alpha_mul_A = alpha * A;
    const double alpha_div_A = alpha / A;

    const double b0 = 1 + alpha_mul_A;
    const double b1 = -2 * cos_w0;
    const double b2 = 1 - alpha_mul_A;
    const double a0 = 1 + alpha_div_A;
    const double a1 = -2 * cos_w0;
    const double a2 = 1 - alpha_div_A;

    return make(c, b0, b1, b2, a0, a1, a2);
}

// Low Shelf
bool biquad_filter_coeffs_helper::make_low_shelf(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT
{
    const double w0 = CALC_W0(params.fs, params.f0);
    const double sin_w0 = sin(w0);
    const double cos_w0 = cos(w0);
    const double A = CALC_A(params.db_gain);
    const double alpha = CALC_ALPHA_Q(sin_w0, params.q);
    const double sqrt_A = sqrt(A);
    const double d_sqrt_A_alpha = 2 * sqrt_A * alpha;
    const double b0_b2_common = (A + 1) - (A - 1) * cos_w0;
    const double a0_a2_common = (A + 1) + (A - 1) * cos_w0;
    const double b1_a1_common = (A + 1) * cos_w0;

    const double b0 = A * (b0_b2_common + d_sqrt_A_alpha);
    const double b1 = 2 * A * ((A - 1) - b1_a1_common);
    const double b2 = A * (b0_b2_common - d_sqrt_A_alpha);
    const double a0 = a0_a2_common + d_sqrt_A_alpha;
    const double a1 = -2 * ((A - 1) + b1_a1_common);
    const double a2 = a0_a2_common - d_sqrt_A_alpha;

    return make(c, b0, b1, b2, a0, a1, a2);
}

// High Shelf
bool biquad_filter_coeffs_helper::make_high_shelf(biquad_filter_coeffs &c, const filter_params_t &params) CXXPH_NOEXCEPT
{
    const double w0 = CALC_W0(params.fs, params.f0);
    const double sin_w0 = sin(w0);
    const double cos_w0 = cos(w0);
    const double A = CALC_A(params.db_gain);
    const double alpha = CALC_ALPHA_Q(sin_w0, params.q);
    const double sqrt_A = sqrt(A);
    const double d_sqrt_A_alpha = 2 * sqrt_A * alpha;
    const double b0_b2_common = (A + 1) + (A - 1) * cos_w0;
    const double a0_a2_common = (A + 1) - (A - 1) * cos_w0;
    const double b1_a1_common = (A + 1) * cos_w0;

    const double b0 = A * (b0_b2_common + d_sqrt_A_alpha);
    const double b1 = -2 * A * ((A - 1) + b1_a1_common);
    const double b2 = A * (b0_b2_common - d_sqrt_A_alpha);
    const double a0 = a0_a2_common + d_sqrt_A_alpha;
    const double a1 = 2 * ((A - 1) - b1_a1_common);
    const double a2 = a0_a2_common - d_sqrt_A_alpha;

    return make(c, b0, b1, b2, a0, a1, a2);
}

} // namespace filter
} // namespace cxxadsp
