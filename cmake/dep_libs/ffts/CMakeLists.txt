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
# FFTS library
#
include(ExternalProject)

ExternalProject_Add(ext_ffts
    PREFIX master
    GIT_REPOSITORY https://github.com/h6ah4i/ffts.git
    GIT_TAG 1f4b9a249263e083d03138957f3cd6a293ae26e5
    INSTALL_COMMAND ""
)

add_library(ffts STATIC IMPORTED GLOBAL)
add_dependencies(ffts ext_ffts)

ExternalProject_Get_Property(ext_ffts SOURCE_DIR)
ExternalProject_Get_Property(ext_ffts BINARY_DIR)

set_target_properties(ffts PROPERTIES 
    IMPORTED_LOCATION ${BINARY_DIR}/libffts.a
    # INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}/include>
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET ffts APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/include)
