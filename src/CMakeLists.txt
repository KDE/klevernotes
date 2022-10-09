# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

add_executable(klever
    main.cpp
    about.cpp
    app.cpp

    logic/documentHandler.cpp
    logic/storageHandler.cpp
    logic/kleverUtility.cpp

    contents/logic/view.cpp

    resources.qrc)

target_link_libraries(klever
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Svg
    KF5::I18n
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::ConfigGui)

if (ANDROID)
    kirigami_package_breeze_icons(ICONS
        list-add
        help-about
        application-exit
        applications-graphics
    )
endif()

# add_subdirectory(contents/logic)

kconfig_add_kcfg_files(klever GENERATE_MOC kleverconfig.kcfgc)
install(TARGETS klever ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
