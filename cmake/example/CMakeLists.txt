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
set(EXAMPLE_TOP_DIR ${CXXDASP_TOP_DIR}/example)

#
# Sub directories
#
if (${CXXDASP_BUILD_EXAMPLE_SIMPLE_RESAMPLER})
    add_subdirectory(simple-resampler)
endif()

if (${CXXDASP_BUILD_EXAMPLE_BIQUAD_FILTER})
    add_subdirectory(biquad-filter)
endif()

if (${CXXDASP_BUILD_EXAMPLE_CASCADED_BIQUAD_FILTER})
    add_subdirectory(cascaded-biquad-filter)
endif()

if (${CXXDASP_BUILD_EXAMPLE_TSVF})
    add_subdirectory(trapezoidal-state-variable-filter)
endif()

if (${CXXDASP_BUILD_EXAMPLE_CASCADED_TSVF})
    add_subdirectory(cascaded-trapezoidal-state-variable-filter)
endif()

if (${CXXDASP_BUILD_EXAMPLE_SAMPLE_FORMAT_CONVERTER})
    add_subdirectory(sample-format-converter)
endif()

if (${CXXDASP_BUILD_EXAMPLE_SOXR_RESAMPLER})
    add_subdirectory(soxr-resampler)
endif()

if (${CXXDASP_BUILD_EXAMPLE_FRESAMPLE_RESAMPLER})
    add_subdirectory(fresample-resampler)
endif()

if (${CXXDASP_BUILD_EXAMPLE_X2_RESAMPLER})
    add_subdirectory(x2-resampler)
endif()

if (${CXXDASP_BUILD_EXAMPLE_WINDOW_FUNCTION})
    add_subdirectory(window-function)
endif()
