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

MY_DIR := $(call my-dir)
CXXDASP_TOP_DIR := $(MY_DIR)/../../../..

include $(CXXDASP_TOP_DIR)/android/build-utils/cxxdasp-build-setup.mk

CXXDASP_TOP_DIR := $(call cxxdasp-normalize-path, $(CXXDASP_TOP_DIR))

#
# library (libcxxdasp)
#
LOCAL_PATH := $(CXXDASP_TOP_DIR)

include $(CLEAR_VARS)
LOCAL_MODULE    := cxxdasp_static
LOCAL_MODULE_FILENAME := cxxdasp

LOCAL_SRC_FILES := \
    source/cxxdasp.cpp \
    source/resampler/polyphase/polyphase_resampler_utils.cpp \
    source/resampler/smart/smart_resampler_params_factory.cpp \
    source/utils/utils.cpp \
    source/filter/biquad/biquad_filter_coeffs.cpp \
    source/filter/tsvf/tsvf_coeffs.cpp \
    source/window/window_functions.cpp

SHARED_C_INCLUDES := $(CXXDASP_TOP_DIR)/include

SHARED_CFLAGS := \
    -DCXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_LOW_QUALITY=$(CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_LOW_QUALITY) \
    -DCXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_HIGH_QUALITY=$(CXXDASP_ENABLE_POLYPHASE_RESAMPLER_FACTORY_HIGH_QUALITY) \
    $(CXXPH_CFLAGS_$(TARGET_ARCH_ABI))

LOCAL_C_INCLUDES := \
    $(SHARED_C_INCLUDES) \
    $(CXXDASP_TOP_DIR)/generated_source

LOCAL_CFLAGS := \
    $(SHARED_CFLAGS)

LOCAL_STATIC_LIBRARIES := cxxporthelper_static

# export
LOCAL_EXPORT_C_INCLUDES := $(SHARED_C_INCLUDES)
LOCAL_EXPORT_CFLAGS := $(SHARED_CFLAGS)

# clear local variables
SHARED_C_INCLUDES :=
SHARED_CFLAGS := 

include $(BUILD_STATIC_LIBRARY)


#
# library (libcxxdasp_$(TARGET_ARCH_ABI))
# (target specific optimized module)
#

LOCAL_PATH := $(CXXDASP_TOP_DIR)
include $(CLEAR_VARS)

LOCAL_MODULE    := cxxdasp_$(TARGET_ARCH_ABI)_static
LOCAL_MODULE_FILENAME := cxxdasp_$(TARGET_ARCH_ABI)

HAVE_SPECIALIZED_LIBCXXDASP_ARCH_ABI := 0
SHARED_C_INCLUDES :=
LOCAL_STATIC_LIBRARIES := cxxdasp_static

### for armeabi-v7a, armeabi-v7a-hard and arm64-v8a
ifeq ($(HAVE_SPECIALIZED_LIBCXXDASP_ARCH_ABI), 0)
ifneq (, $(filter armeabi-v7a | armeabi-v7a-hard | arm64-v8a, $(TARGET_ARCH_ABI)))

HAVE_SPECIALIZED_LIBCXXDASP_ARCH_ABI := 1

LOCAL_SRC_FILES := \
    source/utils/utils_impl_neon_optimized.cpp \
    source/filter/biquad/f32_mono_neon_biquad_df1_core_operator.cpp \
    source/filter/biquad/f32_stereo_neon_biquad_df1_core_operator.cpp \
    source/filter/biquad/f32_mono_neon_biquad_tdf2_core_operator.cpp \
    source/filter/biquad/f32_stereo_neon_biquad_tdf2_core_operator.cpp \
    source/filter/cascaded_biquad/f32_stereo_neon_cascaded_2_biquad_tdf2_core_operator.cpp \
    source/filter/tsvf/f32_mono_neon_tsvf_core_operator.cpp \
    source/filter/tsvf/f32_stereo_neon_tsvf_core_operator.cpp \
    source/converter/s16_to_f32_mono_neon_sample_format_converter_core_operator.cpp \
    source/converter/s16_to_f32_stereo_neon_sample_format_converter_core_operator.cpp \
    source/converter/s16_mono_to_f32_stereo_neon_sample_format_converter_core_operator.cpp \
    source/converter/f32_to_s16_mono_neon_sample_format_converter_core_operator.cpp \
    source/converter/f32_to_s16_stereo_neon_sample_format_converter_core_operator.cpp \
    source/converter/s16_to_f32_mono_neon_fast_sample_format_converter_core_operator.cpp \
    source/converter/s16_to_f32_stereo_neon_fast_sample_format_converter_core_operator.cpp \
    source/converter/s16_mono_to_f32_stereo_neon_fast_sample_format_converter_core_operator.cpp \
    source/converter/f32_to_s16_mono_neon_fast_sample_format_converter_core_operator.cpp \
    source/converter/f32_to_s16_stereo_neon_fast_sample_format_converter_core_operator.cpp \
    source/mixer/f32_mono_neon_mixer_core_operator.cpp \
    source/mixer/f32_stereo_neon_mixer_core_operator.cpp

SHARED_C_INCLUDES := $(CXXDASP_TOP_DIR)/include

# neon instructions are available
ifneq (, $(filter armeabi-v7a | armeabi-v7a-hard, $(TARGET_ARCH_ABI)))
    LOCAL_ARM_NEON := true
endif

# workaround for clang
ifneq (, $(filter arm64-v8a, $(TARGET_ARCH_ABI)))
ifneq (, $(filter clang%, $(NDK_TOOLCHAIN_VERSION)))
LOCAL_CFLAGS += -fno-integrated-as
endif
endif

endif
endif  # HAVE_SPECIALIZED_LIBCXXDASP_ARCH_ABI


### for x86 and x86_64

ifeq ($(HAVE_SPECIALIZED_LIBCXXDASP_ARCH_ABI), 0)
ifneq (, $(filter x86 | x86_64, $(TARGET_ARCH_ABI)))

HAVE_SPECIALIZED_LIBCXXDASP_ARCH_ABI := 1

LOCAL_SRC_FILES := \
    source/utils/utils_impl_sse_optimized.cpp \
    source/converter/s16_to_f32_mono_sse_sample_format_converter_core_operator.cpp \
    source/converter/s16_to_f32_stereo_sse_sample_format_converter_core_operator.cpp \
    source/converter/s16_mono_to_f32_stereo_sse_sample_format_converter_core_operator.cpp \
    source/converter/f32_to_s16_mono_sse_sample_format_converter_core_operator.cpp \
    source/converter/f32_to_s16_stereo_sse_sample_format_converter_core_operator.cpp \
    source/mixer/f32_mono_sse_mixer_core_operator.cpp \
    source/mixer/f32_stereo_sse_mixer_core_operator.cpp

SHARED_C_INCLUDES := $(CXXDASP_TOP_DIR)/include

endif
endif  # HAVE_SPECIALIZED_LIBCXXDASP_ARCH_ABI


### for other platforms

ifeq ($(HAVE_SPECIALIZED_LIBCXXDASP_ARCH_ABI), 0)
LOCAL_SRC_FILES :=
endif

# set local variables
LOCAL_C_INCLUDES += \
    $(SHARED_C_INCLUDES) \
    $(CXXPORTHELPER_INC_DIR)

LOCAL_CFLAGS += \
    $(SHARED_CFLAGS)

LOCAL_STATIC_LIBRARIES := cxxdasp_static cxxporthelper_static

# set export variables
LOCAL_EXPORT_C_INCLUDES := $(SHARED_C_INCLUDES)
LOCAL_EXPORT_CFLAGS := $(SHARED_CFLAGS)

# clear local variables
HAVE_SPECIALIZED_LIBCXXDASP_ARCH_ABI :=
SHARED_C_INCLUDES :=
SHARED_CFLAGS := 

# build
include $(BUILD_STATIC_LIBRARY)


#
# library (libcxxdasp_$(TARGET_ARCH_ABI)-no-neon)
#

# if $(TARGET_ARCH_ABI) == {armeabi-v7a | armeabi-v7a-hard}
ifneq (, $(filter armeabi-v7a armeabi-v7a-hard, $(TARGET_ARCH_ABI)))

include $(CLEAR_VARS)

SHARED_CFLAGS := $(CXXPH_CFLAGS_$(TARGET_ARCH_ABI)-no-neon)

LOCAL_MODULE    := cxxdasp_$(TARGET_ARCH_ABI)-no-neon_static
LOCAL_MODULE_FILENAME := cxxdasp_$(TARGET_ARCH_ABI)-no-neon

LOCAL_CFLAGS += \
    $(SHARED_CFLAGS)

LOCAL_EXPORT_CFLAGS := $(SHARED_CFLAGS)

# clear local variables
SHARED_C_INCLUDES :=
SHARED_CFLAGS := 

# build
include $(BUILD_STATIC_LIBRARY)

endif
