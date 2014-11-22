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

#ifndef CXXDASP_FILTER_DIGITAL_FILTER_HPP_
#define CXXDASP_FILTER_DIGITAL_FILTER_HPP_

#include <cxxporthelper/cmath>
#include <cxxporthelper/cstdint>
#include <cxxporthelper/compiler.hpp>

namespace cxxdasp {
namespace filter {

namespace types {

/**
 * Filter types
 */
typedef enum { Unknown, LowPass, HighPass, BandPass, Notch, Peak, Bell, LowShelf, HighShelf, AllPass, } filter_type_t;
}

typedef types::filter_type_t filter_type_t;

/**
 * General filter parameters
 */
struct filter_params_t {
    filter_type_t type; ///< filter type
    double fs;          ///< sampling frequency [Hz]
    double f0;          ///< target frequency [Hz]
    double db_gain;     ///< gain [dB]
    double q;           ///< quality factor (0.0 : infinity]

    /**
     * Constructor.
     */
    filter_params_t() CXXPH_NOEXCEPT : type(types::LowPass), fs(0), f0(0), db_gain(0), q(0) {}

    /**
     * Constructor.
     *
     * @param type [in] filter type
     * @param fs [in] sampling frequncy [Hz]
     * @param f0 [in] target frequncy [Hz]
     * @param db_gain [in] gain [dB]
     * @param q [in] Q parameter
     */
    filter_params_t(filter_type_t type, double fs, double f0, double db_gain, double q) CXXPH_NOEXCEPT
        : type(type),
          fs(fs),
          f0(f0),
          db_gain(db_gain),
          q(q)
    {
    }

    /**
     * Calculate Q parameter from bandwidth (for peaking, bell and notch filter)
     *
     * @param fs [in] sampling frequncy [Hz]
     * @param f0 [in] target frequncy [Hz]
     * @param bandwidth [in] bandwidth [octaves]
     */
    static double calc_q_from_band_width(double fs, double f0, double bandwidth) CXXPH_NOEXCEPT
    {
        const double w0 = (2.0 * M_PI) * (f0 / fs);
        const double ln_2_h = log(2.0) * 0.5;
        return 0.5 / sinh(ln_2_h * bandwidth * w0 / sin(w0));
    }

    /**
     * Calculate Q parameter from bandwidth (for shelving filter)
     *
     * @param db_gain [in] gain [dB]
     * @param slope [in] slope [octaves]
     */
    static double calc_q_from_slope(double db_gain, double slope) CXXPH_NOEXCEPT
    {
        const double A = pow(10.0, (db_gain / 40.0));
        return 1.0 / sqrt((A + (1.0 / A)) * ((1.0 / slope) - 1.0) + 2.0);
    }
};

} // namespace filter
} // namespace cxxdasp

#endif // CXXDASP_FILTER_DIGITAL_FILTER_HPP_
