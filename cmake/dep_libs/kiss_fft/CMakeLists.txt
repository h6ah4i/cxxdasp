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
# Kiss FFT library
#
include(ExternalProject)

ExternalProject_Add(ext_kiss_fft
    PREFIX v1_3_0
    URL http://sourceforge.net/projects/kissfft/files/kissfft/v1_3_0/kiss_fft130.tar.gz/download
    URL_HASH MD5=8497ac4c6eb423ed2f984516b0906d24
    CMAKE_ARGS -G ${CMAKE_GENERATOR} ../kiss_fft
    CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS}
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-kiss_fft.txt <SOURCE_DIR>/CMakeLists.txt
    INSTALL_COMMAND ""
)


add_library(kiss_fft STATIC IMPORTED GLOBAL)
add_dependencies(kiss_fft ext_kiss_fft)

ExternalProject_Get_Property(ext_kiss_fft SOURCE_DIR)
ExternalProject_Get_Property(ext_kiss_fft BINARY_DIR)

if (MSVC)
    set(KISS_FFT_IMPORTED_LOCATION ${BINARY_DIR}/$(ConfigurationName)/${CMAKE_STATIC_LIBRARY_PREFIX}kiss_fft${CMAKE_STATIC_LIBRARY_SUFFIX})
else()  
    set(KISS_FFT_IMPORTED_LOCATION ${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}kiss_fft${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set(KISS_FFT_INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}>)

set_target_properties(kiss_fft PROPERTIES 
    IMPORTED_LOCATION ${KISS_FFT_IMPORTED_LOCATION}
    INTERFACE_INCLUDE_DIRECTORIES ${KISS_FFT_INTERFACE_INCLUDE_DIRECTORIES}
)
