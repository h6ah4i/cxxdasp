#
# Cricket FFT library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_ckfft
    PREFIX latest
    URL http://www.crickettechnology.com/public/ckfft-1.3.zip
    URL_HASH MD5=33a4adffd7ad58c6c25d34c143829064
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/ckfft
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
