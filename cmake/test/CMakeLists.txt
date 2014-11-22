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
set(TEST_TOP_DIR ${CXXDASP_TOP_DIR}/test)

#
# Sub directories
#
add_subdirectory(gmock)

if (${CXXDASP_BUILD_TEST_UTILS_UTILS})
    add_subdirectory(utils_utils)
endif()

if (${CXXDASP_BUILD_TEST_FFT})
    add_subdirectory(fft)
endif()

if (${CXXDASP_BUILD_TEST_RESAMPLER_POLYPHASE})
    add_subdirectory(resampler_polyphase)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_BIQUAD_DF1})
    add_subdirectory(filter_biquad_df1)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_BIQUAD_TDF2})
    add_subdirectory(filter_biquad_tdf2)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_CASCADED_BIQUAD_DF1})
    add_subdirectory(filter_cascaded_biquad_df1)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_CASCADED_BIQUAD_TDF2})
    add_subdirectory(filter_cascaded_biquad_tdf2)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_TSVF})
    add_subdirectory(filter_tsvf)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_CASCADED_TSVF})
    add_subdirectory(filter_cascaded_tsvf)
endif()

if (${CXXDASP_BUILD_TEST_SAMPLE_FORMAT_CONVERTER})
    add_subdirectory(sample_format_converter)
endif()

if (${CXXDASP_BUILD_TEST_MIXER})
    add_subdirectory(mixer)
endif()

#
# Tests
#
if (${CXXDASP_BUILD_TEST_FFT})
    add_test(NAME fft COMMAND test_fft)
endif()

if (${CXXDASP_BUILD_TEST_UTILS_UTILS})
    add_test(NAME utils_utils COMMAND test_utils_utils)
endif()

if (${CXXDASP_BUILD_TEST_RESAMPLER_POLYPHASE})
    add_test(NAME resampler_polyphase COMMAND test_resampler_polyphase)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_BIQUAD_DF1})
    add_test(NAME filter_biquad_df1 COMMAND test_filter_biquad_df1)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_BIQUAD_TDF2})
    add_test(NAME filter_biquad_df2 COMMAND test_filter_biquad_tdf2)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_CASCADED_BIQUAD_DF1})
    add_test(NAME filter_cascaded_biquad_df1 COMMAND test_filter_cascaded_biquad_df1)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_CASCADED_BIQUAD_TDF2})
    add_test(NAME filter_cascaded_biquad_tdf2 COMMAND test_filter_cascaded_biquad_tdf2)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_TSVF})
    add_test(NAME filter_tsvf COMMAND test_filter_tsvf)
endif()

if (${CXXDASP_BUILD_TEST_FILTER_CASCADED_TSVF})
    add_test(NAME filter_cascaded_tsvf COMMAND test_filter_cascaded_tsvf)
endif()

if (${CXXDASP_BUILD_TEST_SAMPLE_FORMAT_CONVERTER})
    add_test(NAME sample_format_converter COMMAND test_sample_format_converter)
endif()

if (${CXXDASP_BUILD_TEST_MIXER})
    add_test(NAME mixer COMMAND test_mixer)
endif()
