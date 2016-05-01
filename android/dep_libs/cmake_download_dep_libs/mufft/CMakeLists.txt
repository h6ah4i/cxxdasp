#
# muFFT library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_mufft
    PREFIX latest
    URL https://github.com/Themaister/muFFT/archive/6d716abf9d813627338b9580589fc442975559d1.tar.gz
    URL_HASH MD5=ea8363a7b6c0ca4c7177439e2d035411
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/mufft
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/complex.h <SOURCE_DIR>/complex.h
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ${DEP_LIBS_PLACEMENT_DIR}/mufft/fft.h <SOURCE_DIR>/inc/mufft/fft.h
)
