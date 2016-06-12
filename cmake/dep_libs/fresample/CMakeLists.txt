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
# libfresample library
#
include(ExternalProject)

ExternalProject_Add(ext_fresample
    PREFIX master
    GIT_REPOSITORY https://github.com/depp/libfresample.git
    GIT_TAG 884c39a71a3af579c40eb2798854b82bf54a4b37
    CMAKE_ARGS -G ${CMAKE_GENERATOR} ../ext_fresample
    CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS}
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists-fresample.txt <SOURCE_DIR>/CMakeLists.txt
    INSTALL_COMMAND ""
)

add_library(fresample STATIC IMPORTED GLOBAL)
add_dependencies(fresample ext_fresample)

ExternalProject_Get_Property(ext_fresample SOURCE_DIR)
ExternalProject_Get_Property(ext_fresample BINARY_DIR)

set_target_properties(fresample PROPERTIES
    IMPORTED_LOCATION ${BINARY_DIR}/libfresample.a
    # INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}/include>
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET fresample APPEND PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/include)
