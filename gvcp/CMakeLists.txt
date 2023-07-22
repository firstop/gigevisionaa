find_package(
    Qt4
    REQUIRED QtCore)

include(
    ${QT_USE_FILE})

add_definitions(
    -Wall
    -O2
    -march=native
    ${QT_DEFINITIONS})

set(
    GVCPSERVER_HEADERS
    broadcastudpsocket.h
    gvcpdiscoverer.h
    gvcpclient.h
    gvcpserver.h)

qt4_wrap_cpp(
    GVCPSERVER_MOC
    ${GVCPSERVER_HEADERS})

set(
    GVCPSERVER_SOURCES
    gvcp.h
    packethelper.cpp
    headerhelper.cpp
    discoveryhelper.cpp
    forceiphelper.cpp
    readreghelper.cpp
    writereghelper.cpp
    writememhelper.cpp
    readmemhelper.cpp
    broadcastudpsocket.cpp
    bootstrapregisters.cpp bootstrapregisters_p.h
    gvcpdiscoverer.cpp gvcpdiscoverer_p.h
    gvcpserver.cpp gvcpserver_p.h
    gvcpclient.cpp gvcpclient_p.h)

add_library(
    gvcp SHARED
    ${GVCPSERVER_MOC}
    ${GVCPSERVER_SOURCES})
   
target_link_libraries(
    gvcp
    ${QT_LIBRARIES})

install(
    TARGETS gvcp
    LIBRARY DESTINATION lib)
