#
# FFTS library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_ffts
    PREFIX master
    GIT_REPOSITORY https://github.com/h6ah4i/ffts.git
    GIT_TAG 1f4b9a249263e083d03138957f3cd6a293ae26e5
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/ffts
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
