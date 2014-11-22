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

TEST_APP_BASENAME := test_filter_cascaded_biquad_df1
TEST_TOP_DIR := $(CXXDASP_TOP_DIR)/test
TEST_SRC_FILES := \
    filter_cascaded_biquad_df1/cascaded_biquad_filter_df1_test.cpp

#
# test app
#
LOCAL_PATH := $(TEST_TOP_DIR)

include $(CLEAR_VARS)

LOCAL_MODULE := $(TEST_APP_BASENAME)
LOCAL_SRC_FILES := $(TEST_SRC_FILES)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_STATIC_LIBRARIES := \
    cxxdasp_static cxxdasp_$(TARGET_ARCH_ABI)_static cxxporthelper_static $(CXXDASP_FFT_BACKEND_LIBS_$(TARGET_ARCH_ABI)) cpufeatures \
    gmock-main_static gmock_static gtest_static

# if $(TARGET_ARCH_ABI) == {armeabi-v7a | armeabi-v7a-hard}
ifneq (, $(filter armeabi-v7a armeabi-v7a-hard, $(TARGET_ARCH_ABI)))
    LOCAL_ARM_NEON  := true
endif

include $(BUILD_EXECUTABLE)


#
# test app (-no-neon)
#
# if $(TARGET_ARCH_ABI) == {armeabi-v7a | armeabi-v7a-hard}
ifneq (, $(filter armeabi-v7a armeabi-v7a-hard, $(TARGET_ARCH_ABI)))

LOCAL_PATH := $(TEST_TOP_DIR)
include $(CLEAR_VARS)

LOCAL_MODULE := $(TEST_APP_BASENAME)-no-neon
LOCAL_SRC_FILES := $(TEST_SRC_FILES)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_STATIC_LIBRARIES := \
    cxxdasp_static cxxdasp_$(TARGET_ARCH_ABI)-no-neon_static $(CXXDASP_FFT_BACKEND_LIBS_$(TARGET_ARCH_ABI)-no-neon) cxxporthelper_static cpufeatures \
    gmock-main_static gmock_static gtest_static

include $(BUILD_EXECUTABLE)

else
# dummy entry
LOCAL_PATH := $(TEST_TOP_DIR)
include $(CLEAR_VARS)
LOCAL_MODULE := $(TEST_APP_BASENAME)-no-neon
LOCAL_MODULE_FILENAME := $(TEST_APP_BASENAME)-no-neon-dummy
include $(BUILD_STATIC_LIBRARY)
endif
