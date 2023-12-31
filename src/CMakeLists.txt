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
    Qt6::WebEngineQuick 
    KF6::I18n
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::ConfigWidgets
    KF6::KIOCore 
    KF6::KIOFileWidgets 
    KF6::KIOWidgets
)

set(exec_SRCS
    app.cpp
    main.cpp
    resources.qrc

    logic/colorschemer.cpp
    logic/documentHandler.cpp
    logic/kleverUtility.cpp
    logic/mdHandler.cpp

    logic/emoji/emoticonFilterModel.cpp
    logic/emoji/emojiModel.cpp
    logic/emoji/emojiTones.cpp

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
)

add_executable(klevernotes ${exec_SRCS})
target_link_libraries(klevernotes klevernoteslib)

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

