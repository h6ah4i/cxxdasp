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
# Example (cascaded-biquad-filter)
#
set(EXAMPLE_CASCADED_BIQUAD_FILTER_DIR ${EXAMPLE_TOP_DIR}/cascaded-biquad-filter)

add_executable(cascaded-biquad-filter
    ${EXAMPLE_CASCADED_BIQUAD_FILTER_DIR}/cascaded-biquad-filter.cpp)

target_link_libraries(cascaded-biquad-filter cxxdasp)

target_include_directories(cascaded-biquad-filter
    PRIVATE $<BUILD_INTERFACE:${EXAMPLE_TOP_DIR}/include>
)
