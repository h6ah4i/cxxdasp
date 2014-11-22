#
# android-ndk-profiler library (download only)
#
include(ExternalProject)

ExternalProject_Add(download_android-ndk-profiler
    PREFIX latest
    URL https://github.com/richq/android-ndk-profiler/releases/download/v3.3/android-ndk-profiler-prebuilt-3.3.zip
    SOURCE_DIR ${DEP_LIBS_PLACEMENT_DIR}/android-ndk-profiler
    # copy armeabi-v7a/ to armeabi-v7a-hard
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/armeabi-v7a <SOURCE_DIR>/armeabi-v7a-hard
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

