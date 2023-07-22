include_directories(
    commons)

set(
    GEV_QT_HEADERS)


qt4_wrap_cpp(
    GEV_HEADERS_MOC
    ${GEV_QT_HEADERS})


set(
    GEV_SOURCES
    geviport.cpp geviport_p.h
    gevmonitor.cpp gevmonitor_p.h)

add_library(
    gev
    ${GEV_HEADERS_MOC}
    ${GEV_SOURCES})

target_link_libraries(
    gev
    gvcp)
