#
#    Copyright (C) 2016 Haruki Hasegawa
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
# Cricket FFT package library
#

add_library(ckfft STATIC IMPORTED GLOBAL)

include(ExternalProject)
ExternalProject_Add(ext_ckfft
    PREFIX latest
    URL http://www.crickettechnology.com/public/ckfft-1.3.zip
    URL_HASH MD5=33a4adffd7ad58c6c25d34c143829064
    CMAKE_ARGS -G ${CMAKE_GENERATOR} ../ext_ckfft
    CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS}
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-ckfft.txt <SOURCE_DIR>/CMakeLists.txt
    INSTALL_COMMAND ""
)

add_dependencies(ckfft ext_ckfft)

ExternalProject_Get_Property(ext_ckfft SOURCE_DIR)
ExternalProject_Get_Property(ext_ckfft BINARY_DIR)

if (MSVC)
    set(CKFFT_IMPORTED_LOCATION ${BINARY_DIR}/$(ConfigurationName)/${CMAKE_STATIC_LIBRARY_PREFIX}ckfft${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set(CKFFT_IMPORTED_LOCATION ${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}ckfft${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set_target_properties(ckfft PROPERTIES 
    IMPORTED_LOCATION ${CKFFT_IMPORTED_LOCATION}
#    INTERFACE_INCLUDE_DIRECTORIES ${CKFFT_INTERFACE_INCLUDE_DIRECTORIES}
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET ckfft APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/inc)
