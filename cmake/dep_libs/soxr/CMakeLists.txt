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
# The SoX Resampler library (soxr)
#
include(ExternalProject)


if (MSVC)
set(SOXR_CMAKE_C_FLAGS /wd4819)    # Ignore warning C4819
set(SOXR_CMAKE_CXX_FLAGS /wd4819)    # Ignore warning C4819
else()
set(SOXR_CMAKE_C_FLAGS)
set(SOXR_CMAKE_CXX_FLAGS)
endif()

ExternalProject_Add(ext_soxr
    PREFIX 0.1.1
    # NOTE: tar.xz support was added since cmake 3.1
    # URL http://sourceforge.net/projects/soxr/files/soxr-0.1.1-Source.tar.xz/download
    # GIT_REPOSITORY git://git.code.sf.net/p/soxr/code
    GIT_REPOSITORY http://git.code.sf.net/p/soxr/code
    GIT_TAG 0.1.1
    CMAKE_ARGS -G ${CMAKE_GENERATOR} ../ext_soxr
    CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS} -DCMAKE_C_FLAGS:STRING=${SOXR_CMAKE_C_FLAGS} -DCMAKE_CXX_FLAGS:STRING=${SOXR_CMAKE_CXX_FLAGS} -DBUILD_TESTS:BOOL=OFF -DBUILD_SHARED_LIBS:BOOL=OFF -DWITH_OPENMP:BOOL=OFF -DWITH_LSR_BINDINGS:BOOL=OFF
    INSTALL_COMMAND ""
)

add_library(soxr STATIC IMPORTED GLOBAL)
add_dependencies(soxr ext_soxr)

ExternalProject_Get_Property(ext_soxr SOURCE_DIR)
ExternalProject_Get_Property(ext_soxr BINARY_DIR)

if (MSVC)
    set(SOXR_IMPORTED_LOCATION ${BINARY_DIR}/src/$(ConfigurationName)/${CMAKE_STATIC_LIBRARY_PREFIX}soxr${CMAKE_STATIC_LIBRARY_SUFFIX})
else()  
    set(SOXR_IMPORTED_LOCATION ${BINARY_DIR}/src/${CMAKE_STATIC_LIBRARY_PREFIX}soxr${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set_target_properties(soxr PROPERTIES 
    IMPORTED_LOCATION ${SOXR_IMPORTED_LOCATION}
    # INTERFACE_INCLUDE_DIRECTORIES ${SOURCE_DIR}/src
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET soxr APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/src)
