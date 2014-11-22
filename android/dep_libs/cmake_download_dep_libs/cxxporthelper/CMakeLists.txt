#
# cxxporthelper library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_cxxporthelper
    PREFIX master
    GIT_REPOSITORY https://github.com/h6ah4i/cxxporthelper.git
    GIT_TAG 8b997ca1b8b5d43d9c3f9c96f2440013881d340c
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/cxxporthelper
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
