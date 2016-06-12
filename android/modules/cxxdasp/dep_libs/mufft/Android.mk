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
# muFFT (libmufft)
#
MY_DIR := $(call my-dir)
CXXDASP_TOP_DIR := $(call cxxdasp-normalize-path, $(MY_DIR)/../../../../..)

LOCAL_PATH := $(CXXDASP_TOP_DIR)/android/dep_libs/mufft

include $(CLEAR_VARS)

LOCAL_MODULE := mufft_static
LOCAL_MODULE_FILENAME := mufft
LOCAL_SRC_FILES := \
    $(LOCAL_PATH)/fft.c \
    $(LOCAL_PATH)/kernel.c \
    $(LOCAL_PATH)/cpu.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS += -std=c99
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_EXPORT_CFLAGS := -DCXXDASP_USE_FFT_BACKEND_MUFFT=1

# if $(TARGET_ARCH_ABI) == {x86 | x86_64}
ifneq (, $(filter x86 x86_64, $(TARGET_ARCH_ABI)))
	LOCAL_CFLAGS += -DMUFFT_HAVE_X86 -DMUFFT_HAVE_SSE3
    LOCAL_SRC_FILES += \
    	$(LOCAL_PATH)/x86/kernel.c
endif

include $(BUILD_STATIC_LIBRARY)
