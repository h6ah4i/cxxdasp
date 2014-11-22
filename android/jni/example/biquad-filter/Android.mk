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
# example (biquad-filter)
#
LOCAL_PATH := $(CXXDASP_TOP_DIR)/example

include $(CLEAR_VARS)

LOCAL_MODULE := biquad-filter
LOCAL_SRC_FILES := biquad-filter/biquad-filter.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_STATIC_LIBRARIES := cxxdasp_static cxxdasp_$(TARGET_ARCH_ABI)_static cxxporthelper_static $(CXXDASP_FFT_BACKEND_LIBS_$(TARGET_ARCH_ABI)) cpufeatures

# if $(TARGET_ARCH_ABI) == {armeabi-v7a | armeabi-v7a-hard}
ifneq (, $(filter armeabi-v7a armeabi-v7a-hard, $(TARGET_ARCH_ABI)))
    LOCAL_ARM_NEON  := true
endif

# compile with profiling
ifeq ($(USE_ANDROID_NDK_PROFILER), 1)
    LOCAL_CFLAGS += -pg -DUSE_ANDROID_NDK_PROFILER
    LOCAL_STATIC_LIBRARIES += android-ndk-profiler
endif

include $(BUILD_EXECUTABLE)


#
# example (biquad-filter-no-neon)
#
# if $(TARGET_ARCH_ABI) == {armeabi-v7a | armeabi-v7a-hard}
ifneq (, $(filter armeabi-v7a armeabi-v7a-hard, $(TARGET_ARCH_ABI)))

LOCAL_PATH := $(CXXDASP_TOP_DIR)/example
include $(CLEAR_VARS)

LOCAL_MODULE := biquad-filter-no-neon
LOCAL_SRC_FILES := biquad-filter/biquad-filter.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_STATIC_LIBRARIES := cxxdasp_static cxxdasp_$(TARGET_ARCH_ABI)-no-neon_static cxxporthelper_static $(CXXDASP_FFT_BACKEND_LIBS_$(TARGET_ARCH_ABI)-no-neon) cpufeatures

# compile with profiling
ifeq ($(USE_ANDROID_NDK_PROFILER), 1)
    LOCAL_CFLAGS += -DUSE_ANDROID_NDK_PROFILER
    LOCAL_STATIC_LIBRARIES += android-ndk-profiler
endif

include $(BUILD_EXECUTABLE)

else
# dummy entry
LOCAL_PATH := $(CXXDASP_TOP_DIR)/example
include $(CLEAR_VARS)
LOCAL_MODULE := biquad-filter-no-neon
LOCAL_MODULE_FILENAME := biquad-filter-no-neon-dummy
include $(BUILD_STATIC_LIBRARY)
endif
