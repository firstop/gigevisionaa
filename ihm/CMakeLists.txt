

include_directories(
    ${jiguiviou_SOURCE_DIR}/gev
    ${jiguiviou_SOURCE_DIR}/remotegvsp)

set(
    IHM_HEADERS
    mainwidget.h
    controlsview.h
#    linearproxymodel.h
    genicamtreeview.h
    jgvwizardpage.h
    discoverypage.h
    forceippage.h
    gvsppage.h
    networkselectionpage.h
    camerasession.h
    sessionstoolbar.h
    devicetoolbar.h
    genicamcommandaction.h)

qt4_wrap_cpp(
    IHM_HEADERS_MOC
    ${IHM_HEADERS})

set(
    IHM_SOURCES
    mainwidget.cpp
    controlsview.cpp
#    linearproxymodel.cpp
    genicamtreeview.cpp
    jgvwizardpage.cpp jgvwizardpage_p.h
    discoverypage.cpp discoverypage_p.h
    forceippage.cpp forceippage_p.h
    networkselectionpage.cpp networkselectionpage_p.h
    gvsppage.cpp gvsppage_p.h
    lineseparator.cpp
    camerasession.cpp camerasession_p.h
    sessionstoolbar.cpp sessionstoolbar_p.h
    devicetoolbar.cpp devicetoolbar_p.h
    genicamcommandaction.cpp genicamcommandaction_p.h)

add_library(
    ihm
    ${IHM_HEADERS_MOC}
    ${IHM_SOURCES})

