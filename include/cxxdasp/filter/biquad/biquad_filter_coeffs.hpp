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

#ifndef CXXDASP_FILTER_BIQUAD_BIQUAD_FILTERCOEFFS_HPP_
#define CXXDASP_FILTER_BIQUAD_BIQUAD_FILTERCOEFFS_HPP_

#include <cxxporthelper/compiler.hpp>

#include <cxxdasp/filter/digital_filter.hpp>

namespace cxxdasp {
namespace filter {

/**
 * Biquad filter coefficients
 */
struct biquad_filter_coeffs {
    /**
     * Constructor.
     */
    biquad_filter_coeffs() CXXPH_NOEXCEPT;

    /**
     * Destructor.
     */
    ~biquad_filter_coeffs();

    /**
     * Make filter coefficient.
     * @param params [in] general filter parameters
     * @returns Success or Failure
     */
    bool make(const filter_params_t &params) CXXPH_NOEXCEPT;

    /**
     * Make filter coefficient.
     * @param b0 [in] b0 (normalized with a0)
     * @param b1 [in] b1 (normalized with a0)
     * @param b2 [in] b2 (normalized with a0)
     * @param a1 [in] a1 (normalized with a0)
     * @param a2 [in] a2 (normalized with a0)
     * @returns Success or Failure
     */
    bool make(double b0, double b1, double b2, double a1, double a2) CXXPH_NOEXCEPT;

    /**
     * Make filter coefficient.
     * @param b0 [in] b0
     * @param b1 [in] b1
     * @param b2 [in] b2
     * @param a0 [in] a0
     * @param a1 [in] a1
     * @param a2 [in] a2
     * @returns Success or Failure
     */
    bool make(double b0, double b1, double b2, double a0, double a1, double a2) CXXPH_NOEXCEPT;

    double b0; ///< "b0" coefficient value (normalized with a0)
    double b1; ///< "b1" coefficient value (normalized with a0)
    double b2; ///< "b2" coefficient value (normalized with a0)
    double a1; ///< "a1" coefficient value (normalized with a0)
    double a2; ///< "a2" coefficient value (normalized with a0)
};

} // namespace filter
} // namespace cxxdasp

#endif // CXXDASP_FILTER_BIQUAD_BIQUAD_FILTERCOEFFS_HPP_
