project(jiguiviou)
cmake_minimum_required( VERSION 2.8)

set(
    CMAKE_BUILD_TYPE
#    Release)
    Debug)

option(GUI "Build with QtGui" ON)
if(GUI)
    MESSAGE(STATUS "QtGui support")
    add_definitions(-DGUI)
endif()

option(GENICAM "Build with GenICam control support" ON)
if(GENICAM)
    MESSAGE(STATUS "GenICam support")
    add_definitions(-DGENICAM)
    include_directories(genicam)
    add_subdirectory(genicam)
endif()


set(
    CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(
    Qt4
    REQUIRED QtCore QtGui QtNetwork QtXml QtDbus)

set(
    QT_USE_QTNETWORK TRUE)

set(
    QT_USE_QTXML TRUE)

set(
    QT_USE_QTDBUS TRUE)


include(
    ${QT_USE_FILE})

add_definitions(
    ${QT_DEFINITIONS})

include_directories(
    gev
    defaultbackend
    gstreamerbackend
    ihm
    remotegsvp
    gvsp
    gvspgstreamer
    gvcp)

add_subdirectory(
    gvsp)

add_subdirectory(
    gvspgstreamer)

add_subdirectory(
    gstreamerbackend)

add_subdirectory(
    defaultbackend)

add_subdirectory(
    gev)

add_subdirectory(
    ihm)

add_subdirectory(
    gvcp)

set(
    JIGUIVIOU_SOURCES
    main.cpp)

set(
    JIGUIVIOU_RESOURCES
    ressources.qrc)

qt4_add_resources(
    JIGUIVIOU_RESOURCES_RCC
    ${JIGUIVIOU_RESOURCES})

add_definitions(
    -Wall
    -O3
    -march=native)



# sortie complete make
set(
    CMAKE_VERBOSE_MAKEFILE
    OFF)

# fixe le chemin de liaison dans la section dynamique elf
set(
    CMAKE_INSTALL_RPATH
    "${CMAKE_INSTALL_PREFIX}/lib")

# remplit la section RPATH du binaire au moment de l'installation
set(
    CMAKE_INSTALL_RPATH_USE_LINK_PATH
    TRUE)

# demande la duplication du RPATH vers RUNPATH
set(
    CMAKE_EXE_LINKER_FLAGS
    "-Wl,--enable-new-dtags")


add_executable(
    ${PROJECT_NAME}
    ${JIGUIVIOU_SOURCES}
    ${JIGUIVIOU_RESOURCES_RCC})

target_link_libraries(
    ${PROJECT_NAME}
    ihm
    gev
    gvcp
    gvsp
    genicam
    gstreamerbackend
    ${QT_LIBRARIES})

install(TARGETS
    ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)

find_program(
    FILECAP_EXECUTABLE
    filecap)

if(FILECAP_EXECUTABLE)
    set(FILECAP_FOUND true)
else(FILECAP_EXECUTABLE)
    message(WARNING "failed to find filecap, ${PROJECT_NAME} will not have capacities net_raw !")
    set(FILECAP_FOUND false)
endif(FILECAP_EXECUTABLE)

install(
CODE
"if(${FILECAP_FOUND} STREQUAL \"true\")
    message(STATUS \"doing filecap ${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME} net_raw\")
    execute_process (
            COMMAND ${FILECAP_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME} net_raw
            RESULT_VARIABLE FILECAP_RESULT)
    if(FILECAP_RESULT)
        message( WARNING \"filecap failed !\")
    endif()
else()
    message( WARNING \"filecap failed, can't find cap-ng !\")
endif()"
)

