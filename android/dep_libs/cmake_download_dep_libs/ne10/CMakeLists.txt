#
# Ne10 library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_ne10
    PREFIX master
    # GIT_REPOSITORY https://github.com/projectNe10/Ne10.git
    GIT_REPOSITORY https://github.com/h6ah4i/Ne10.git
    GIT_TAG 6b797854f2d3498557619c6dfaa9ad55bc41663c
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/ne10
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
