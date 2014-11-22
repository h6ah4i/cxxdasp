#
# FFTS library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_ffts
    PREFIX master
    # TODO ./build_android.sh fails if downloaded from the git repository. Why???
    # GIT_REPOSITORY https://github.com/anthonix/ffts.git
    URL https://github.com/anthonix/ffts/archive/master.tar.gz
    BUILD_IN_SOURCE 1
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/ffts
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
