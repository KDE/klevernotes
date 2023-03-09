# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

add_executable(klevernotes
    main.cpp
    app.cpp

    logic/documentHandler.cpp
    logic/kleverUtility.cpp
    logic/storageHandler.cpp
    logic/todoHandler.cpp

    contents/logic/mdHandler.cpp
    contents/logic/qmlLinker.cpp
    contents/logic/view.cpp

    resources.qrc)

find_package(KF5KIO)

target_link_libraries(klevernotes
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
    Qt::WebChannel
    Qt::WebEngine
    Qt::WebEngineWidgets)

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
