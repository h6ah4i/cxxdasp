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

#ifndef CXXDASP_UTILS_STOPWATCH_HPP_
#define CXXDASP_UTILS_STOPWATCH_HPP_

#include <cxxporthelper/cstdint>
#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/platform_info.hpp>

#if (CXXPH_TARGET_PLATFORM == CXXPH_PLATFORM_LINUX) || (CXXPH_TARGET_PLATFORM == CXXPH_PLATFORM_ANDROID) ||            \
    (CXXPH_TARGET_PLATFORM == CXXPH_PLATFORM_UNIX)

#include <cxxporthelper/time.hpp>

namespace cxxdasp {
namespace utils {

/**
 * Stopwatch utility class
 */
class stopwatch {
public:
    /**
     * Start.
     */
    void start() CXXPH_NOEXCEPT { ::clock_gettime(CLOCK_MONOTONIC, &ts_); }

    /**
     * Stop.
     */
    void stop() CXXPH_NOEXCEPT { ::clock_gettime(CLOCK_MONOTONIC, &te_); }

    /**
     * Get elapes time.
     * @returns elapsed time [us]
     */
    int get_elapsed_time_us() const CXXPH_NOEXCEPT
    {
        long long t = 0;
        t += (te_.tv_sec - ts_.tv_sec) * 1000000LL;
        t += (te_.tv_nsec - ts_.tv_nsec) / 1000;
        return static_cast<int>(t);
    }

private:
    /// @cond INTERNAL_FIELD
    ::timespec ts_, te_;
    /// @endcond
};

} // namespace utils
} // namespace cxxdasp

#elif(CXXPH_TARGET_PLATFORM == CXXPH_PLATFORM_OSX) || (CXXPH_TARGET_PLATFORM == CXXPH_PLATFORM_IOS) ||                 \
    (CXXPH_TARGET_PLATFORM == CXXPH_PLATFORM_IOS_SIMULATOR)

// for OSX (and iOS; not tested yet)
#include <mach/mach_time.h>

namespace cxxdasp {
namespace utils {

class stopwatch {
public:
    /**
     * Start.
     */
    void start() CXXPH_NOEXCEPT { ts_ = ::mach_absolute_time(); }

    /**
     * Stop.
     */
    void stop() CXXPH_NOEXCEPT { te_ = ::mach_absolute_time(); }

    /**
     * Get elapes time.
     * @returns elapsed time [us]
     */
    int get_elapsed_time_us() const CXXPH_NOEXCEPT
    {
        mach_timebase_info_data_t tb;
        ::mach_timebase_info(&tb);
        uint64_t nsec = (te_ - ts_) * tb.numer / tb.denom;
        return static_cast<int>(nsec / 1000);
    }

private:
    /// @cond INTERNAL_FIELD
    uint64_t ts_, te_;
    /// @endcond
};

} // namespace utils
} // namespace cxxdasp

#elif(CXXPH_TARGET_PLATFORM == CXXPH_PLATFORM_WINDOWS)

// for Windows
#include <windows.h>

namespace cxxdasp {
namespace utils {

class stopwatch {
public:
    /**
     * Start.
     */
    void start() CXXPH_NOEXCEPT { ::QueryPerformanceCounter(&ts_); }

    /**
     * Stop.
     */
    void stop() CXXPH_NOEXCEPT { ::QueryPerformanceCounter(&te_); }

    /**
     * Get elapes time.
     * @returns elapsed time [us]
     */
    int get_elapsed_time_us() const CXXPH_NOEXCEPT
    {
        LARGE_INTEGER freq;
        ::QueryPerformanceFrequency(&freq);
        __int64 usec = (te_.QuadPart - ts_.QuadPart) * 1000000 / freq.QuadPart;
        return static_cast<int>(usec);
    }

private:
    /// @cond INTERNAL_FIELD
    LARGE_INTEGER ts_, te_;
    /// @endcond
};

} // namespace utils
} // namespace cxxdasp

#else
#error
#endif

#endif // CXXDASP_UTILS_STOPWATCH_HPP_
