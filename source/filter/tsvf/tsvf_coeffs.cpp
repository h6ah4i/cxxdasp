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

#include <cxxdasp/filter/tsvf/tsvf_coeffs.hpp>

#include <cxxporthelper/cmath>

// ref.) "Solving the continuous SVF equations using trapezoidal integration and equivalent currents"
//          by Andrew Simper  <andy@cytomic.com>
//
//   http://www.cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf

namespace cxxdasp {
namespace filter {

trapezoidal_state_variable_filter_coeffs::trapezoidal_state_variable_filter_coeffs()
    : a1(0.0), a2(0.0), a3(0.0), m0(0.0), m1(0.0), m2(0.0)
{
}

trapezoidal_state_variable_filter_coeffs::~trapezoidal_state_variable_filter_coeffs() {}

bool trapezoidal_state_variable_filter_coeffs::make(const filter_params_t &params)
{
    switch (params.type) {
    case types::LowPass: {
        const double g = std::tan(M_PI * params.f0 / params.fs);
        const double k = 1.0 / params.q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = 0.0;
        m1 = 0.0;
        m2 = 1.0;
    } break;

    case types::BandPass: {
        const double g = std::tan(M_PI * params.f0 / params.fs);
        const double k = 1.0 / params.q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = 0.0;
        m1 = 1.0;
        m2 = 0.0;
    } break;

    case types::HighPass: {
        const double g = std::tan(M_PI * params.f0 / params.fs);
        const double k = 1.0 / params.q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = 1.0;
        m1 = -k;
        m2 = -1.0;
    } break;

    case types::Notch: {
        const double g = std::tan(M_PI * params.f0 / params.fs);
        const double k = 1.0 / params.q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = 1.0;
        m1 = -k;
        m2 = -2.0;
    } break;

    case types::Peak: {
        const double g = std::tan(M_PI * params.f0 / params.fs);
        const double k = 1.0 / params.q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = 1.0;
        m1 = -k;
        m2 = 0.0;
    } break;

    case types::Bell: {
        const double A = std::pow(10.0, params.db_gain / 40.0);
        const double g = std::tan(M_PI * params.f0 / params.fs);
        const double k = 1.0 / (params.q * A);
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = 1.0;
        m1 = k * (A * A - 1.0);
        m2 = 0.0;
    } break;

    case types::LowShelf: {
        const double A = std::pow(10.0, params.db_gain / 40.0);
        const double g = std::tan(M_PI * params.f0 / params.fs);
        const double k = 1.0 / params.q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = 1.0;
        m1 = k * (A - 1.0);
        m2 = (A * A - 1.0);
    } break;

    case types::HighShelf: {
        const double A = std::pow(10.0, params.db_gain / 40.0);
        const double g = std::tan(M_PI * params.f0 / params.fs);
        const double k = 1.0 / params.q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        m0 = A * A;
        m1 = k * (1.0 - A) * A;
        m2 = (1.0 - A * A);
    } break;

    default:
        a1 = 0.0;
        a2 = 0.0;
        a3 = 0.0;
        m0 = 0.0;
        m1 = 0.0;
        m2 = 0.0;
        return false;
    }

    return true;
}

} // namespace filter
} // namespace cxxdasp
