#
# Configurations
#
### single-precision FFT libraries
option(CXXDASP_CONFIG_USE_FFT_BACKEND_PFFFT     "Use PFFFT library for single-precision FFT backend  (compatible with all platforms)"     YES)
option(CXXDASP_CONFIG_USE_FFT_BACKEND_FFTS      "Use FFTS library for single-precision FFT backend  (not compatible with MSVC)"           NO)
option(CXXDASP_CONFIG_USE_FFT_BACKEND_KISS_FFT  "Use Kiss FFT library for single-precision FFT backend  (compatible with all platforms)"  NO)
option(CXXDASP_CONFIG_USE_FFT_BACKEND_FFTWF     "Use FFTWF library for single-precision FFT backend  (not compatible with MSVC)"          NO)
option(CXXDASP_CONFIG_USE_FFT_BACKEND_NE10      "Use Ne10 library for single-precision FFT backend  (compatible with ARMv7 only)"         NO)
option(CXXDASP_CONFIG_USE_FFT_BACKEND_CKFFT     "Use Cricket FFT library for single-precision FFT backend  (compatible with all platforms)"  NO)
option(CXXDASP_CONFIG_USE_FFT_BACKEND_MUFFT     "Use muFFT library for single-precision FFT backend  (compatible with all platforms)"     YES)
option(CXXDASP_CONFIG_USE_FFT_BACKEND_KFR_F     "Use KFR library for single-precision FFT backend  (compatible with all platforms)"       NO)

### double-precision FFT libraries
option(CXXDASP_CONFIG_USE_FFT_BACKEND_GP_FFT    "Use General Purpose FFT package for double-precision FFT backend (compatible with all platforms)" YES)
option(CXXDASP_CONFIG_USE_FFT_BACKEND_FFTW      "Use FFTW library for double-precision FFT backend  (not compatible with MSVC)"           NO)
option(CXXDASP_CONFIG_USE_FFT_BACKEND_KFR_D     "Use KFR library for double-precision FFT backend  (compatible with all platforms)"       NO)

#
# Build targets
#
option(CXXDASP_BUILD_EXAMPLE_SIMPLE_RESAMPLER       "Build simple-resampler example app"                YES)
option(CXXDASP_BUILD_EXAMPLE_BIQUAD_FILTER          "Build biquad-filter example app"                   YES)
option(CXXDASP_BUILD_EXAMPLE_CASCADED_BIQUAD_FILTER "Build cascaded-biquad-filter example app"          YES)
option(CXXDASP_BUILD_EXAMPLE_TSVF                   "Build trapezoidal-state-variable-filter example app" YES)
option(CXXDASP_BUILD_EXAMPLE_CASCADED_TSVF          "Build cascaded-trapezoidal-state-variable-filter example app" YES)
option(CXXDASP_BUILD_EXAMPLE_SOXR_RESAMPLER         "Build soxr-resampler example app"                  YES)
option(CXXDASP_BUILD_EXAMPLE_FRESAMPLE_RESAMPLER    "Build fresample-resampler example app"                  YES)
option(CXXDASP_BUILD_EXAMPLE_SECRET_RABBIT_CODE_RESAMPLER "Build src-resampler example app"            YES)
option(CXXDASP_BUILD_EXAMPLE_X2_RESAMPLER           "Build x2-resampler example app"                    YES)
option(CXXDASP_BUILD_EXAMPLE_SAMPLE_FORMAT_CONVERTER "Build sample-format-converter example app"        YES)
option(CXXDASP_BUILD_EXAMPLE_WINDOW_FUNCTION        "Build window-function example app"                 YES)

option(CXXDASP_BUILD_TEST_UTILS_UTILS               "Build utils_utils test tartet"                     YES)
option(CXXDASP_BUILD_TEST_FFT                       "Build fft test target"                             YES)
option(CXXDASP_BUILD_TEST_RESAMPLER_POLYPHASE       "Build resampler_polyphase test target"             YES)
option(CXXDASP_BUILD_TEST_FILTER_BIQUAD_DF1         "Build filter_biquad_df1 test target"               YES)
option(CXXDASP_BUILD_TEST_FILTER_BIQUAD_TDF2        "Build filter_biquad_tdf2 test target"              YES)
option(CXXDASP_BUILD_TEST_FILTER_CASCADED_BIQUAD_DF1 "Build filter_cascaded_biquad_df1 test target"     YES)
option(CXXDASP_BUILD_TEST_FILTER_CASCADED_BIQUAD_TDF2 "Build filter_cascaded_biquad_tdf2 test target"   YES)
option(CXXDASP_BUILD_TEST_FILTER_TSVF               "Build filter_tsvf test target"                     YES)
option(CXXDASP_BUILD_TEST_FILTER_CASCADED_TSVF      "Build filter_cascaded_tsvf test target"            YES)
option(CXXDASP_BUILD_TEST_SAMPLE_FORMAT_CONVERTER   "Build sample_format_converter test target"         YES)
option(CXXDASP_BUILD_TEST_MIXER                     "Build mixer test target"                           YES)
