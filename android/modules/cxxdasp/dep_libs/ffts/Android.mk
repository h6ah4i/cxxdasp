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
# FFTS (libffts)
#

# NOTEs:
# - only supports armeabi-v7a platform
# - have to be built with android/dep_libs/ffts/built_android.sh

MY_DIR := $(call my-dir)
CXXDASP_TOP_DIR := $(call cxxdasp-normalize-path, $(MY_DIR)/../../../../..)

LOCAL_PATH := $(CXXDASP_TOP_DIR)/android/dep_libs/ffts

include $(CLEAR_VARS)

# if $(TARGET_ARCH_ABI) == {armeabi-v7a | armeabi-v7a-hard}
ifneq (, $(filter armeabi-v7a armeabi-v7a-hard, $(TARGET_ARCH_ABI)))

# refer pre-built library
LOCAL_MODULE := ffts_static
LOCAL_MODULE_FILENAME := ffts
LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/src/codegen.c \
	$(LOCAL_PATH)/src/ffts.c \
	$(LOCAL_PATH)/src/ffts_nd.c \
	$(LOCAL_PATH)/src/ffts_real.c \
	$(LOCAL_PATH)/src/ffts_real_nd.c \
	$(LOCAL_PATH)/src/ffts_static.c \
	$(LOCAL_PATH)/src/ffts_transpose.c \
	$(LOCAL_PATH)/src/ffts_trig.c \
	$(LOCAL_PATH)/src/neon.s
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_CFLAGS := -DHAVE_MALLOC_H -DHAVE_STDINT_H -DHAVE_STDLIB_H -DHAVE_STRING_H -DHAVE_SYS_MMAN_H -DHAVE_UNISTD_H -DHAVE_NEON
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_CFLAGS := -DCXXDASP_USE_FFT_BACKEND_FFTS=1
LOCAL_ARM_NEON  := true

include $(BUILD_STATIC_LIBRARY)

else

# dummy library
LOCAL_MODULE := ffts_static
LOCAL_MODULE_FILENAME := ffts_dummy

include $(BUILD_STATIC_LIBRARY)

endif
