find_package(PkgConfig)

pkg_check_modules(GSTREAMER gstreamer-1.0)

link_directories(
    ${GSTREAMER_LIBRARY_DIRS})

include_directories(
    ${GSTREAMER_INCLUDE_DIRS})

pkg_check_modules(GSTREAMER_APP gstreamer-app-1.0)

link_directories(
    ${GSTREAMER_APP_LIBRARY_DIRS})

include_directories(
    ${GSTREAMER_APP_INCLUDE_DIRS})

pkg_check_modules(GSTREAMER_VIDEO gstreamer-video-1.0)

link_directories(
    ${GSTREAMER_VIDEO_LIBRARY_DIRS})

include_directories(
    ${GSTREAMER_VIDEO_INCLUDE_DIRS})

include_directories(
    ${jiguiviou_SOURCE_DIR}/commons)

add_definitions(
    -Wall
    -O3
    -march=native)

set(
    GVSPGSTREAMER_HEADERS
    pipeline.h)

qt4_wrap_cpp(
    GVSPGSTREAMER_HEADERS_MOC
    ${GVSPGSTREAMER_HEADERS})

set(
    GVSPGSTREAMER_SOURCES
    pipeline.cpp pipeline_p.h
    demosaic.cpp demosaic_p.h
    gstsinkbin.c
    gevinfos.c
    gstgevoverlay.cpp
    timeimage.cpp)


add_library(
    gvspgstreamer
    ${GVSPGSTREAMER_HEADERS_MOC}
    ${GVSPGSTREAMER_SOURCES})

target_link_libraries(
    gvspgstreamer
    ${GSTREAMER_LIBRARIES}
    ${GSTREAMER_APP_LIBRARIES}
    ${GSTREAMER_VIDEO_LIBRARIES})
