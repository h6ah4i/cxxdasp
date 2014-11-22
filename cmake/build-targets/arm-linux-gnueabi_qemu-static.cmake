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

set(CMAKE_SYSTEM_NAME Linux)
set(HAVE_WORDS_BIGENDIAN_EXITCODE NO)

set(CXXDASP_TOOLSET_PREFIX arm-linux-gnueabi)

find_program(CMAKE_C_COMPILER ${CXXDASP_TOOLSET_PREFIX}-gcc)
find_program(CMAKE_CXX_COMPILER ${CXXDASP_TOOLSET_PREFIX}-g++)

set(CMAKE_FIND_ROOT_PATH /usr/${CXXDASP_TOOLSET_PREFIX}/)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# NOTE these settings are required to run on QEMU
add_definitions(-DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_ARM_NEON=0)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=softfp" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=softfp" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=softfp" CACHE STRING "" FORCE)

# for dependent libraries
set(CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS} -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_CURRENT_LIST_FILE})
