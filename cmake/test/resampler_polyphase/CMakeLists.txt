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
# Test (test_resampler_polyphase)
#
set(TEST_RESAMPLER_POLYPHASE ${TEST_TOP_DIR}/resampler_polyphase)

add_executable(test_resampler_polyphase
    ${TEST_RESAMPLER_POLYPHASE}/polyphase_core_operator.cpp)

target_link_libraries(test_resampler_polyphase cxxdasp gmock gmock_main)

target_include_directories(test_resampler_polyphase
    PRIVATE $<BUILD_INTERFACE:${TEST_TOP_DIR}/include> 
    # PRIVATE $<TARGET_PROPERTY:gmock,INTERFACE_INCLUDE_DIRECTORIES>
    PRIVATE $<TARGET_PROPERTY:gmock,INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK>
)
