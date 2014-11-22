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

#ifndef CXXDASP_CXXDASP_CONFIG_HPP_
#define CXXDASP_CXXDASP_CONFIG_HPP_

// low quality polyphase resampler
#ifndef CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_LOW_QUALITY
#define CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_LOW_QUALITY 1
#endif

// high quality polyphase resampler
#ifndef CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_HIGH_QUALITY
#define CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_HIGH_QUALITY 1
#endif

// FFT backend - PFFFT
#ifndef CXXDASP_USE_FFT_BACKEND_PFFFT
#define CXXDASP_USE_FFT_BACKEND_PFFFT 0
#endif

// FFT backend - FFTS
#ifndef CXXDASP_USE_FFT_BACKEND_FFTS
#define CXXDASP_USE_FFT_BACKEND_FFTS 0
#endif

// FFT backend - Kiss FFT
#ifndef CXXDASP_USE_FFT_BACKEND_KISS_FFT
#define CXXDASP_USE_FFT_BACKEND_KISS_FFT 0
#endif

// FFT backend - NE10
#ifndef CXXDASP_USE_FFT_BACKEND_NE10
#define CXXDASP_USE_FFT_BACKEND_NE10 0
#endif

// FFT backend - FFTWF
#ifndef CXXDASP_USE_FFT_BACKEND_FFTWF
#define CXXDASP_USE_FFT_BACKEND_FFTWF 0
#endif

// FFT backend - General purpose FFT package
#ifndef CXXDASP_USE_FFT_BACKEND_GP_FFT
#define CXXDASP_USE_FFT_BACKEND_GP_FFT 0
#endif

// FFT backend - FFTW
#ifndef CXXDASP_USE_FFT_BACKEND_FFTW
#define CXXDASP_USE_FFT_BACKEND_FFTW 0
#endif

#endif // CXXDASP_CXXDASP_CONFIG_HPP_
