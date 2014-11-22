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

#ifndef TEST_COMMON_HPP_
#define TEST_COMMON_HPP_

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <vector>
#include <limits>
#include <cstring>
#include <iostream>

#include <cxxdasp/cxxdasp.hpp>
#include <cxxdasp/utils/utils.hpp>
#include <cxxdasp/datatype/audio_frame.hpp>

//
// Custom Matchers
//
inline testing::Matcher<const std::complex<float> &> FloatComplexEquals(const std::complex<float> &x)
{
    using namespace testing;
    return AllOf(Property(&std::complex<float>::real, FloatEq(x.real())),
                 Property(&std::complex<float>::imag, FloatEq(x.imag())));
}

inline testing::Matcher<const std::complex<double> &> DoubleComplexEquals(const std::complex<double> &x)
{
    using namespace testing;
    return AllOf(Property(&std::complex<double>::real, DoubleEq(x.real())),
                 Property(&std::complex<double>::imag, DoubleEq(x.imag())));
}

inline testing::Matcher<const std::complex<float> &> FloatComplexNear(const std::complex<float> &x, float abs_error)
{
    using namespace testing;
    return AllOf(Property(&std::complex<float>::real, FloatNear(x.real(), abs_error)),
                 Property(&std::complex<float>::imag, FloatNear(x.imag(), abs_error)));
}

inline testing::Matcher<const std::complex<double> &> DoubleComplexNear(const std::complex<double> &x, double abs_error)
{
    using namespace testing;
    return AllOf(Property(&std::complex<double>::real, DoubleNear(x.real(), abs_error)),
                 Property(&std::complex<double>::imag, DoubleNear(x.imag(), abs_error)));
}

inline testing::Matcher<const std::complex<float> &> AutoComplexEquals(const std::complex<float> &x)
{
    return FloatComplexEquals(x);
}

inline testing::Matcher<const std::complex<double> &> AutoComplexEquals(const std::complex<double> &x)
{
    return DoubleComplexEquals(x);
}

inline testing::Matcher<const std::complex<float> &> AutoComplexNear(const std::complex<float> &x, float abs_error)
{
    return FloatComplexNear(x, abs_error);
}

inline testing::Matcher<const std::complex<double> &> AutoComplexNear(const std::complex<double> &x, double abs_error)
{
    return DoubleComplexNear(x, abs_error);
}

inline testing::internal::FloatingEqMatcher<float> AutoFloatingPointEquals(float x) { return testing::FloatEq(x); }

inline testing::internal::FloatingEqMatcher<double> AutoFloatingPointEquals(double x) { return testing::DoubleEq(x); }

inline testing::internal::FloatingEqMatcher<float> AutoFloatingPointNear(float x, float abs_error)
{
    return testing::FloatNear(x, abs_error);
}

inline testing::internal::FloatingEqMatcher<double> AutoFloatingPointNear(double x, double abs_error)
{
    return testing::DoubleNear(x, abs_error);
}

MATCHER_P2(MonoAudioFrameNear, expexted, abs_error, "")
{
    return (arg.c(0) >= (expexted.c(0) - abs_error)) && (arg.c(0) <= (expexted.c(0) + abs_error));
}

MATCHER_P2(StereoAudioFrameNear, expexted, abs_error, "")
{
    return ((arg.c(0) >= (expexted.c(0) - abs_error)) && (arg.c(0) <= (expexted.c(0) + abs_error))) &&
           ((arg.c(1) >= (expexted.c(1) - abs_error)) && (arg.c(1) <= (expexted.c(1) + abs_error)));
}

template <typename TDataType>
inline MonoAudioFrameNearMatcherP2<cxxdasp::datatype::audio_frame<TDataType, 1>, TDataType>
AutoAudioFrameNear(const cxxdasp::datatype::audio_frame<TDataType, 1> &x, TDataType abs_error)
{
    return MonoAudioFrameNear(x, abs_error);
}

template <typename TDataType>
inline StereoAudioFrameNearMatcherP2<cxxdasp::datatype::audio_frame<TDataType, 2>, TDataType>
AutoAudioFrameNear(const cxxdasp::datatype::audio_frame<TDataType, 2> &x, TDataType abs_error)
{
    return StereoAudioFrameNear(x, abs_error);
}

template <typename TDataType>
inline MonoAudioFrameNearMatcherP2<cxxdasp::datatype::audio_frame<TDataType, 1>, TDataType>
AutoAudioFrameEq(const cxxdasp::datatype::audio_frame<TDataType, 1> &x)
{
    return MonoAudioFrameNear(x, std::numeric_limits<TDataType>::epsilon() * 2);
}

template <typename TDataType>
inline StereoAudioFrameNearMatcherP2<cxxdasp::datatype::audio_frame<TDataType, 2>, TDataType>
AutoAudioFrameEq(const cxxdasp::datatype::audio_frame<TDataType, 2> &x)
{
    return StereoAudioFrameNear(x, std::numeric_limits<TDataType>::epsilon() * 2);
}

#define ASSERT_FLOAT_COMPLEX_EQ(expexted, actual) ASSERT_THAT((actual), FloatComplexEquals(expexted))

#define ASSERT_DOUBLE_COMPLEX_EQ(expexted, actual) ASSERT_THAT((actual), DoubleComplexEquals(expexted))

#define ASSERT_AUTO_COMPLEX_EQ(expexted, actual) ASSERT_THAT((actual), AutoComplexEquals(expexted))

#define ASSERT_FLOAT_COMPLEX_NEAR(expexted, actual, abs_error)                                                         \
    ASSERT_THAT((actual), FloatComplexNear((expexted), (abs_error)))

#define ASSERT_DOUBLE_COMPLEX_NEAR(expexted, actual, abs_error)                                                        \
    ASSERT_THAT((actual), DoubleComplexNear((expexted), (abs_error)))

#define ASSERT_AUTO_COMPLEX_NEAR(expexted, actual, abs_error)                                                          \
    ASSERT_THAT((actual), AutoComplexNear((expexted), (abs_error)))

#define ASSERT_AUTO_FLOATING_POINT_EQ(expexted, actual) ASSERT_THAT((actual), AutoFloatingPointEquals(expexted))

#define ASSERT_AUTO_FLOATING_POINT_NEAR(expexted, actual, abs_error)                                                   \
    ASSERT_THAT((actual), AutoFloatingPointNear((expexted), (abs_error)))

#define ASSERT_AUTO_AUDIO_FRAME_NEAR(expexted, actual, abs_error)                                                      \
    ASSERT_THAT((actual), AutoAudioFrameNear((expexted), (abs_error)))

#define ASSERT_AUTO_AUDIO_FRAME_EQ(expexted, actual) ASSERT_THAT((actual), AutoAudioFrameEq((expexted)))

//
// Misc.
//
template <typename TData, int NChannels>
inline std::ostream &operator<<(std::ostream &os, const cxxdasp::datatype::audio_frame<TData, NChannels> &frame)
{
    os << "(";
    for (int i = 0; i < NChannels; ++i) {
        if (i > 0) {
            os << ", ";
        }
        os << frame.c(i);
    }
    os << ")";
    return os;
}

#endif // TEST_COMMON_HPP_
