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

MY_DIR = $(call my-dir)

#
# definitions
#
# NOTE: this function is used to avoid MATH_PATH(=260) issue on Windows...
cxxdasp-normalize-path = $(shell $(HOST_PYTHON) -c "import os.path; print os.path.abspath(\"$(firstword $1)\").replace(\"\\\\\", \"/\")")

#
# load config file
#
ifndef CXXDASP_OVERRIDE_CONFIG_FILE
include $(MY_DIR)/cxxdasp-config.mk
endif

#
# register module directory
#
$(call import-add-path, $(MY_DIR)/../modules)

#
# initialize internal variables
#
CXXDASP_FFT_BACKEND_LIBS_armeabi :=
CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a :=
CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a-no-neon :=
CXXDASP_FFT_BACKEND_LIBS_arm64-v8a :=
CXXDASP_FFT_BACKEND_LIBS_x86 :=
CXXDASP_FFT_BACKEND_LIBS_x86_64 :=
CXXDASP_FFT_BACKEND_LIBS_mips :=
CXXDASP_FFT_BACKEND_LIBS_mips64 :=

ifeq ($(CXXDASP_USE_FFT_BACKEND_FFTS), 1)
    # FFTS is only available on armeabi-v7a
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a            += ffts_static
endif

ifeq ($(CXXDASP_USE_FFT_BACKEND_PFFFT), 1)
    # PFFFT is available on all platforms
    CXXDASP_FFT_BACKEND_LIBS_armeabi                += pffft-no-simd_static
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a            += pffft_static
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a-no-neon    += pffft-no-simd_static # NOTE: requires -no-simd suffixed version
    CXXDASP_FFT_BACKEND_LIBS_arm64-v8a              += pffft-no-simd_static
    CXXDASP_FFT_BACKEND_LIBS_x86                    += pffft_static
    CXXDASP_FFT_BACKEND_LIBS_x86_64                 += pffft_static
    CXXDASP_FFT_BACKEND_LIBS_mips                   += pffft-no-simd_static
    CXXDASP_FFT_BACKEND_LIBS_mips64                 += pffft-no-simd_static
endif

ifeq ($(CXXDASP_USE_FFT_BACKEND_KISS_FFT), 1)
    # Kiss FFT is available on all platforms
    CXXDASP_FFT_BACKEND_LIBS_armeabi                += kiss_fft_static
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a            += kiss_fft_static
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a-no-neon    += kiss_fft_static
    CXXDASP_FFT_BACKEND_LIBS_arm64-v8a              += kiss_fft_static
    CXXDASP_FFT_BACKEND_LIBS_x86                    += kiss_fft_static
    CXXDASP_FFT_BACKEND_LIBS_x86_64                 += kiss_fft_static
    CXXDASP_FFT_BACKEND_LIBS_mips                   += kiss_fft_static
    CXXDASP_FFT_BACKEND_LIBS_mips64                 += kiss_fft_static
endif

ifeq ($(CXXDASP_USE_FFT_BACKEND_NE10), 1)
    # FFTS is only available on armeabi-v7a
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a            += ne10_static
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a-no-neon    += ne10_static
    CXXDASP_FFT_BACKEND_LIBS_arm64-v8a              += ne10_static
endif

ifeq ($(CXXDASP_USE_FFT_BACKEND_CKFFT), 1)
    # Cricket FFT is available on all platforms
    CXXDASP_FFT_BACKEND_LIBS_armeabi                += ckfft-no-simd_static
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a            += ckfft_static
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a-no-neon    += ckfft-no-simd_static # NOTE: requires -no-simd suffixed version
    CXXDASP_FFT_BACKEND_LIBS_arm64-v8a              += ckfft_static
    CXXDASP_FFT_BACKEND_LIBS_x86                    += ckfft-no-simd_static
    CXXDASP_FFT_BACKEND_LIBS_x86_64                 += ckfft-no-simd_static
    CXXDASP_FFT_BACKEND_LIBS_mips                   += ckfft-no-simd_static
    CXXDASP_FFT_BACKEND_LIBS_mips64                 += ckfft-no-simd_static
endif

ifeq ($(CXXDASP_USE_FFT_BACKEND_MUFFT), 1)
    # muFFT is available on all platforms
    CXXDASP_FFT_BACKEND_LIBS_armeabi                += mufft_static
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a            += mufft_static
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a-no-neon    += mufft_static
    CXXDASP_FFT_BACKEND_LIBS_arm64-v8a              += mufft_static
    CXXDASP_FFT_BACKEND_LIBS_x86                    += mufft_static
    CXXDASP_FFT_BACKEND_LIBS_x86_64                 += mufft_static
    CXXDASP_FFT_BACKEND_LIBS_mips                   += mufft_static
    CXXDASP_FFT_BACKEND_LIBS_mips64                 += mufft_static
endif

ifeq ($(CXXDASP_USE_FFT_BACKEND_GP_FFT), 1)
    # General purpose FFT package is available on all platforms
    CXXDASP_FFT_BACKEND_LIBS_armeabi                += gp_fft_static
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a            += gp_fft_static
    CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a-no-neon    += gp_fft_static
    CXXDASP_FFT_BACKEND_LIBS_arm64-v8a              += gp_fft_static
    CXXDASP_FFT_BACKEND_LIBS_x86                    += gp_fft_static
    CXXDASP_FFT_BACKEND_LIBS_x86_64                 += gp_fft_static
    CXXDASP_FFT_BACKEND_LIBS_mips                   += gp_fft_static
    CXXDASP_FFT_BACKEND_LIBS_mips64                 += gp_fft_static
endif

# copy armeabi-v7a settings to armeabi-v7a-hard
CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a-hard := $(CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a)
CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a-hard-no-neon := $(CXXDASP_FFT_BACKEND_LIBS_armeabi-v7a-no-neon)


#
# target specific CFLAGS for cxxporthelper library
#
CXXPH_CFLAGS_armeabi                := -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_ARM_NEON=0
CXXPH_CFLAGS_armeabi-v7a            := -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_ARM_NEON=0 -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_ARM_V7=0
CXXPH_CFLAGS_armeabi-v7a-no-neon    := -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_ARM_NEON=0 -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_ARM_V7=0
CXXPH_CFLAGS_armeabi-v7a-hard       := $(CXXPH_CFLAGS_armeabi-v7a)
CXXPH_CFLAGS_armeabi-v7a-hard-no-neon := $(CXXPH_CFLAGS_armeabi-v7a-no-neon)
CXXPH_CFLAGS_arm64-v8a              := -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_ARM_NEON=0
# mmx, sse, sse2 and sse3 instructions are always available on Intel Atom processor
CXXPH_CFLAGS_x86                    := \
    -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_X86_MMX=0 \
    -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_X86_SSE=0 \
    -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_X86_SSE2=0 \
    -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_X86_SSE3=0
CXXPH_CFLAGS_x86_64                 := \
    -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_X86_MMX=0 \
    -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_X86_SSE=0 \
    -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_X86_SSE2=0 \
    -DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_X86_SSE3=0
CXXPH_CFLAGS_mips                   := 
CXXPH_CFLAGS_mips64                 := 
