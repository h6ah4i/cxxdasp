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
# General purpose FFT package library
#

add_library(gp_fft STATIC IMPORTED GLOBAL)

include(ExternalProject)
ExternalProject_Add(ext_gp_fft
    PREFIX latest
    URL http://www.kurims.kyoto-u.ac.jp/~ooura/fft.tgz
    URL_HASH MD5=4255dd8a74949d123216b1ab91520469
    CMAKE_ARGS -G ${CMAKE_GENERATOR} ../ext_gp_fft
    CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS}
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-gp_fft.txt <SOURCE_DIR>/CMakeLists.txt
    INSTALL_COMMAND ""
)

add_dependencies(gp_fft ext_gp_fft)

ExternalProject_Get_Property(ext_gp_fft SOURCE_DIR)
ExternalProject_Get_Property(ext_gp_fft BINARY_DIR)

if (MSVC)
    set(GP_FFT_IMPORTED_LOCATION ${BINARY_DIR}/$(ConfigurationName)/${CMAKE_STATIC_LIBRARY_PREFIX}gp_fft${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set(GP_FFT_IMPORTED_LOCATION ${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gp_fft${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set(GP_FFT_INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}>)

set_target_properties(gp_fft PROPERTIES 
    IMPORTED_LOCATION ${GP_FFT_IMPORTED_LOCATION}
    INTERFACE_INCLUDE_DIRECTORIES ${GP_FFT_INTERFACE_INCLUDE_DIRECTORIES}
)
