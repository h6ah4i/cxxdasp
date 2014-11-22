#
# General purpose FFT package library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_gp_fft
    PREFIX latest
    URL http://www.kurims.kyoto-u.ac.jp/~ooura/fft.tgz
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/gp_fft
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

