#
# GoogleTest (googletest) (download only)
#
include(ExternalProject)

ExternalProject_Add(download_googletest
    PREFIX master
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG ec44c6c1675c25b9827aacd08c02433cccde7780
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/googletest
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
