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
# FFTW library (fftw: double-precision)
#
include(ExternalProject)


if (MSVC)
if (CMAKE_GENERATOR MATCHES " Win64$")

### MSVC - x86_64
### (use pre-built DLLs)

ExternalProject_Add(ext_fftw
    PREFIX 3.3.5
    URL ftp://ftp.fftw.org/pub/fftw/fftw-3.3.5-dll64.zip
    URL_HASH MD5=cb3c5ad19a89864f036e7a2dd5be168c
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/gen_vc_libs_x64.cmd <SOURCE_DIR>/gen_vc_libs_x64.cmd
    BUILD_COMMAND gen_vc_libs_x64.cmd
    INSTALL_COMMAND ""
)

add_library(fftw SHARED IMPORTED GLOBAL)
add_dependencies(fftw ext_fftw)

ExternalProject_Get_Property(ext_fftw SOURCE_DIR)
ExternalProject_Get_Property(ext_fftw BINARY_DIR)

set_target_properties(fftw PROPERTIES 
    IMPORTED_LOCATION ${BINARY_DIR}/libfftw3-3.dll
    IMPORTED_IMPLIB ${BINARY_DIR}/libfftw3-3.lib
    # INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}>
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET fftw APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR})

elseif (CMAKE_GENERATOR MATCHES "ARM$")

### MSVC - ARM
### (not supported)

message(SEND_ERROR "ARM processor is not supported")

else()

### MSVC - x86
### (use pre-built DLLs)

ExternalProject_Add(ext_fftw
    PREFIX 3.3.5
    URL ftp://ftp.fftw.org/pub/fftw/fftw-3.3.5-dll32.zip
    URL_HASH MD5=f9928318c8a35fa8b594aa75af9f0550
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/gen_vc_libs_x86.cmd <SOURCE_DIR>/gen_vc_libs_x86.cmd
    BUILD_COMMAND gen_vc_libs_x86.cmd
    INSTALL_COMMAND ""
)

add_library(fftw SHARED IMPORTED GLOBAL)
add_dependencies(fftw ext_fftw)

ExternalProject_Get_Property(ext_fftw SOURCE_DIR)
ExternalProject_Get_Property(ext_fftw BINARY_DIR)

set_target_properties(fftw PROPERTIES 
    IMPORTED_LOCATION ${BINARY_DIR}/libfftw3-3.dll
    IMPORTED_IMPLIB ${BINARY_DIR}/libfftw3-3.lib
    # INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}>
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET fftw APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR})

endif()
else()

### Other platforms
### (build from source)

set(CONFIGURE_OPTIONS )

if (CXXDASP_TOOLSET_PREFIX)
    set(CONFIGURE_OPTIONS ${CONFIGURE_OPTIONS} --host=${CXXDASP_TOOLSET_PREFIX})
    if(CXXDASP_TOOLSET_PREFIX MATCHES "^arm-.*$")
        # set(CONFIGURE_OPTIONS ${CONFIGURE_OPTIONS} --enable-neon)
    endif()
else()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "i.86" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(CONFIGURE_OPTIONS ${CONFIGURE_OPTIONS} --enable-sse2 --enable-sse3)
    endif()
endif()

ExternalProject_Add(ext_fftw
    PREFIX 3.3.5
    URL http://www.fftw.org/fftw-3.3.5.tar.gz
    URL_HASH MD5=6cc08a3b9c7ee06fdd5b9eb02e06f569
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./configure ${CONFIGURE_OPTIONS}
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} -j8
    INSTALL_COMMAND ""
)

add_library(fftw STATIC IMPORTED GLOBAL)
add_dependencies(fftw ext_fftw)

ExternalProject_Get_Property(ext_fftw SOURCE_DIR)
ExternalProject_Get_Property(ext_fftw BINARY_DIR)

set_target_properties(fftw PROPERTIES 
    IMPORTED_LOCATION ${BINARY_DIR}/.libs/${CMAKE_STATIC_LIBRARY_PREFIX}fftw3${CMAKE_STATIC_LIBRARY_SUFFIX}
    # INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}/api>
)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET fftw APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/api)

endif()
