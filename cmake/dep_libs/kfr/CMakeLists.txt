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
# KFR library
#
include(ExternalProject)

ExternalProject_Add(ext_kfr
    PREFIX v1.2.0
    GIT_REPOSITORY https://github.com/kfrlib/kfr.git
    GIT_TAG v1.2.0
    CMAKE_ARGS -G ${CMAKE_GENERATOR} ../ext_kfr
    CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS}
    INSTALL_COMMAND ""
)

add_library(kfr STATIC IMPORTED GLOBAL)
add_dependencies(kfr ext_kfr)

ExternalProject_Get_Property(ext_kfr SOURCE_DIR)
ExternalProject_Get_Property(ext_kfr BINARY_DIR)

set_target_properties(kfr PROPERTIES 
    IMPORTED_LOCATION ${BINARY_DIR}/modules/libkfr.a
    # INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}/include>
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET kfr APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/include)
