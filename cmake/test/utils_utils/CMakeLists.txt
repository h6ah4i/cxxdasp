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
# Test (test_utils_utils)
#
set(TEST_UTILS_UTILS ${TEST_TOP_DIR}/utils_utils)

add_executable(test_utils_utils
    ${TEST_UTILS_UTILS}/misc.cpp
    ${TEST_UTILS_UTILS}/copy.cpp
    ${TEST_UTILS_UTILS}/multiply_real.cpp
    ${TEST_UTILS_UTILS}/multiply_complex.cpp
    ${TEST_UTILS_UTILS}/conj.cpp
    ${TEST_UTILS_UTILS}/mirror_conj.cpp
    ${TEST_UTILS_UTILS}/interleave.cpp)

target_link_libraries(test_utils_utils cxxdasp gmock gmock_main)

target_include_directories(test_utils_utils
    PRIVATE $<BUILD_INTERFACE:${TEST_TOP_DIR}/include> 
    # PRIVATE $<TARGET_PROPERTY:gmock,INTERFACE_INCLUDE_DIRECTORIES>
    PRIVATE $<TARGET_PROPERTY:gmock,INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK>
)
