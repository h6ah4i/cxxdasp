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
# General settings 
#

# NOTE: 
# c++_static or gnustl_static is required to build test codes
# (cxxdasp is compatible with stlport, but gmock/gtest are not compatible.)

#
# Detect NDK version
#
#CXXDASP_NDK_RELEASE_VERSION_FULL := $(firstword $(strip $(shell cat $(NDK_ROOT)/RELEASE.TXT)))
#CXXDASP_NDK_RELEASE_VERSION_NUMBER := $(shell echo $(CXXDASP_NDK_RELEASE_VERSION_FULL) | $(HOST_SED) 's/^r\([0-9]\+\).*$$/\1/g')
CXXDASP_NDK_RELEASE_VERSION_NUMBER := $(firstword $(strip $(shell awk 'BEGIN{ file="'$(NDK_ROOT)/RELEASE.TXT'";while ((getline<file) > 0){ if (match($$0,/[0-9]+/)) { print substr($$0, RSTART, RLENGTH); } } }')))
#$(info CXXDASP_NDK_RELEASE_VERSION_NUMBER = $(CXXDASP_NDK_RELEASE_VERSION_NUMBER))


ifeq ($(CXXDASP_NDK_RELEASE_VERSION_NUMBER), 9)
# NDK r9
NDK_TOOLCHAIN_VERSION := 4.8
# NDK_TOOLCHAIN_VERSION := clang3.4
APP_STL := c++_static
# APP_STL := gnustl_static
else ifeq ($(CXXDASP_NDK_RELEASE_VERSION_NUMBER), 10)
# NDK r10
NDK_TOOLCHAIN_VERSION := 4.9
# NDK_TOOLCHAIN_VERSION := clang3.5
APP_STL := c++_static
# APP_STL := gnustl_static
else
# use default compiler
APP_STL := c++_static
endif

#
# APP_OPTIM
#
APP_OPTIM := release

#
# APP_CPPFLAGS
#
APP_CPPFLAGS := -fexceptions -std=c++11

#
# Example apps
#
APP_MODULES += simple-resampler
APP_MODULES += x2-resampler
APP_MODULES += biquad-filter
APP_MODULES += cascaded-biquad-filter
APP_MODULES += trapezoidal-state-variable-filter
APP_MODULES += cascaded-trapezoidal-state-variable-filter
APP_MODULES += sample-format-converter
APP_MODULES += window-function

### without NEON instruction version (for armeabi-v7a only)
APP_MODULES += simple-resampler-no-neon
APP_MODULES += x2-resampler-no-neon
APP_MODULES += biquad-filter-no-neon
APP_MODULES += cascaded-biquad-filter-no-neon
APP_MODULES += trapezoidal-state-variable-filter-no-neon
APP_MODULES += cascaded-trapezoidal-state-variable-filter-no-neon
APP_MODULES += sample-format-converter-no-neon
APP_MODULES += window-function-no-neon

#
# Test apps
#
APP_MODULES += test_utils_utils
APP_MODULES += test_fft
APP_MODULES += test_resampler_polyphase
APP_MODULES += test_filter_biquad_df1
APP_MODULES += test_filter_biquad_tdf2
APP_MODULES += test_filter_cascaded_biquad_df1
APP_MODULES += test_filter_cascaded_biquad_tdf2
APP_MODULES += test_filter_tsvf
APP_MODULES += test_filter_cascaded_tsvf
APP_MODULES += test_sample_format_converter

### without NEON instruction version (for armeabi-v7a only)
APP_MODULES += test_utils_utils-no-neon
APP_MODULES += test_fft-no-neon
APP_MODULES += test_resampler_polyphase-no-neon
APP_MODULES += test_filter_biquad_df1-no-neon
APP_MODULES += test_filter_biquad_tdf2-no-neon
APP_MODULES += test_filter_cascaded_biquad_df1-no-neon
APP_MODULES += test_filter_cascaded_biquad_tdf2-no-neon
APP_MODULES += test_filter_tsvf-no-neon
APP_MODULES += test_filter_cascaded_tsvf-no-neon
APP_MODULES += test_sample_format_converter-no-neon

#
# Options
#

### enable -O3 optimization
ifeq ($(APP_OPTIM), release)
APP_CFLAGS += -O3 -DNDEBUG
endif

### enable ASM output
APP_CFLAGS += -save-temps=obj -fverbose-asm

### enable android-ndk-profiler
### NOTE: requres GCC 4.8 (4.9 and clang won't work)
USE_ANDROID_NDK_PROFILER := 0

ifeq ($(USE_ANDROID_NDK_PROFILER), 1)
    APP_CFLAGS += -pg
    APP_LDFLAGS += -pg
endif
