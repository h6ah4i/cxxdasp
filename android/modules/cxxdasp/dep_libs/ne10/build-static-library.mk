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
CXXDASP_TOP_DIR := $(call cxxdasp-normalize-path, $(MY_DIR)/../../../../..)

#
# for C (*.c) code
#
LOCAL_PATH := $(CXXDASP_TOP_DIR)/android/dep_libs/ne10
include $(CLEAR_VARS)

# NOTE:
#  aarch32 -> C + NEON ASM
#  aarch64 -> C + NEON Intrinsics

LOCAL_MODULE := ne10_static
LOCAL_MODULE_FILENAME := ne10

LOCAL_STATIC_LIBRARIES := ne10_asm_static

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc

LOCAL_SRC_FILES := \
    modules/dsp/NE10_fft.c \
    modules/dsp/NE10_rfft_float32.c \
    modules/dsp/NE10_fft_float32.c

ifneq (, $(filter armeabi-v7a armeabi-v7a-hard, $(TARGET_ARCH_ABI)))
    LOCAL_ARM_NEON := true
    LOCAL_SRC_FILES += \
        modules/dsp/NE10_fft_float32.neon.c
endif

ifneq (, $(filter arm64-v8a, $(TARGET_ARCH_ABI)))
LOCAL_SRC_FILES += \
    modules/dsp/NE10_rfft_float32.neonintrinsic.c \
    modules/dsp/NE10_fft_float32.neonintrinsic.c
endif

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_EXPORT_CFLAGS := -DCXXDASP_USE_FFT_BACKEND_NE10=1

include $(BUILD_STATIC_LIBRARY)

#
# for Assembly (*.s) code
#
# NOTE: This library is required to avoid the object file name confliction between "foo.neon.c" and "foo.neon.s"
#
LOCAL_PATH := $(CXXDASP_TOP_DIR)/android/dep_libs/ne10
include $(CLEAR_VARS)

LOCAL_MODULE := ne10_asm_static
LOCAL_MODULE_FILENAME := ne10_asm

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc

LOCAL_CFLAGS := -x assembler-with-cpp

ifneq (, $(filter armeabi-v7a armeabi-v7a-hard, $(TARGET_ARCH_ABI)))
    LOCAL_ARM_NEON := true
    LOCAL_CFLAGS += -Wa,-k
    LOCAL_SRC_FILES := \
        modules/dsp/NE10_fft_float32.neon.s
endif

# Maybe, this doesn't work properly...
# ifneq (, $(filter arm64-v8a, $(TARGET_ARCH_ABI)))
#     LOCAL_SRC_FILES := \
#         modules/dsp/NE10_fft_float32.neonv8.S
# endif

include $(BUILD_STATIC_LIBRARY)
