# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

set(lib_SRCS
    app.cpp

    # General utility
    logic/colorschemer.cpp
    logic/documentHandler.cpp
    logic/kleverUtility.cpp
    logic/mdHandler.cpp
    logic/qmlLinker.cpp

    # Editor
    logic/editor/colors.cpp
    logic/editor/editorHandler.cpp
    logic/editor/syntaxvisitor.cpp

    # Extended Syntax
    logic/extendedSyntax/extendedSyntaxHelper.cpp
    logic/extendedSyntax/extendedSyntaxMaker.cpp
    logic/extendedSyntax/md4qtHelperFunc.cpp

    # PAINTING
    logic/painting/pointMaker.cpp
    logic/painting/sketchserializer.cpp
    logic/painting/sketchview.cpp
    logic/painting/strokeitem.cpp
    logic/painting/strokelistitem.cpp
    logic/painting/strokepainter.cpp
    logic/painting/pressureequation.cpp
    logic/painting/sketchmodel.cpp
    logic/painting/stroke.cpp

    # PARSER
    logic/parser/parser.cpp
    logic/parser/renderer.cpp

    # Printing
    logic/printing/printingHelper.cpp

    # Treeview
    logic/treeview/noteTreeModel.cpp
    
    # PLUGINS
    logic/plugins/cliHelper.cpp
    logic/plugins/pluginHelper.cpp

    logic/plugins/emoji/emoticonFilterModel.cpp
    logic/plugins/emoji/emojiModel.cpp
    logic/plugins/emoji/emojiTones.cpp

    logic/plugins/noteMapper/noteMapper.cpp
    logic/plugins/noteMapper/noteMapperParserUtils.cpp
    logic/plugins/noteMapper/noteMapperUtils.cpp

    logic/plugins/puml/pumlHelper.cpp
    logic/plugins/puml/pumlParserUtils.cpp

    logic/plugins/syntaxHighlight/highlightHelper.cpp
    logic/plugins/syntaxHighlight/highlightParserUtils.cpp
)

add_library(klevernoteslib STATIC ${lib_SRCS})

kconfig_add_kcfg_files(klevernoteslib GENERATE_MOC kleverconfig.kcfgc)

target_link_libraries(klevernoteslib PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::QuickControls2
    Qt::Svg
    Qt::WebChannel
    Qt::PrintSupport
    Qt6::WebEngineQuick 
    KF6::I18n
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::ConfigWidgets
    KF6::KIOCore 
)

add_executable(klevernotes main.cpp resources.qrc)
target_link_libraries(klevernotes PRIVATE klevernoteslib)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

install(TARGETS klevernotes ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
