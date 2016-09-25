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
# PFFFT library
#
include(ExternalProject)

ExternalProject_Add(ext_pffft
    PREFIX latest
    # URL https://bitbucket.org/jpommier/pffft/get/default.tar.gz
    # URL https://bitbucket.org/h6a_h4i/pffft/get/default.tar.gz
    URL https://bitbucket.org/h6a_h4i/pffft/get/bc00500.tar.gz
    # HG_REPOSITORY https://jpommier@bitbucket.org/h6a_h4i/pffft
    # HG_REPOSITORY https://h6a_h4i@bitbucket.org/h6a_h4i/pffft
    CMAKE_ARGS -G ${CMAKE_GENERATOR} ../ext_pffft
    CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS}
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-pffft.txt <SOURCE_DIR>/CMakeLists.txt
    INSTALL_COMMAND ""
)

add_library(pffft STATIC IMPORTED GLOBAL)
add_dependencies(pffft ext_pffft)

ExternalProject_Get_Property(ext_pffft SOURCE_DIR)
ExternalProject_Get_Property(ext_pffft BINARY_DIR)

if (MSVC)
    set(PFFFT_IMPORTED_LOCATION ${BINARY_DIR}/$(ConfigurationName)/${CMAKE_STATIC_LIBRARY_PREFIX}pffft${CMAKE_STATIC_LIBRARY_SUFFIX})
else()  
    set(PFFFT_IMPORTED_LOCATION ${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}pffft${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set(PFFFT_INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}>)

set_target_properties(pffft PROPERTIES 
    IMPORTED_LOCATION ${PFFFT_IMPORTED_LOCATION}
    INTERFACE_INCLUDE_DIRECTORIES ${PFFFT_INTERFACE_INCLUDE_DIRECTORIES}
)
