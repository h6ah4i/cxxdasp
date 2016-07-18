#
# cxxporthelper library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_cxxporthelper
    PREFIX master
    GIT_REPOSITORY https://github.com/h6ah4i/cxxporthelper.git
    GIT_TAG b17619379e43089e214fe476b31ee800660e9acd
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/cxxporthelper
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
