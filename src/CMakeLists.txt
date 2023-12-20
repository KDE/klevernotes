# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

set(lib_SRCS
    logic/painting/pressureequation.cpp
    logic/painting/sketchmodel.cpp
    logic/painting/stroke.cpp
)

add_library(klevernoteslib STATIC ${lib_SRCS})
target_link_libraries(klevernoteslib
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::QuickControls2
    Qt::Svg
    Qt::WebChannel
    Qt::WebEngineWidgets
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::CoreAddons
    KF${QT_MAJOR_VERSION}::ConfigCore
    KF${QT_MAJOR_VERSION}::ConfigGui
    KF${QT_MAJOR_VERSION}::ConfigWidgets
)

if(QT_MAJOR_VERSION STREQUAL "5")
       set(QTQUICKDIALOG_VERSION "1.3")
       set(FILTER_NAMING "filterRole")
       # set(KIRIGAMI_LISTITEM_ICON "icon")
       set(KIRIGAMI_ACTION_ICON "iconName")
       set(KIRIGAMI_PAGE_ACTION "actions.contextualActions")

       target_link_libraries(klevernoteslib Qt::WebEngine KF5::KIOCore)
else()
       set(QTQUICKDIALOG_VERSION "")
       set(FILTER_NAMING "filterRoleName")
       # set(KIRIGAMI_LISTITEM_ICON "icon.name")
       set(KIRIGAMI_ACTION_ICON "icon.name")
       set(KIRIGAMI_PAGE_ACTION "actions")

       target_link_libraries(klevernoteslib Qt6::WebEngineQuick KF6::KIOCore KF6::KIOFileWidgets KF6::KIOWidgets)
endif()

configure_file(contents/ui/sideBar/ActionBar.qml ${CMAKE_CURRENT_BINARY_DIR}/contents/ui/sideBar/ActionBar.qml)
configure_file(contents/ui/sideBar/SearchBar.qml ${CMAKE_CURRENT_BINARY_DIR}/contents/ui/sideBar/SearchBar.qml)
configure_file(contents/ui/sideBar/Sidebar.qml ${CMAKE_CURRENT_BINARY_DIR}/contents/ui/sideBar/Sidebar.qml)

configure_file(contents/ui/textEditor/BottomToolBar.qml ${CMAKE_CURRENT_BINARY_DIR}/contents/ui/textEditor/BottomToolBar.qml)
configure_file(contents/ui/textEditor/NotesMap.qml ${CMAKE_CURRENT_BINARY_DIR}/contents/ui/textEditor/NotesMap.qml)
configure_file(contents/ui/textEditor/TextToolBar.qml ${CMAKE_CURRENT_BINARY_DIR}/contents/ui/textEditor/TextToolBar.qml)

configure_file(contents/ui/dialogs/FilePickerDialog.qml ${CMAKE_CURRENT_BINARY_DIR}/contents/ui/dialogs/FilePickerDialog.qml)

configure_file(contents/ui/pages/MainPage.qml ${CMAKE_CURRENT_BINARY_DIR}/contents/ui/pages/MainPage.qml)
configure_file(contents/ui/pages/PaintingPage.qml ${CMAKE_CURRENT_BINARY_DIR}/contents/ui/pages/PaintingPage.qml)

configure_file(resources.qrc ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

set(exec_SRCS
    main.cpp
    app.cpp

    logic/colorschemer.cpp
    logic/documentHandler.cpp
    logic/kleverUtility.cpp
    logic/mdHandler.cpp

    logic/noteMapper/noteMapper.cpp
    logic/noteMapper/noteMapperUtils.cpp

    logic/painting/pointMaker.cpp
    logic/painting/sketchserializer.cpp
    logic/painting/sketchview.cpp
    logic/painting/strokeitem.cpp
    logic/painting/strokelistitem.cpp
    logic/painting/strokepainter.cpp

    logic/parser/blockLexer.cpp
    logic/parser/inlineLexer.cpp
    logic/parser/parser.cpp
    logic/parser/renderer.cpp

    logic/printing/printingHelper.cpp

    logic/syntaxHighlight/highlightHelper.cpp

    logic/treeview/noteTreeModel.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc
)

add_executable(klevernotes ${exec_SRCS})
target_link_libraries(klevernotes klevernoteslib)
find_package(KF${QT_MAJOR_VERSION}KIO)


if (ANDROID)
    kirigami_package_breeze_icons(ICONS
        list-add
        help-about
        application-exit
        applications-graphics
    )
endif()

kconfig_add_kcfg_files(klevernotes GENERATE_MOC kleverconfig.kcfgc)
install(TARGETS klevernotes ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

