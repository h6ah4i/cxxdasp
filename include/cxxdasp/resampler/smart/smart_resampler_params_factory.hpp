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

#ifndef CXXDASP_RESAMPLER_SMART_SMART_RESAMPLER_PARAMS_FACTORY_HPP_
#define CXXDASP_RESAMPLER_SMART_SMART_RESAMPLER_PARAMS_FACTORY_HPP_

#include <cxxporthelper/compiler.hpp>

#include <cxxdasp/resampler/smart/smart_resampler_params.hpp>

namespace cxxdasp {
namespace resampler {

/**
 * Factory class of the smart_resampler_params
 */
class smart_resampler_params_factory {

    /// @cond INTERNAL_FIELD
    smart_resampler_params_factory(const smart_resampler_params_factory &) = delete;
    smart_resampler_params_factory &operator=(const smart_resampler_params_factory &) = delete;
    /// @endcond

public:
    /**
     * Quality specifiers.
     */
    enum quality_spec_t { LowQuality, MidQuality, HighQuality, };

    /**
     * Constructor.
     *
     * @param input_freq [in] input frequency [Hz]
     * @param output_freq [in] output frequency [Hz]
     * @param quality [in] quality specifier
     */
    smart_resampler_params_factory(int input_freq, int output_freq, quality_spec_t quality);

    /**
     * Obtein parameters of smart_resampler
     *
     * @returns reference to smart_resampler_params
     */
    const smart_resampler_params &params() const CXXPH_NOEXCEPT { return params_; }

    /**
     * 'bool' operator
     *
     * @returns whether the parameters are vaild
     */
    operator bool() const CXXPH_NOEXCEPT { return is_valid_; }

private:
    /// @cond INTERNAL_FIELD
    bool is_valid_;
    smart_resampler_params params_;
    /// @endcond
};

} // namespace resampler
} // namespace cxxdasp

#endif // CXXDASP_RESAMPLER_SMART_SMART_RESAMPLER_PARAMS_FACTORY_HPP_
