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

#ifndef CXXDASP_DATATYPE_AUDIO_FRAME_HPP_
#define CXXDASP_DATATYPE_AUDIO_FRAME_HPP_

#include <cxxporthelper/compiler.hpp>
#include <cxxporthelper/cstdint>

namespace cxxdasp {
namespace datatype {

/**
* Audio data frame. (POD)
*
* @tparam Tdata data type of audio samle
* @tparam Nchannels number of channels per frame
*/
template <typename TData, int NChannels>
struct pod_audio_frame {
    /// @cond INTERNAL_FIELD
    typedef pod_audio_frame<TData, NChannels> self_type;
    typedef TData data_type;
#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
    static constexpr int num_channels = NChannels;
    static constexpr int size = sizeof(data_type) * num_channels;
#else
    enum { num_channels = NChannels };
    enum { size = sizeof(data_type) * num_channels };
#endif
    /// @endcond

    data_type sample[num_channels];
};

/**
 * Audio data frame. (non-trivial)
 *
 * @tparam Tdata data type of audio samle
 * @tparam Nchannels number of channels per frame
 */
template <typename TData, int NChannels>
struct audio_frame {
    /// @cond INTERNAL_FIELD
    typedef audio_frame<TData, NChannels> self_type;
    typedef pod_audio_frame<TData, NChannels> pod_type;
    typedef TData data_type;
#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
    static constexpr int num_channels = NChannels;
    static constexpr int size = sizeof(data_type) * num_channels;
#else
    enum { num_channels = NChannels };
    enum { size = sizeof(data_type) * num_channels };
#endif
    /// @endcond

    /**
     * Samples.
     */
    data_type sample[num_channels];

    /**
     * Default constructor.
     */
    audio_frame() CXXPH_NOEXCEPT
    {
        for (auto &s : sample)
            s = 0;
    }

    /**
     * Constructor.
     *
     * @param [in] value  initial value of all samples.
     */
    audio_frame(const TData &value) CXXPH_NOEXCEPT
    {
        for (auto &s : sample) {
            s = value;
        }
    }

    /**
     * Access sample data
     *
     * @param [in] channel  channel
     */
    TData &c(int channel) CXXPH_NOEXCEPT { return sample[channel]; }

    /**
     * Access sample data
     *
     * @param [in] channel  channel
     */
    const TData &c(int channel) const CXXPH_NOEXCEPT { return sample[channel]; }

    /// @cond INTERNAL_FIELD
    template <typename T>
    audio_frame &operator+=(T s) CXXPH_NOEXCEPT
    {
        for (int ch = 0; ch < num_channels; ++ch) {
            c(ch) += s;
        }
        return (*this);
    }

    template <typename T>
    audio_frame operator+(T s) const CXXPH_NOEXCEPT
    {
        audio_frame t = (*this);
        t += s;
        return t;
    }

    template <typename T>
    audio_frame &operator-=(T s) CXXPH_NOEXCEPT
    {
        for (int ch = 0; ch < num_channels; ++ch) {
            c(ch) -= s;
        }
        return (*this);
    }

    template <typename T>
    audio_frame operator-(T s) const CXXPH_NOEXCEPT
    {
        audio_frame t = (*this);
        t -= s;
        return t;
    }

    template <typename T>
    audio_frame &operator*=(T s) CXXPH_NOEXCEPT
    {
        for (int ch = 0; ch < num_channels; ++ch) {
            c(ch) *= s;
        }
        return (*this);
    }

    template <typename T>
    audio_frame operator*(T s) const CXXPH_NOEXCEPT
    {
        audio_frame t = (*this);
        t *= s;
        return t;
    }

    template <typename T>
    audio_frame &operator/=(T s) CXXPH_NOEXCEPT
    {
        for (int ch = 0; ch < num_channels; ++ch) {
            c(ch) /= s;
        }
        return (*this);
    }

    template <typename T>
    audio_frame operator/(T s) const CXXPH_NOEXCEPT
    {
        audio_frame t = (*this);
        t /= s;
        return t;
    }

    audio_frame &operator+=(const audio_frame &af) CXXPH_NOEXCEPT
    {
        for (int ch = 0; ch < num_channels; ++ch) {
            c(ch) += af.c(ch);
        }
        return (*this);
    }

    audio_frame operator+(const audio_frame &af) const CXXPH_NOEXCEPT
    {
        audio_frame t = (*this);
        t += af;
        return t;
    }
    audio_frame &operator-=(const audio_frame &af) CXXPH_NOEXCEPT
    {
        for (int ch = 0; ch < num_channels; ++ch) {
            c(ch) -= af.c(ch);
        }
        return (*this);
    }

    audio_frame operator-(const audio_frame &af) const CXXPH_NOEXCEPT
    {
        audio_frame t = (*this);
        t -= af;
        return t;
    }

    audio_frame &operator*=(const audio_frame &af) CXXPH_NOEXCEPT
    {
        for (int ch = 0; ch < num_channels; ++ch) {
            c(ch) *= af.c(ch);
        }
        return (*this);
    }

    audio_frame operator*(const audio_frame &af) const CXXPH_NOEXCEPT
    {
        audio_frame t = (*this);
        t *= af;
        return t;
    }

    audio_frame &operator/=(const audio_frame &af) CXXPH_NOEXCEPT
    {
        for (int ch = 0; ch < num_channels; ++ch) {
            c(ch) /= af.c(ch);
        }
        return (*this);
    }

    audio_frame operator/(const audio_frame &af) const CXXPH_NOEXCEPT
    {
        audio_frame t = (*this);
        t /= af;
        return t;
    }
    /// @endcond
};

//
// Global operators
//
template <typename TScalar, typename TData, int Nchannels>
audio_frame<TData, Nchannels> operator+(TScalar s, const audio_frame<TData, Nchannels> &af)
{
    return (af + s);
}

template <typename TScalar, typename TData, int Nchannels>
audio_frame<TData, Nchannels> operator*(TScalar s, const audio_frame<TData, Nchannels> &af)
{
    return (af * s);
}

//
// well-known types
//
typedef audio_frame<int16_t, 1> s16_mono_frame_t;
typedef audio_frame<int16_t, 2> s16_stereo_frame_t;

typedef audio_frame<float, 1> f32_mono_frame_t;
typedef audio_frame<float, 2> f32_stereo_frame_t;

typedef audio_frame<double, 1> f64_mono_frame_t;
typedef audio_frame<double, 2> f64_stereo_frame_t;

} // namespace datatype
} // namespace cxxdasp

#endif // CXXDASP_DATATYPE_AUDIO_FRAME_HPP_
