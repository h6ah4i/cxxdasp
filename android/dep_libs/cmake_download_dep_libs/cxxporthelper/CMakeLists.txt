#
# cxxporthelper library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_cxxporthelper
    PREFIX master
    GIT_REPOSITORY https://github.com/h6ah4i/cxxporthelper.git
    GIT_TAG 30c774dde7cd283e238d0434ba68989f9604e027
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/cxxporthelper
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
