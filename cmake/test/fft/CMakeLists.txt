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
# Test (test_fft)
#
set(TEST_FFT_DIR ${TEST_TOP_DIR}/fft)

add_executable(test_fft
    ${TEST_FFT_DIR}/fft_test.cpp
    ${TEST_FFT_DIR}/bluestein_fft_test.cpp)

target_link_libraries(test_fft cxxdasp gmock gmock_main)

target_include_directories(test_fft
    PRIVATE $<BUILD_INTERFACE:${TEST_TOP_DIR}/include> 
    # PRIVATE $<TARGET_PROPERTY:gmock,INTERFACE_INCLUDE_DIRECTORIES>
    PRIVATE $<TARGET_PROPERTY:gmock,INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK>
)
