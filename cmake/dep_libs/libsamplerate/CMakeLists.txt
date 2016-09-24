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
# The Secret Rabbit Cocde library (libsamplerate)
#

include(ExternalProject)
ExternalProject_Add(ext_libsamplerate
    PREFIX 0.1.9
    URL http://www.mega-nerd.com/SRC/libsamplerate-0.1.9.tar.gz
    URL_HASH MD5=2b78ae9fe63b36b9fbb6267fad93f259
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./configure --disable-fftw --disable-shared --enable-static --disable-sndfile
    PATCH_COMMAND ""
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} -j8
    INSTALL_COMMAND ""
)

add_library(libsamplerate STATIC IMPORTED GLOBAL)
add_dependencies(libsamplerate ext_libsamplerate)

ExternalProject_Get_Property(ext_libsamplerate SOURCE_DIR)
ExternalProject_Get_Property(ext_libsamplerate BINARY_DIR)

if (MSVC)
    set(LIBSAMPLERATE_IMPORTED_LOCATION ${BINARY_DIR}/$(ConfigurationName)/${CMAKE_STATIC_LIBRARY_PREFIX}samplerate${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set(LIBSAMPLERATE_IMPORTED_LOCATION ${BINARY_DIR}/src/.libs/${CMAKE_STATIC_LIBRARY_PREFIX}samplerate${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set(LIBSAMPLERATE_INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}/src>)

set_target_properties(libsamplerate PROPERTIES 
    IMPORTED_LOCATION ${LIBSAMPLERATE_IMPORTED_LOCATION}
    INTERFACE_INCLUDE_DIRECTORIES ${LIBSAMPLERATE_INTERFACE_INCLUDE_DIRECTORIES}
)
