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

#ifndef CXXDASP_RESAMPLER_HALFBAND_HALFBAND_X2_CORE_OPERATORS_HPP_
#define CXXDASP_RESAMPLER_HALFBAND_HALFBAND_X2_CORE_OPERATORS_HPP_

#include <cxxporthelper/compiler.hpp>

// basic implementation
#include <cxxdasp/resampler/halfband/general_halfband_x2_resampler_core_operator.hpp>

// SSE optimized implementation
#if CXXPH_COMPILER_SUPPORTS_X86_SSE
#include <cxxdasp/resampler/halfband/f32_mono_sse_halfband_x2_resampler_core_operator.hpp>
#include <cxxdasp/resampler/halfband/f32_stereo_sse_halfband_x2_resampler_core_operator.hpp>
#endif

// NEON optimized implementation
#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64)
#include <cxxdasp/resampler/halfband/f32_mono_neon_halfband_x2_resampler_core_operator.hpp>
#include <cxxdasp/resampler/halfband/f32_stereo_neon_halfband_x2_resampler_core_operator.hpp>
#endif

#endif // CXXDASP_RESAMPLER_HALFBAND_HALFBAND_X2_CORE_OPERATORS_HPP_
