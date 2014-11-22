#
# GoogleMock (gmock) (download only)
#
include(ExternalProject)

ExternalProject_Add(download_gmock
    PREFIX 1.7.0
    URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/gmock
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
