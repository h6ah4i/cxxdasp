#
# Ne10 library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_ne10
    PREFIX master
    # GIT_REPOSITORY https://github.com/projectNe10/Ne10.git
    GIT_REPOSITORY https://github.com/h6ah4i/Ne10.git
    GIT_TAG c9e38acc720e62dd5dea8bc3a6f1864c58a84fda
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/ne10
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
