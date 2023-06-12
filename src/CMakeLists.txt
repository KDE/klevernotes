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
    )

if (QT_MAJOR_VERSION LESS 6)
    target_link_libraries(klevernoteslib Qt::WebEngine KF5::KIOCore)
else()
    target_link_libraries(klevernoteslib Qt6::WebEngineQuick KF6::KIOCore KF6::KIOFileWidgets KF6::KIOWidgets)
endif()

set(exec_SRCS
    main.cpp
    app.cpp

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

