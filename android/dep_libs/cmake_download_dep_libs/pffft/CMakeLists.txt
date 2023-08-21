#
# PFFFT library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_pffft
    PREFIX master
    GIT_REPOSITORY https://bitbucket.org/jpommier/pffft.git
    GIT_TAG 1adf4b936bcf4a61c5ae457efc6d6e33692a8c9e
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/pffft
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
