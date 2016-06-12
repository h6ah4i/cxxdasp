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

cmake_minimum_required(VERSION 2.8)

project(AndroidDepLibsDownload NONE)
set(CMAKE_SYSTEM_NAME Generic)

#
# Declare variables (reffered by subdirectory modules)
#

set(DEP_LIBS_PLACEMENT_DIR ${PROJECT_SOURCE_DIR}/..)

#
# Sub directories
#
message("")

if (NOT EXISTS ${DEP_LIBS_PLACEMENT_DIR}/cxxporthelper/)
    add_subdirectory(cxxporthelper)
else()
    message("INFO: ''cxxporthelper'' directory is already exists.")
endif()

if (NOT EXISTS ${DEP_LIBS_PLACEMENT_DIR}/pffft/)
    add_subdirectory(pffft)
else()
    message("INFO: ''pffft'' directory is already exists.")
endif()

if (NOT EXISTS ${DEP_LIBS_PLACEMENT_DIR}/ffts/)
    add_subdirectory(ffts)
else()
    message("INFO: ''ffts'' directory is already exists.")
endif()

if (NOT EXISTS ${DEP_LIBS_PLACEMENT_DIR}/kiss_fft/)
    add_subdirectory(kiss_fft)
else()
    message("INFO: ''kiss_fft'' directory is already exists.")
endif()

if (NOT EXISTS ${DEP_LIBS_PLACEMENT_DIR}/ne10/)
    add_subdirectory(ne10)
else()
    message("INFO: ''ne10'' directory is already exists.")
endif()

if (NOT EXISTS ${DEP_LIBS_PLACEMENT_DIR}/ckfft/)
    add_subdirectory(ckfft)
else()
    message("INFO: ''ckfft'' directory is already exists.")
endif()

if (NOT EXISTS ${DEP_LIBS_PLACEMENT_DIR}/mufft/)
    add_subdirectory(mufft)
else()
    message("INFO: ''mufft'' directory is already exists.")
endif()

if (NOT EXISTS ${DEP_LIBS_PLACEMENT_DIR}/gp_fft/)
    add_subdirectory(gp_fft)
else()
    message("INFO: ''gp_fft'' directory is already exists.")
endif()

if (NOT EXISTS ${DEP_LIBS_PLACEMENT_DIR}/android-ndk-profiler/)
    add_subdirectory(android-ndk-profiler)
else()
    message("INFO: ''android-ndk-profiler'' directory is already exists.")
endif()

if (NOT EXISTS ${DEP_LIBS_PLACEMENT_DIR}/gmock/)
    add_subdirectory(gmock)
else()
    message("INFO: ''gmock'' directory is already exists.")
endif()

message("")
