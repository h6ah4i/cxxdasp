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
# cxxporthelper library
#
include(ExternalProject)

ExternalProject_Add(ext_cxxporthelper
    PREFIX master
    GIT_REPOSITORY https://github.com/h6ah4i/cxxporthelper.git
    GIT_TAG 767d1c29f6adce8348bd26e475e7b81d4ad79871
    CMAKE_ARGS -G ${CMAKE_GENERATOR} ../ext_cxxporthelper
    CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS}
    INSTALL_COMMAND ""
)

add_library(cxxporthelper STATIC IMPORTED GLOBAL)
add_dependencies(cxxporthelper ext_cxxporthelper)

ExternalProject_Get_Property(ext_cxxporthelper SOURCE_DIR)
ExternalProject_Get_Property(ext_cxxporthelper BINARY_DIR)

if (MSVC)
    set(CXXPORTHELPER_IMPORTED_LOCATION ${BINARY_DIR}/$(ConfigurationName)/${CMAKE_STATIC_LIBRARY_PREFIX}cxxporthelper${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set(CXXPORTHELPER_IMPORTED_LOCATION ${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}cxxporthelper${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set_target_properties(cxxporthelper PROPERTIES 
    IMPORTED_LOCATION ${CXXPORTHELPER_IMPORTED_LOCATION}
    # INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}/include>
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET cxxporthelper APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/include)
