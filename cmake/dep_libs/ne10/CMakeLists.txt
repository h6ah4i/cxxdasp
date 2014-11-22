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
# Ne10 library
#
include(ExternalProject)

ExternalProject_Add(ext_ne10
    PREFIX master
    # GIT_REPOSITORY https://github.com/projectNe10/Ne10.git
    GIT_REPOSITORY https://github.com/h6ah4i/Ne10.git
    GIT_TAG c9e38acc720e62dd5dea8bc3a6f1864c58a84fda
    CMAKE_ARGS -G ${CMAKE_GENERATOR} ../ext_ne10
    CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS} -DGNULINUX_PLATFORM:BOOL=ON -DNE10_BUILD_SHARED:BOOL=OFF -DNE10_BUILD_STATIC:BOOL=ON -DNE10_BUILD_EXAMPLES:BOOL=OFF -DNE10_BUILD_UNIT_TEST:BOOL=OFF -DNE10_ENABLE_MATH:BOOL=OFF -DNE10_ENABLE_DSP:BOOL=ON -DNE10_ENABLE_IMGPROC:BOOL=OFF -DNE10_ENABLE_PHYSICS:BOOL=OFF
    INSTALL_COMMAND ""
)

add_library(ne10 STATIC IMPORTED GLOBAL)
add_dependencies(ne10 ext_ne10)

ExternalProject_Get_Property(ext_ne10 SOURCE_DIR)
ExternalProject_Get_Property(ext_ne10 BINARY_DIR)

set_target_properties(ne10 PROPERTIES 
    IMPORTED_LOCATION ${BINARY_DIR}/modules/libNE10.a
    # INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}/inc>
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET ne10 APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/inc)
