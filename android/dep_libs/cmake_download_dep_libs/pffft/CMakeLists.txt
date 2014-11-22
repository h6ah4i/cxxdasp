#
# PFFFT library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_pffft
    PREFIX latest
    # URL https://bitbucket.org/jpommier/pffft/get/default.tar.gz
    # URL https://bitbucket.org/h6a_h4i/pffft/get/default.tar.gz
    URL https://bitbucket.org/h6a_h4i/pffft/get/a8db214.tar.gz
    # HG_REPOSITORY https://jpommier@bitbucket.org/h6a_h4i/pffft
    # HG_REPOSITORY https://h6a_h4i@bitbucket.org/h6a_h4i/pffft
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/pffft
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
