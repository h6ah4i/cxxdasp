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

#ifndef CXXDASP_MIXER_MIXER_CORE_OPERATORS_HPP_
#define CXXDASP_MIXER_MIXER_CORE_OPERATORS_HPP_

#include <cxxporthelper/compiler.hpp>

#include <cxxdasp/mixer/general_mixer_core_operator.hpp>

#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
#include <cxxdasp/mixer/f32_mono_sse_mixer_core_operator.hpp>
#include <cxxdasp/mixer/f32_stereo_sse_mixer_core_operator.hpp>
#endif

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON
#include <cxxdasp/mixer/f32_mono_neon_mixer_core_operator.hpp>
#include <cxxdasp/mixer/f32_stereo_neon_mixer_core_operator.hpp>
#endif

#endif // CXXDASP_MIXER_MIXER_CORE_OPERATORS_HPP_
