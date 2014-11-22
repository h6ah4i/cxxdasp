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
# Example (sample-format-converter)
#
set(EXAMPLE_SIMPLE_RESAMPLER_DIR ${EXAMPLE_TOP_DIR}/sample-format-converter)

add_executable(sample-format-converter
    ${EXAMPLE_SIMPLE_RESAMPLER_DIR}/sample-format-converter.cpp)

target_link_libraries(sample-format-converter cxxdasp)

target_include_directories(sample-format-converter
    PRIVATE $<BUILD_INTERFACE:${EXAMPLE_TOP_DIR}/include>
)
