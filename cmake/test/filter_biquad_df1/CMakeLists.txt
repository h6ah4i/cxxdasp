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
# Test (test_filter_biquad_df1)
#
set(TEST_FILTER_BIQUAD_DF1 ${TEST_TOP_DIR}/filter_biquad_df1)

add_executable(test_filter_biquad_df1
    ${TEST_FILTER_BIQUAD_DF1}/biquad_filter_df1_test.cpp)

target_link_libraries(test_filter_biquad_df1 cxxdasp gmock gmock_main)

get_target_property(GMOCK_INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK gmock INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK)

target_include_directories(test_filter_biquad_df1
    PRIVATE $<BUILD_INTERFACE:${TEST_TOP_DIR}/include> 
    # PRIVATE $<TARGET_PROPERTY:gmock,INTERFACE_INCLUDE_DIRECTORIES>
    PRIVATE ${GMOCK_INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK}
)
