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

#ifndef CXXDASP_UTILS_NEON_DEBUG_UTILS_HPP_
#define CXXDASP_UTILS_NEON_DEBUG_UTILS_HPP_

#include <iostream>

#include <cxxporthelper/arm_neon.hpp>

namespace cxxdasp {
namespace utils {

inline void print_f32x2(const char *label, const float32x2_t &x)
{
    std::cout << label << " = {" << vget_lane_f32(x, 0) << ", " << vget_lane_f32(x, 1) << "}" << std::endl;
}

inline void print_f32x2x2(const char *label, const float32x2x2_t &x)
{
    std::cout << label << " = {" << vget_lane_f32(x.val[0], 0) << ", " << vget_lane_f32(x.val[0], 1) << "}, {"
              << vget_lane_f32(x.val[1], 0) << ", " << vget_lane_f32(x.val[1], 1) << "}" << std::endl;
}

inline void print_f32x2x4(const char *label, const float32x2x4_t &x)
{
    std::cout << label << " = {" << vget_lane_f32(x.val[0], 0) << ", " << vget_lane_f32(x.val[0], 1) << "}, {"
              << vget_lane_f32(x.val[1], 0) << ", " << vget_lane_f32(x.val[1], 1) << "}, {"
              << vget_lane_f32(x.val[2], 0) << ", " << vget_lane_f32(x.val[2], 1) << "}, {"
              << vget_lane_f32(x.val[3], 0) << ", " << vget_lane_f32(x.val[3], 1) << "}" << std::endl;
}

inline void print_f32x4(const char *label, const float32x4_t &x)
{
    std::cout << label << " = {" << vgetq_lane_f32(x, 0) << ", " << vgetq_lane_f32(x, 1) << ", " << vgetq_lane_f32(x, 2)
              << ", " << vgetq_lane_f32(x, 3) << "}" << std::endl;
}

inline void print_f32x4x2(const char *label, const float32x4x2_t &x)
{
    std::cout << label << " = {" << vgetq_lane_f32(x.val[0], 0) << ", " << vgetq_lane_f32(x.val[0], 1) << ", "
              << vgetq_lane_f32(x.val[0], 2) << ", " << vgetq_lane_f32(x.val[0], 3) << "}, {"
              << vgetq_lane_f32(x.val[1], 0) << ", " << vgetq_lane_f32(x.val[1], 1) << ", "
              << vgetq_lane_f32(x.val[1], 2) << ", " << vgetq_lane_f32(x.val[1], 3) << "}" << std::endl;
}

} // namespace cxxdasp
} // namespace utils

#endif // CXXDASP_UTILS_NEON_DEBUG_UTILS_HPP_
