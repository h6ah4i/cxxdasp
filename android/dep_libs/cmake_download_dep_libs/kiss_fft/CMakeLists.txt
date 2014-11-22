#
# Kiss FFT library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_kiss_fft
    PREFIX v1_3_0
    URL http://sourceforge.net/projects/kissfft/files/kissfft/v1_3_0/kiss_fft130.tar.gz/download
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/kiss_fft
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
