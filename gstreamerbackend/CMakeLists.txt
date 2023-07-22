find_package(OpenCV REQUIRED)

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

#include_directories(
#    addons)

#add_subdirectory(
#    addons)

include_directories(
    sinkbin)

add_definitions(
    -Wall
    -O3
    -march=native)

set(
    GSTREAMERBACKEND_HEADERS
    gstreamerbackend.h
    overlayrenderer.h
    viewpage.h
    recordpage.h
    rtppage.h)

qt4_wrap_cpp(
    GSTREAMERBACKEND_HEADERS_MOC
    ${GSTREAMERBACKEND_HEADERS})

set(
    GSTREAMERBACKEND_SOURCES
    gstreamerbackend.cpp gstreamerbackend_p.h
#    pipeline.cpp pipeline_p.h
#    demosaic.cpp demosaic_p.h
    overlayrenderer.cpp
    gstreamerutils.cpp
    viewpage.cpp viewpage_p.h
    recordpage.cpp recordpage_p.h
    rtppage.cpp)
#    gstsinkbin.c
#    gevinfos.c
#    timeimage.cpp
#    gstgevoverlay.cpp)



add_library(
    gstreamerbackend
    ${GSTREAMERBACKEND_HEADERS_MOC}
    ${GSTREAMERBACKEND_SOURCES})

target_link_libraries(
    gstreamerbackend
    defaultbackend
    gvspgstreamer
    ${GSTREAMER_LIBRARIES}
    ${GSTREAMER_APP_LIBRARIES}
    ${GSTREAMER_VIDEO_LIBRARIES}
    ${OpenCV_LIBS})
