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
# Example (window-function)
#
set(EXAMPLE_WINDOW_FUNCTION_DIR ${EXAMPLE_TOP_DIR}/window-function)

add_executable(window-function
    ${EXAMPLE_WINDOW_FUNCTION_DIR}/window-function.cpp)

target_link_libraries(window-function cxxdasp)

target_include_directories(window-function
    PRIVATE $<BUILD_INTERFACE:${EXAMPLE_TOP_DIR}/include>
)
