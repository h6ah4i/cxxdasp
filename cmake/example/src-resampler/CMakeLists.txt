#
#    Copyright (C) 2015 Haruki Hasegawa
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
# Example (src-resampler)
#
set(EXAMPLE_SRC_RESAMPLER_DIR ${EXAMPLE_TOP_DIR}/src-resampler)

add_executable(src-resampler
    ${EXAMPLE_SRC_RESAMPLER_DIR}/src-resampler.cpp)

target_link_libraries(src-resampler cxxdasp libsamplerate)

target_include_directories(src-resampler
    PRIVATE $<BUILD_INTERFACE:${EXAMPLE_TOP_DIR}/include>
    PRIVATE ${LIB_SOXR_INCLUDE_DIRECTRIES} 
    PRIVATE $<TARGET_PROPERTY:libsamplerate,INTERFACE_INCLUDE_DIRECTORIES>
    #PRIVATE $<TARGET_PROPERTY:libsamplerate,INTERFACE_INCLUDE_DIRECTORIES_NO_EXIST_CHECK>
)
