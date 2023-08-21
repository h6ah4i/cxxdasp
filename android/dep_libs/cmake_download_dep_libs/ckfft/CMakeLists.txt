#
# Cricket FFT library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_ckfft
    PREFIX master
    GIT_REPOSITORY https://github.com/sjmerel/ckfft.git
    GIT_TAG 64eb5dad4c569e1dd19264cd09db08c7519a49a6
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/ckfft
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
