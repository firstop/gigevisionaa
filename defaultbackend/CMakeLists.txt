set(
    DEFAULTBACKEND_HEADERS
    defaultbackend.h)

qt4_wrap_cpp(
    DEFAULTBACKEND_HEADERS_MOC
    ${DEFAULTBACKEND_HEADERS})

set(
    DEFAULTBACKEND_SOURCES
    defaultbackend.cpp defaultbackend_p.h)

add_library(
    defaultbackend
    ${DEFAULTBACKEND_HEADERS_MOC}
    ${DEFAULTBACKEND_SOURCES})

