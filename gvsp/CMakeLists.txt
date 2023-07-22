# libcap-ng
include(
    FindPkgConfig)

pkg_search_module(
    CAPNG REQUIRED libcap-ng)

if (CAPNG_FOUND)
    message(STATUS "cap-ng found")
endif (CAPNG_FOUND)


find_package(
    Qt4
    REQUIRED QtCore
    REQUIRED QtNetwork
    REQUIRED QtDBus)

include(
    ${QT_USE_FILE})

add_definitions(
    -Wall
    -O3
    -march=native
    ${QT_DEFINITIONS})

set(
    GVSP_SOURCES
    gvsp.cpp
    constmemoryblock.h
    gvsppacket.cpp
    gvspblock.cpp
    gvspreceiver.cpp gvspreceiver_p.h)

add_library(
    gvsp SHARED
    ${GVSP_SOURCES})
   
target_link_libraries(
    gvsp
    ${QT_LIBRARIES}
    ${CAPNG_LIBRARIES})

install(
    TARGETS gvsp
    LIBRARY DESTINATION lib)
