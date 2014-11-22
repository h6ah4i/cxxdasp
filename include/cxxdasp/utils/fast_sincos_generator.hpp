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

#ifndef CXXDASP_UTILS_FAST_SINCOS_GENERATOR_HPP_
#define CXXDASP_UTILS_FAST_SINCOS_GENERATOR_HPP_

#include <cxxporthelper/cmath>
#include <cxxporthelper/compiler.hpp>

namespace cxxdasp {
namespace utils {

// fast sin/cos generator using trigonometric identities trick
// ref.) http://www.iquilezles.org/www/articles/sincos/sincos.htm
template <typename T>
class fast_sincos_generator {
public:
    fast_sincos_generator(double init_phase, double phase_step)
        : a_(static_cast<T>(cos(phase_step))), b_(static_cast<T>(sin(phase_step))), s_(static_cast<T>(sin(init_phase))),
          c_(static_cast<T>(cos(init_phase)))
    {
    }

    /**
     * Update
     *
     * (phase += phase_step)
     */
    void update() CXXPH_NOEXCEPT
    {
        const T ns = (b_ * c_) + (a_ * s_);
        const T nc = (a_ * c_) - (b_ * s_);
        s_ = ns;
        c_ = nc;
    }

    /**
     * Get sin() value
     *
     * @returns sin(phase)
     */
    T s() const CXXPH_NOEXCEPT { return s_; }

    /**
    * Get cos() value
    *
    * @returns cos(phase)
    */
    T c() const CXXPH_NOEXCEPT { return c_; }

private:
    const T a_;
    const T b_;
    T s_;
    T c_;
};

} // namespace utils
} // namespace cxxdasp

#endif // CXXDASP_UTILS_FAST_SINCOS_GENERATOR_HPP_
