#
#    Copyright (C) 2014 Haruki Hasegawa
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

#
# Sub directories
#
add_subdirectory(cxxporthelper)

if (CXXDASP_CONFIG_USE_FFT_BACKEND_PFFFT)
    add_subdirectory(pffft)
endif()

if (CXXDASP_CONFIG_USE_FFT_BACKEND_FFTS)
    add_subdirectory(ffts)
endif()

if (CXXDASP_CONFIG_USE_FFT_BACKEND_KISS_FFT)
    add_subdirectory(kiss_fft)
endif()

if (CXXDASP_CONFIG_USE_FFT_BACKEND_FFTWF)
    add_subdirectory(fftwf)
endif()

if (CXXDASP_CONFIG_USE_FFT_BACKEND_NE10)
    add_subdirectory(ne10)
endif()

if (CXXDASP_CONFIG_USE_FFT_BACKEND_CKFFT)
    add_subdirectory(ckfft)
endif()

if (CXXDASP_CONFIG_USE_FFT_BACKEND_MUFFT)
    add_subdirectory(mufft)
endif()

if (CXXDASP_CONFIG_USE_FFT_BACKEND_GP_FFT)
    add_subdirectory(gp_fft)
endif()

if (CXXDASP_CONFIG_USE_FFT_BACKEND_FFTW)
    add_subdirectory(fftw)
endif()

if (CXXDASP_CONFIG_USE_FFT_BACKEND_KFR_F OR CXXDASP_CONFIG_USE_FFT_BACKEND_KFR_D)
    add_subdirectory(kfr)
endif()

if (CXXDASP_BUILD_EXAMPLE_SOXR_RESAMPLER)
    add_subdirectory(soxr)
endif()

if (CXXDASP_BUILD_EXAMPLE_FRESAMPLE_RESAMPLER)
    add_subdirectory(fresample)
endif()

if (CXXDASP_BUILD_EXAMPLE_SECRET_RABBIT_CODE_RESAMPLER)
    add_subdirectory(libsamplerate)
endif()

