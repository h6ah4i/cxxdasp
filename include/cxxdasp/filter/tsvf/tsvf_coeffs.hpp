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

#ifndef CXXDASP_FILTER_TSVF_TSVF_COEFFS_HPP_
#define CXXDASP_FILTER_TSVF_TSVF_COEFFS_HPP_

#include <cxxdasp/filter/digital_filter.hpp>

namespace cxxdasp {
namespace filter {

/**
 * Linear Trapezoidal Integrated State Variable Filter coefficients
 */
struct trapezoidal_state_variable_filter_coeffs {

    /**
     * Constructor.
     */
    trapezoidal_state_variable_filter_coeffs();

    /**
     * Destructor.
     */
    ~trapezoidal_state_variable_filter_coeffs();

    /**
     * Make filter coefficient.
     * @param params [in] general filter parameters
     * @returns Success or Failure
     */
    bool make(const filter_params_t &params);

    double a1; ///< "a1" coefficient valuue
    double a2; ///< "a2" coefficient valuue
    double a3; ///< "a3" coefficient valuue
    double m0; ///< "m0" coefficient valuue
    double m1; ///< "m1" coefficient valuue
    double m2; ///< "m2" coefficient valuue
};

} // namespace filter
} // namespace cxxdasp

#endif // CXXDASP_FILTER_TSVF_TSVF_COEFFS_HPP_
