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
# muFFT library
#
include(ExternalProject)

ExternalProject_Add(ext_mufft
    PREFIX latest
    URL https://github.com/Themaister/muFFT/archive/6d716abf9d813627338b9580589fc442975559d1.tar.gz
    URL_HASH MD5=ea8363a7b6c0ca4c7177439e2d035411
    # GIT_REPOSITORY https://github.com/Themaister/muFFT.git
    # GIT_TAG 6d716abf9d813627338b9580589fc442975559d1
    CMAKE_ARGS -G ${CMAKE_GENERATOR} ../ext_mufft
    CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS}
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-mufft.txt <SOURCE_DIR>/CMakeLists.txt
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ../ext_mufft/fft.h <SOURCE_DIR>/inc/mufft/fft.h
)

add_library(mufft STATIC IMPORTED GLOBAL)
add_dependencies(mufft ext_mufft)

ExternalProject_Get_Property(ext_mufft SOURCE_DIR)
ExternalProject_Get_Property(ext_mufft BINARY_DIR)

if (MSVC)
    set(MUFFT_IMPORTED_LOCATION ${BINARY_DIR}/$(ConfigurationName)/${CMAKE_STATIC_LIBRARY_PREFIX}mufft${CMAKE_STATIC_LIBRARY_SUFFIX})
else()  
    set(MUFFT_IMPORTED_LOCATION ${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}mufft${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

# set(MUFFT_INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}/inc>)

set_target_properties(mufft PROPERTIES 
    IMPORTED_LOCATION ${MUFFT_IMPORTED_LOCATION}
    # INTERFACE_INCLUDE_DIRECTORIES ${MUFFT_INTERFACE_INCLUDE_DIRECTORIES}
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET mufft APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/inc)
