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
# PFFFT (libpffft)
#
MY_DIR := $(call my-dir)
CXXDASP_TOP_DIR := $(call cxxdasp-normalize-path, $(MY_DIR)/../../../../..)

LOCAL_PATH := $(CXXDASP_TOP_DIR)/android/dep_libs/pffft

include $(CLEAR_VARS)

LOCAL_MODULE := pffft_static
LOCAL_MODULE_FILENAME := pffft
LOCAL_SRC_FILES := pffft.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_EXPORT_CFLAGS := -DCXXDASP_USE_FFT_BACKEND_PFFFT=1

# if $(TARGET_ARCH_ABI) == {armeabi-v7a | armeabi-v7a-hard}
ifneq (, $(filter armeabi-v7a armeabi-v7a-hard arm64-v8a, $(TARGET_ARCH_ABI)))
    LOCAL_ARM_NEON  := true
endif

include $(BUILD_STATIC_LIBRARY)

#
# PFFFT (libpffft-no-simd)
#
LOCAL_PATH := $(CXXDASP_TOP_DIR)/android/dep_libs/pffft

include $(CLEAR_VARS)

LOCAL_MODULE := pffft-no-simd_static
LOCAL_MODULE_FILENAME := pffft-no-simd
LOCAL_SRC_FILES := pffft.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS := -DPFFFT_SIMD_DISABLE
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_EXPORT_CFLAGS := -DCXXDASP_USE_FFT_BACKEND_PFFFT=1

include $(BUILD_STATIC_LIBRARY)
