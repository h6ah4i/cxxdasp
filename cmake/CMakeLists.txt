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
# Declare variables (reffered by subdirectory modules)
#
set(CXXDASP_TOP_DIR ${PROJECT_SOURCE_DIR})

#
# Tweak compiler options
#

# enable C++11
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif()

# enable SSE, SSE2 and SSE3
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse -msse2 -msse3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3")
endif()

# keep assembly listings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -save-temps=obj -fverbose-asm")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps=obj -fverbose-asm")
endif()

# enable profiling
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif()

#
# Sub directories
#
add_subdirectory(dep_libs)
add_subdirectory(cxxdasp)
add_subdirectory(example)
add_subdirectory(test)
