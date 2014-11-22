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
# GoogleMock (gmock)
#
include(ExternalProject)

if (MSVC)
    set(GMOCK_CMAKE_CXX_FLAGS /wd4819)    # Ignore warning C4819
    set(GMOCK_OPTS -Dgtest_force_shared_crt:BOOL=ON)
    set(GMOCK_INTERFACE_COMPILE_DEFINITIONS GTEST_HAS_PTHREAD=0)
else()
    set(GMOCK_CMAKE_CXX_FLAGS)
    set(GMOCK_OPTS -Dgtest_disable_pthreads:BOOL=ON)
    set(GMOCK_INTERFACE_COMPILE_DEFINITIONS GTEST_HAS_PTHREAD=0)
endif()

ExternalProject_Add(ext_gmock
    PREFIX 1.7.0
    URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
    URL_HASH SHA1=f9d9dd882a25f4069ed9ee48e70aff1b53e3c5a5
    CMAKE_ARGS -G ${CMAKE_GENERATOR} ../ext_gmock
    CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS} ${GMOCK_OPTS} -DBUILD_SHARED_LIBS:BOOL=OFF -DCMAKE_CXX_FLAGS:STRING=${GMOCK_CMAKE_CXX_FLAGS}
    INSTALL_COMMAND ""
)


ExternalProject_Get_Property(ext_gmock SOURCE_DIR)
ExternalProject_Get_Property(ext_gmock BINARY_DIR)

### gmock
add_library(gmock STATIC IMPORTED GLOBAL)
add_dependencies(gmock ext_gmock)

if (MSVC)
    set(GMOCK_IMPORTED_LOCATION ${BINARY_DIR}/$(ConfigurationName)/${CMAKE_STATIC_LIBRARY_PREFIX}gmock${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(GMOCK_MAIN_IMPORTED_LOCATION ${BINARY_DIR}/$(ConfigurationName)/${CMAKE_STATIC_LIBRARY_PREFIX}gmock_main${CMAKE_STATIC_LIBRARY_SUFFIX})
else()  
    set(GMOCK_IMPORTED_LOCATION ${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gmock${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(GMOCK_MAIN_IMPORTED_LOCATION ${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gmock_main${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set_target_properties(gmock PROPERTIES 
    IMPORTED_LOCATION ${GMOCK_IMPORTED_LOCATION}
    INTERFACE_COMPILE_DEFINITIONS ${GMOCK_INTERFACE_COMPILE_DEFINITIONS}
)

# set_property(TARGET gmock APPEND PROPERTY 
#     INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}/include>)

# set_property(TARGET gmock APPEND PROPERTY 
#     INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${SOURCE_DIR}/gtest/include>)

# NOTE: Workaround of non-existent error on configuration time
#    http://public.kitware.com/Bug/view.php?id=15052
set_property(TARGET gmock APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/include)

set_property(TARGET gmock APPEND PROPERTY 
    INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK ${SOURCE_DIR}/gtest/include)

# if (NOT MSVC)
#     set_property(TARGET gmock APPEND PROPERTY
#         INTERFACE_LINK_LIBRARIES pthread)
# endif()

### gmock_main
add_library(gmock_main STATIC IMPORTED GLOBAL)
add_dependencies(gmock_main ext_gmock)

set_target_properties(gmock_main PROPERTIES 
    IMPORTED_LOCATION ${GMOCK_MAIN_IMPORTED_LOCATION}
)
