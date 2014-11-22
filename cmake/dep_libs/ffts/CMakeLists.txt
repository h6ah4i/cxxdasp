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

set(CONFIGURE_OPTIONS --enable-static --enable-single)

if (CXXDASP_TOOLSET_PREFIX)
    set(CONFIGURE_OPTIONS ${CONFIGURE_OPTIONS} --host=${CXXDASP_TOOLSET_PREFIX})
    if(CXXDASP_TOOLSET_PREFIX MATCHES "^arm-.*$")
        set(CONFIGURE_OPTIONS ${CONFIGURE_OPTIONS} --enable-neon)
    endif()
else()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "i.86" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(CONFIGURE_OPTIONS ${CONFIGURE_OPTIONS} --enable-sse)
    endif()
endif()

ExternalProject_Add(ext_ffts
    PREFIX master
    # TODO ./configure fails if downloaded from the git repository. Why???
    # GIT_REPOSITORY https://github.com/anthonix/ffts.git
    URL https://github.com/anthonix/ffts/archive/master.tar.gz
    BUILD_IN_SOURCE 1
    # PATCH_COMMAND sh -c "if [ ! -f ./force_autoreconf_done ]$<SEMICOLON> then autoreconf --force --install && touch force_autoreconf_done$<SEMICOLON> fi"
    # CONFIGURE_COMMAND sh -c "if [ ! -f ./configure_done ]$<SEMICOLON> then ./configure --enable-static --enable-single --enable-sse && touch configure_done$<SEMICOLON> fi"
    CONFIGURE_COMMAND ./configure ${CONFIGURE_OPTIONS}
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} -j8
    INSTALL_COMMAND ""
)

add_library(ffts STATIC IMPORTED GLOBAL)
add_dependencies(ffts ext_ffts)

ExternalProject_Get_Property(ext_ffts SOURCE_DIR)
ExternalProject_Get_Property(ext_ffts BINARY_DIR)

set_target_properties(ffts PROPERTIES 
    IMPORTED_LOCATION ${BINARY_DIR}/src/.libs/libffts.a
    # INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}/include>
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET ffts APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/include)
