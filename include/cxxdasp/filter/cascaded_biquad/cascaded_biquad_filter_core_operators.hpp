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

#ifndef CXXDASP_FILTER_CASCADED_BIQUAD_CASCADED_BIQUAD_FILTER_CORE_OPERATORS_HPP_
#define CXXDASP_FILTER_CASCADED_BIQUAD_CASCADED_BIQUAD_FILTER_CORE_OPERATORS_HPP_

#include <cxxporthelper/compiler.hpp>

#include <cxxdasp/filter/cascaded_biquad/general_cascaded_biquad_core_operator.hpp>

// transposed direct form 2
#if CXXPH_COMPILER_SUPPORTS_ARM_NEON
#include <cxxdasp/filter/cascaded_biquad/f32_stereo_neon_cascaded_2_biquad_tdf2_core_operator.hpp>
#endif

#endif // CXXDASP_FILTER_CASCADED_BIQUAD_CASCADED_BIQUAD_FILTER_CORE_OPERATORS_HPP_
