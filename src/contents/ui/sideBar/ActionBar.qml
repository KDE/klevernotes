// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import org.kde.kitemmodels
import org.kde.kirigami as Kirigami

import org.kde.Klever

import "qrc:/contents/ui/dialogs"

ToolBar {
    id: mainToolBar

    required property QtObject treeView
    required property bool sideBarWide
    readonly property QtObject renameAction: renameAction
    readonly property QtObject createNoteAction: createNoteAction
    readonly property QtObject createFolderAction: createFolderAction

    NamingDialog {
        id: namingDialog

        onRejected: {
            close()
            treeView.useCurrentItem()
        }
    }

    RowLayout {
        id: barLayout

        property bool searching: false

        anchors.fill: parent
        spacing: 0

        ToolButton {
            action: createFolderAction
            visible: !barLayout.searching && mainToolBar.sideBarWide
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered 
            ToolTip.text: i18nc("@tooltip", "Create a new folder") + " (" + createFolderAction.shortcut + ")"
        }

        ToolButton {
            action: createNoteAction
            visible: !barLayout.searching && mainToolBar.sideBarWide
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered 
            ToolTip.text: i18nc("@tooltip", "Create a new note") + " (" + createNoteAction.shortcut + ")"
        }

        ToolButton {
            action: renameAction
            visible: !barLayout.searching && mainToolBar.sideBarWide
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered
            ToolTip.text: i18n("Rename") + " (" + renameAction.shortcut + ")"
        }

        Item {
            Layout.fillWidth: true; 
            visible: !barLayout.searching
        }

        ToolButton {
            action: searchAction
            visible: !barLayout.searching && mainToolBar.sideBarWide
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered
            ToolTip.text: i18n("Search") + " (" + searchAction.shortcut + ")"
        }

        ToolSeparator {
            visible: drawer.isWide
            orientation: Qt.Vertical

            Layout.fillHeight: true
            Layout.margins: Kirigami.Units.smallSpacing
        }

        ToolButton {
            action: closeAction
            visible: !Kirigami.Settings.isMobile
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered
            ToolTip.text: (mainToolBar.sideBarWide 
                ? i18n("Collapse sidebar")
                : i18n("Expand sidebar")
            ) + " (" + closeAction.shortcut + ")"
            Layout.fillWidth: !mainToolBar.sideBarWide
        }

    }

    SearchBar {
        id: searchBar

        visible: barLayout.searching && mainToolBar.sideBarWide
        treeView: mainToolBar.treeView
        inSideBar: true

        width: drawer.isWide 
            ? drawer.largeWidth - Kirigami.Units.smallSpacing 
            : drawer.largeWidth * 2
        x: drawer.isWide 
            ? 0 
            : Math.round((applicationWindow().width - width + drawer.narrowWidth) / 2) // Center it in the main page
        y: drawer.isWide 
            ? 0 
            : Math.round((applicationWindow().height - height) / 4) // "Center" it vertically (not pure center but looks nice)

        popup.onClosed: {
            barLayout.searching = false
            text = ""
        }
        onClickedIndexChanged: if (clickedIndex) {
            askForFocus(clickedIndex)
            searchBar.popup.close()
            return;
        }
    }

    Kirigami.Action {
        id: createFolderAction

        shortcut: "Ctrl+Alt+C"
        icon.name: "folder-new-symbolic" 

        onTriggered: {
            mainToolBar.getName(false, makeFolder, true)
        }

        function makeFolder(name: string): void {
            makeRow(false, name)
        }
    }

    Kirigami.Action {
        id: createNoteAction

        property string name
        property bool isActive : false

        shortcut: "Ctrl+Alt+N"
        icon.name: "document-new-symbolic"

        onTriggered: {
            mainToolBar.getName(true, makeNote, true)
        }

        function makeNote(name: string): void {
            makeRow(true, name)
        }
    }

    Kirigami.Action{
        id: renameAction

        shortcut: "Ctrl+R"
        icon.name: "edit-rename-symbolic"

        onTriggered: {
            mainToolBar.getName(treeView.currentClickedItem.isNote, renameNote, false)
        }

        function renameNote(name: string): void {
            applicationWindow().saveState()
            treeView.model.rename(treeView.currentModelIndex, name)
            treeView.useCurrentItem()
        }
    }

    Kirigami.Action{
        id: searchAction

        shortcut: "Ctrl+F"
        icon.name: "search-symbolic"

        onTriggered: {
            barLayout.searching = true
            searchBar.forceActiveFocus()
        }
    }

   Kirigami.Action{
        id: closeAction

        shortcut: "Ctrl+Shift+1"
        icon.name: mainToolBar.sideBarWide 
            ? "sidebar-collapse-left-symbolic"
            : "sidebar-expand-symbolic"

        onTriggered: {
            drawer.close()
        }
    }

    function makeRow(isNote: bool, name: string): void {
        const parentModelIndex = treeView.currentClickedItem.isNote 
            ? treeView.model.parent(treeView.currentModelIndex)
            : mainToolBar.treeView.currentModelIndex

        const newModelIndex = treeView.model.addRow(name, isNote, parentModelIndex)
        askForFocus(newModelIndex)
    }

    function getName(isNote: bool, callBackFunc: var, newItem: bool): void {
        let defaultName
        if (newItem) {
            if (isNote) {
                defaultName = Config.defaultNoteName.length !== 0 ? Config.defaultNoteName : i18n("New Note")
            } else {
                defaultName = Config.defaultFolderName.length !== 0 ? Config.defaultFolderName : i18n("New Folder")
            }
        } else {
            defaultName = treeView.currentClickedItem.text
        }

        namingDialog.isNote = isNote
        namingDialog.shownName = defaultName
        namingDialog.textFieldText = defaultName
        namingDialog.parentPath = treeView.currentClickedItem.dir
        namingDialog.callBackFunc = callBackFunc
        namingDialog.newItem = newItem
        namingDialog.open()
    }
    
    function forceError(error: string): void {
        namingDialog.throwError(error) 
    }
    
    function askForFocus(itemModelIndex): void {
        applicationWindow().globalDrawer.askForFocus(itemModelIndex)
    }
}
