#
#    Copyright (C) 2016 Haruki Hasegawa
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
# Cricket FFT (libckfft)
#
MY_DIR := $(call my-dir)
CXXDASP_TOP_DIR := $(call cxxdasp-normalize-path, $(MY_DIR)/../../../../..)

LOCAL_PATH := $(CXXDASP_TOP_DIR)/android/dep_libs/ckfft

include $(CLEAR_VARS)

LOCAL_MODULE := ckfft_static
LOCAL_MODULE_FILENAME := ckfft
LOCAL_SRC_FILES := \
    $(LOCAL_PATH)/src/ckfft/ckfft.cpp \
    $(LOCAL_PATH)/src/ckfft/context.cpp \
    $(LOCAL_PATH)/src/ckfft/debug.cpp \
    $(LOCAL_PATH)/src/ckfft/fft.cpp \
    $(LOCAL_PATH)/src/ckfft/fft_default.cpp \
    $(LOCAL_PATH)/src/ckfft/fft_real.cpp \
    $(LOCAL_PATH)/src/ckfft/fft_real_default.cpp \
    $(LOCAL_PATH)/src/ckfft/fft_neon.cpp \
    $(LOCAL_PATH)/src/ckfft/fft_real_neon.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc $(LOCAL_PATH)/src
LOCAL_STATIC_LIBRARIES := cpufeatures
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_EXPORT_CFLAGS := -DCXXDASP_USE_FFT_BACKEND_CKFFT=1

# if $(TARGET_ARCH_ABI) == {armeabi-v7a | armeabi-v7a-hard | arm64-v8a}
ifneq (, $(filter armeabi-v7a armeabi-v7a-hard arm64-v8a, $(TARGET_ARCH_ABI)))
	LOCAL_ARM_NEON := true
endif

ifneq (, $(filter arm64-v8a, $(TARGET_ARCH_ABI)))
	LOCAL_CFLAGS += -D__arm__=1 -D__ARM_NEON__=1
endif

include $(BUILD_STATIC_LIBRARY)

#
# Cricket FFT (libckfft-no-simd)
#
LOCAL_PATH := $(CXXDASP_TOP_DIR)/android/dep_libs/ckfft

include $(CLEAR_VARS)

LOCAL_MODULE := ckfft-no-simd_static
LOCAL_MODULE_FILENAME := ckfft-no-simd
LOCAL_SRC_FILES := \
    $(LOCAL_PATH)/src/ckfft/ckfft.cpp \
    $(LOCAL_PATH)/src/ckfft/context.cpp \
    $(LOCAL_PATH)/src/ckfft/debug.cpp \
    $(LOCAL_PATH)/src/ckfft/fft.cpp \
    $(LOCAL_PATH)/src/ckfft/fft_default.cpp \
    $(LOCAL_PATH)/src/ckfft/fft_real.cpp \
    $(LOCAL_PATH)/src/ckfft/fft_real_default.cpp \
    $(LOCAL_PATH)/src/ckfft/fft_neon.cpp \
    $(LOCAL_PATH)/src/ckfft/fft_real_neon.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc $(LOCAL_PATH)/src
LOCAL_STATIC_LIBRARIES := cpufeatures
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_EXPORT_CFLAGS := -DCXXDASP_USE_FFT_BACKEND_CKFFT=1

include $(BUILD_STATIC_LIBRARY)
