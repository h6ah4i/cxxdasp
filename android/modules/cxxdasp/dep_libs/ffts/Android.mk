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

# check prebuilt library file is available
ifneq ("$(wildcard $(LOCAL_PATH)/java/android/bin/lib/libffts.a)","")
    # found
    LOCAL_SRC_FILES := java/android/bin/lib/libffts.a
    LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
    LOCAL_EXPORT_CFLAGS := -DCXXDASP_USE_FFT_BACKEND_FFTS=1
    LOCAL_EXPORT_LDLIBS := -llog
    
    include $(PREBUILT_STATIC_LIBRARY)
else
    # not found... create dummy library 
    $(info libffts.a is not found)
    include $(BUILD_STATIC_LIBRARY)
endif


else

# dummy library
LOCAL_MODULE := ffts_static
LOCAL_MODULE_FILENAME := ffts_dummy

include $(BUILD_STATIC_LIBRARY)

endif
