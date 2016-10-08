#
# cxxporthelper library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_cxxporthelper
    PREFIX master
    GIT_REPOSITORY https://github.com/h6ah4i/cxxporthelper.git
    GIT_TAG 767d1c29f6adce8348bd26e475e7b81d4ad79871
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/cxxporthelper
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
