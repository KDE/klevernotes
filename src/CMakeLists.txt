# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

set(lib_SRCS
    logic/painting/pressureequation.cpp
    logic/painting/sketchmodel.cpp
    logic/painting/stroke.cpp
)

add_library(klevernoteslib STATIC ${lib_SRCS})
target_link_libraries(klevernoteslib
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Svg
    KF5::I18n
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::KIOCore
    KF5::ConfigWidgets
    Qt::WebChannel
    Qt::WebEngine
    Qt::WebEngineWidgets)

set(exec_SRCS
    main.cpp
    app.cpp

    logic/colorschemer.cpp
    logic/documentHandler.cpp
    logic/kleverUtility.cpp
    logic/todoHandler.cpp
    logic/mdHandler.cpp
    logic/qmlLinker.cpp
    logic/view.cpp

    logic/painting/pointMaker.cpp
    logic/painting/sketchserializer.cpp
    logic/painting/sketchview.cpp
    logic/painting/strokeitem.cpp
    logic/painting/strokelistitem.cpp
    logic/painting/strokepainter.cpp

    logic/treeview/noteTreeModel.cpp

    resources.qrc)

add_executable(klevernotes ${exec_SRCS})
target_link_libraries(klevernotes klevernoteslib)

find_package(KF5KIO)

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
