# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022-2024 Louis Schul <schul9louis@gmail.com>

add_library(klevernoteslib STATIC
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
    logic/editor/editorHighlighter.cpp
    logic/editor/editorHighlighterPrivate.cpp
    logic/editor/posCacheUtils.cpp

    # Painting
    logic/painting/pointMaker.cpp
    logic/painting/sketchserializer.cpp
    logic/painting/sketchview.cpp
    logic/painting/strokeitem.cpp
    logic/painting/strokelistitem.cpp
    logic/painting/strokepainter.cpp
    logic/painting/pressureequation.cpp
    logic/painting/sketchmodel.cpp
    logic/painting/stroke.cpp

    # Printing
    logic/printing/printingHelper.cpp

    # Preview
    logic/preview/styleHandler.cpp

    # Treeview
    logic/treeview/noteTreeModel.cpp

    # === PARSER ===
    logic/parser/parser.cpp
    logic/parser/renderer.cpp
    logic/parser/md4qtDataCleaner.cpp
    logic/parser/md4qtDataGetter.cpp
    logic/parser/md4qtDataManip.cpp
    # Extended Syntax
    logic/parser/extendedSyntax/extendedSyntaxMaker.cpp

    # plugins
    logic/parser/plugins/cliHelper.cpp
    logic/parser/plugins/pluginHelper.cpp

    logic/parser/plugins/emoji/emoticonFilterModel.cpp
    logic/parser/plugins/emoji/emojiModel.cpp
    logic/parser/plugins/emoji/emojiTones.cpp
    logic/parser/plugins/emoji/emojiPlugin.cpp

    logic/parser/plugins/noteMapper/noteMapper.cpp
    logic/parser/plugins/noteMapper/noteMapperParserUtils.cpp
    logic/parser/plugins/noteMapper/noteMapperUtils.cpp
    logic/parser/plugins/noteMapper/noteLinkingPlugin.cpp

    logic/parser/plugins/puml/pumlHelper.cpp
    logic/parser/plugins/puml/pumlParserUtils.cpp

    logic/parser/plugins/syntaxHighlight/highlightHelper.cpp
    logic/parser/plugins/syntaxHighlight/highlightParserUtils.cpp
)

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
