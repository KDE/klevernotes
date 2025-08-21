# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>

add_library(klevernotes_static STATIC)

ecm_add_qml_module(klevernotes_static
    URI org.kde.klevernotes
    GENERATE_PLUGIN_SOURCE
    QML_FILES
        contents/ui/Main.qml

        contents/ui/dialogs/DeleteConfirmationDialog.qml
        contents/ui/dialogs/FilePickerDialog.qml
        contents/ui/dialogs/FileSaverDialog.qml
        contents/ui/dialogs/FolderPickerDialog.qml
        contents/ui/dialogs/FontPickerDialog.qml
        contents/ui/dialogs/ItemPropertiesDialog.qml
        contents/ui/dialogs/LeavePaintingDialog.qml
        contents/ui/dialogs/LinkDialog.qml
        contents/ui/dialogs/LinkNoteDialog.qml
        contents/ui/dialogs/MoveDialog.qml
        contents/ui/dialogs/NamingDialog.qml
        contents/ui/dialogs/NamingErrorDialog.qml
        contents/ui/dialogs/PrintingDialog.qml
        contents/ui/dialogs/StorageDialog.qml
        contents/ui/dialogs/StorageConversionDialog.qml
        contents/ui/dialogs/TextDialog.qml
        contents/ui/dialogs/URLDialog.qml

        contents/ui/dialogs/emojiDialog/EmojiDelegate.qml
        contents/ui/dialogs/emojiDialog/EmojiDialog.qml
        contents/ui/dialogs/emojiDialog/EmojiGrid.qml
        contents/ui/dialogs/emojiDialog/EmojiPicker.qml
        contents/ui/dialogs/emojiDialog/EmojiTonesPicker.qml

        contents/ui/dialogs/imagePickerDialog/ButtonDelegate.qml
        contents/ui/dialogs/imagePickerDialog/ImagePickerDialog.qml

        contents/ui/dialogs/tableMakerDialog/TableMakerDialog.qml
        contents/ui/dialogs/tableMakerDialog/UpDownButtonDelegate.qml

        contents/ui/dialogs/todoDialog/TextAreaDelegate.qml
        contents/ui/dialogs/todoDialog/ToDoDialog.qml

        contents/ui/pages/AboutPage.qml
        contents/ui/pages/MainPage.qml
        contents/ui/pages/PaintingPage.qml
        contents/ui/pages/PrintingPage.qml
        contents/ui/pages/SettingsPage.qml

        contents/ui/painting/ColorBar.qml
        contents/ui/painting/ColorButton.qml
        contents/ui/painting/DrawingToolBar.qml
        contents/ui/painting/DrawingToolBarOptions.qml

        contents/ui/settings/components/ClickableArrow.qml
        contents/ui/settings/components/ClickableIcon.qml
        contents/ui/settings/components/ActionListViewDelegate.qml
        contents/ui/settings/DisplayPreview.qml
        contents/ui/settings/EditorHighlightPreview.qml
        contents/ui/settings/EditorTab.qml
        contents/ui/settings/FontPicker.qml
        contents/ui/settings/GeneralTab.qml
        contents/ui/settings/NotePreviewTab.qml
        contents/ui/settings/PluginsTab.qml
        contents/ui/settings/SettingsColorButton.qml
        contents/ui/settings/TabBar.qml

        contents/ui/sharedComponents/ActionsList.qml
        contents/ui/sharedComponents/ExpandingFormSwitch.qml
        contents/ui/sharedComponents/KAction.qml
        contents/ui/sharedComponents/ReversibleButtonDelegate.qml

        contents/ui/sideBar/ActionBar.qml
        contents/ui/sideBar/ContextMenu.qml
        contents/ui/sideBar/SearchBar.qml
        contents/ui/sideBar/Sidebar.qml
        contents/ui/sideBar/TreeItem.qml
        contents/ui/sideBar/TreeView.qml

        contents/ui/textEditor/components/CheatSheetEntry.qml
        contents/ui/textEditor/components/NotesMapEntry.qml
        contents/ui/textEditor/BottomToolBar.qml
        contents/ui/textEditor/CheatSheet.qml
        contents/ui/textEditor/EditorView.qml
        contents/ui/textEditor/NotesMap.qml
        contents/ui/textEditor/TextDisplay.qml
        contents/ui/textEditor/TextEditor.qml
        contents/ui/textEditor/TextToolBar.qml

        contents/ui/todoEditor/TodoDelegate.qml
        contents/ui/todoEditor/ToDoView.qml

    SOURCES
        # General utility
        logic/colorschemer.cpp
        logic/colorschemer.h
        logic/documentHandler.cpp
        logic/documentHandler.h
        logic/qmlLinker.cpp
        logic/qmlLinker.h
        logic/kleverUtility.cpp
        logic/kleverUtility.h

        # Printing
        logic/printing/printingHelper.cpp

        # Treeview
        logic/treeview/treeModel.cpp
        logic/treeview/treeModel.h

        # Preview
        logic/preview/styleHandler.cpp
        logic/preview/styleHandler.hpp

        # Editor
        logic/editor/editorHandler.cpp
        logic/editor/editorHandler.hpp

        # Plugins
        logic/parser/plugins/emoji/emojiModel.cpp
        logic/parser/plugins/emoji/emojiModel.h
        logic/parser/plugins/emoji/emoticonFilterModel.cpp
        logic/parser/plugins/emoji/emoticonFilterModel.h

        logic/parser/plugins/noteMapper/noteMapper.cpp
        logic/parser/plugins/noteMapper/noteMapper.h

        logic/parser/plugins/syntaxHighlight/highlightHelper.cpp
)

target_include_directories(klevernotes_static PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/logic

    ${CMAKE_CURRENT_SOURCE_DIR}/logic/editor

    ${CMAKE_CURRENT_SOURCE_DIR}/logic/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/parser/plugins/emoji
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/parser/plugins/noteMapper
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/parser/plugins/syntaxHighlight

    ${CMAKE_CURRENT_SOURCE_DIR}/logic/preview
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/printing
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/treeview
)

target_sources(klevernotes_static
    PRIVATE
        main.cpp
        app.cpp

        # Editor
        logic/editor/colors.cpp
        logic/editor/editorHighlighter.cpp
        logic/editor/editorHighlighterPrivate.cpp
        logic/editor/posCacheUtils.cpp
        logic/editor/editorTextManipulation.cpp


        # Treeview
        logic/treeview/treeItem.cpp
        logic/treeview/oldModelConverter.cpp
        logic/treeview/fileSystemHelper.cpp

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

        logic/parser/plugins/emoji/emojiTones.cpp
        logic/parser/plugins/emoji/emojiPlugin.cpp

        logic/parser/plugins/noteMapper/noteMapperParserUtils.cpp
        logic/parser/plugins/noteMapper/noteMapperUtils.cpp
        logic/parser/plugins/noteMapper/noteLinkingPlugin.cpp

        logic/parser/plugins/puml/pumlHelper.cpp
        logic/parser/plugins/puml/pumlParserUtils.cpp

        logic/parser/plugins/syntaxHighlight/highlightParserUtils.cpp
)


kconfig_target_kcfg_file(klevernotes_static FILE kleverconfig.kcfg CLASS_NAME KleverConfig MUTATORS GENERATE_PROPERTIES GENERATE_MOC DEFAULT_VALUE_GETTERS PARENT_IN_CONSTRUCTOR QML_REGISTRATION SINGLETON)

target_link_libraries(klevernotes_static PUBLIC
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
    KF6::ColorScheme
    KF6::KIOCore
    KF6::IconThemes
)

add_executable(klevernotes
    main.cpp
    resources.qrc
)

target_link_libraries(klevernotes PRIVATE klevernotes_static klevernotes_staticplugin)

if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

install(TARGETS klevernotes ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
