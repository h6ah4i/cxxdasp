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
# Test (test_sample_format_converter)
#
set(TEST_SAMPLE_FORMAT_CONVERTER ${TEST_TOP_DIR}/sample_format_converter)

add_executable(test_sample_format_converter
    ${TEST_SAMPLE_FORMAT_CONVERTER}/sample_format_converter_test.cpp)

target_link_libraries(test_sample_format_converter cxxdasp gmock gmock_main)

target_include_directories(test_sample_format_converter
    PRIVATE $<BUILD_INTERFACE:${TEST_TOP_DIR}/include> 
    # PRIVATE $<TARGET_PROPERTY:gmock,INTERFACE_INCLUDE_DIRECTORIES>
    PRIVATE $<TARGET_PROPERTY:gmock,INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK>
)
