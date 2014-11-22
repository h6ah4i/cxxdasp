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

#
# library (libgmock)
#
LOCAL_PATH := $(CXXDASP_TOP_DIR)/android/dep_libs/gmock

include $(CLEAR_VARS)
LOCAL_MODULE    := gmock_static
LOCAL_MODULE_FILENAME := gmock

LOCAL_SRC_FILES := src/gmock-all.cc 

LOCAL_C_FLAGS := -DGTEST_HAS_PTHREAD=0
LOCAL_EXPORT_CFLAGS := -DGTEST_HAS_PTHREAD=0
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/gtest/include
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/gtest/include

include $(BUILD_STATIC_LIBRARY)

#
# library (libgmock-main)
#
LOCAL_PATH := $(CXXDASP_TOP_DIR)/android/dep_libs/gmock

include $(CLEAR_VARS)
LOCAL_MODULE    := gmock-main_static
LOCAL_MODULE_FILENAME := gmock-main

LOCAL_SRC_FILES := src/gmock_main.cc 

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/gtest/include

include $(BUILD_STATIC_LIBRARY)

#
# library (libgtest)
#
LOCAL_PATH := $(CXXDASP_TOP_DIR)/android/dep_libs/gmock/gtest

include $(CLEAR_VARS)
LOCAL_MODULE    := gtest_static
LOCAL_MODULE_FILENAME := gtest

LOCAL_SRC_FILES := src/gtest-all.cc

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

include $(BUILD_STATIC_LIBRARY)

