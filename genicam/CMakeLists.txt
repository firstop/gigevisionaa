set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt4 REQUIRED QtCore QtXml)

find_package(ZLIB REQUIRED)

include_directories(qtiocompressor)

set(
    GENICAM_SOURCES
    genicamxmlfile.cpp genicamxmlfile_p.h
    genicamobjectbuilder.cpp genicamobjectbuilder_p.h
    inode.cpp inode_p.h
    inoderegister.cpp inoderegister_p.h
    xmlhelper.cpp xmlhelper_p.h
    category.cpp category_p.h
    integer.cpp integer_p.h
    intreg.cpp intreg_p.h
    stringreg.cpp stringreg_p.h
    enumeration.cpp enumeration_p.h
    enumentry.cpp enumentry_p.h
    jgvfloat.cpp jgvfloat_p.h
    floatreg.cpp floatreg_p.h
    swissknife.cpp swissknife_p.h
    intswissknife.cpp intswissknife_p.h
    converter.cpp converter_p.h
    intconverter.cpp intconverter_p.h
    maskedintreg.cpp maskedintreg_p.h
    boolean.cpp boolean_p.h
    command.cpp command_p.h
    register.cpp register_p.h
    formula.cpp formula_p.h
    genicam.cpp
    token.h


    iinterface.cpp
    icategory.cpp
    istring.cpp
    iinteger.cpp
    ifloat.cpp
    ienumeration.cpp
    iboolean.cpp
    iregister.cpp
    icommand.cpp
    iport.cpp

    qtiocompressor/qtiocompressor.cpp)

if(GUI)
    message(STATUS "Genicam GUI support")
    find_package(Qt4 REQUIRED QtGui)
    include_directories(gui)
    set(GENICAM_SOURCES ${GENICAM_SOURCES}
        gui/genicammodel.cpp gui/genicammodel_p.h
        gui/genicamdelegate.cpp
        gui/genicameditor.cpp
        gui/genicamint64editor.cpp gui/genicamint64editor_p.h
        gui/genicamdoubleeditor.cpp gui/genicamdoubleeditor_p.h
        gui/genicambuttoneditor.cpp gui/genicambuttoneditor_p.h
        gui/genicamcomboboxeditor.cpp gui/genicamcomboboxeditor_p.h
        gui/genicambooleditor.cpp gui/genicambooleditor_p.h
        gui/genicamlineeditor.cpp gui/genicamlineeditor_p.h
        gui/genicamipv4editor.cpp gui/genicamipv4editor_p.h)

endif()

add_library(
    genicam
    ${GENICAM_SOURCES})

target_link_libraries(
    genicam
    ${ZLIB_LIBRARIES}
    Qt4::QtCore Qt4::QtXml)

if(GUI)
    target_link_libraries(genicam Qt4::QtGui)
endif()
