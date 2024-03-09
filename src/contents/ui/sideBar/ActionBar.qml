// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs

import org.kde.kitemmodels 1.0
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"

ToolBar {
    id: mainToolBar

    required property TreeView treeView
    required property bool sideBarWide
    readonly property QtObject renameAction: renameAction
    readonly property QtObject createNoteAction: createNoteAction
    readonly property QtObject createGroupAction: createGroupAction
    readonly property QtObject createCategoryAction: createCategoryAction

    property var currentModelIndex
    property var currentClickedItem

    NamingDialog {
        id: namingDialog

        onRejected: {
            useCurrentItem()
        }
    }

    RowLayout {
        id: barLayout

        property bool searching: false

        anchors.fill: parent
        spacing: 0

        ToolButton {
            action: createCategoryAction
            visible: !barLayout.searching && mainToolBar.sideBarWide
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered 
            ToolTip.text: i18nc("as in 'A note category'", "Create a new category") + " (" + createCategoryAction.shortcut + ")"
        }

        ToolButton {
            action: createGroupAction
            visible: !barLayout.searching && mainToolBar.sideBarWide
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered 
            ToolTip.text: i18nc("as in 'A note group'", "Create a new group") + " (" + createGroupAction.shortcut + ")"
        }

        ToolButton {
            action: createNoteAction
            visible: !barLayout.searching && mainToolBar.sideBarWide
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered 
            ToolTip.text: i18nc("as in 'A note'", "Create a new note") + " (" + createNoteAction.shortcut + ")"
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
                : i18n("Expend sidebar")
            ) + " (" + closeAction.shortcut + ")"
            Layout.fillWidth: !mainToolBar.sideBarWide
        }

    }

    SearchBar {
        id: searchBar

        visible: barLayout.searching && mainToolBar.sideBarWide
        listModel: treeView.model
        inSideBar: true
        currentUseCase: "note"

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
        id: createCategoryAction

        property string name
        property bool isActive : false

        shortcut: "Ctrl+Alt+C"
        icon.name: "journal-new-symbolic"

        onNameChanged: if (isActive) {
            treeView.model.addRow(name, Config.storagePath, 1)
            isActive = false
            name = ""
            useCurrentItem()
        }
        onTriggered: {
            isActive = true
            const objectName = Config.defaultCategoryName
            mainToolBar.getName("Category", objectName, Config.storagePath, createCategoryAction, true)
        }
    }

    Kirigami.Action {
        id: createGroupAction

        property string name
        property string categoryPath
        property var parentModelIndex
        property bool isActive : false

        shortcut: "Ctrl+Alt+G"
        icon.name: "folder-new-symbolic"

        onNameChanged: if (isActive) {
            treeView.model.addRow(name, categoryPath, 2, parentModelIndex)
            isActive = false
            name = ""
            useCurrentItem()
        }
        onTriggered: {
            isActive = true

            switch(mainToolBar.currentClickedItem.useCase) {
                case "Category":
                    categoryPath = mainToolBar.currentClickedItem.path
                    parentModelIndex = currentModelIndex
                    break;
                case "Group":
                    categoryPath = KleverUtility.getParentPath(mainToolBar.currentClickedItem.path)

                    parentModelIndex = treeView.model.parent(currentModelIndex)
                    break;
                case "Note":
                    const groupPath = KleverUtility.getParentPath(mainToolBar.currentClickedItem.path)
                    categoryPath = KleverUtility.getParentPath(groupPath)

                    if (groupPath.endsWith(".BaseGroup")) {
                        parentModelIndex = treeView.model.parent(currentModelIndex)
                    } else {
                        const groupModelIndex = treeView.model.parent(currentModelIndex)
                        parentModelIndex = treeView.model.parent(groupModelIndex)
                    }
                    break;
            }
            const objectName = Config.defaultGroupName
            mainToolBar.getName("Group", objectName, categoryPath, createGroupAction, true)
        }
    }

    Kirigami.Action {
        id: createNoteAction

        property string name
        property string groupPath
        property var parentModelIndex
        property bool isActive : false

        shortcut: "Ctrl+Alt+N"
        icon.name: "document-new-symbolic"

        onNameChanged: if (isActive) {
            const newModelIndex = treeView.model.addRow(name, groupPath, 3, parentModelIndex)
            isActive = false
            name = ""
            askForFocus(newModelIndex)
        }
        onTriggered: {
            isActive = true

            switch(mainToolBar.currentClickedItem.useCase) {
                case "Category":
                    groupPath = mainToolBar.currentClickedItem.path+"/.BaseGroup"
                    parentModelIndex = currentModelIndex
                    break;
                case "Group":
                    groupPath = mainToolBar.currentClickedItem.path
                    parentModelIndex = currentModelIndex
                    break;
                case "Note":
                    groupPath = KleverUtility.getParentPath(mainToolBar.currentClickedItem.path)
                    parentModelIndex = treeView.model.parent(currentModelIndex)
                    break;
            }
            const shownName = Config.defaultNoteName

            mainToolBar.getName("Note", shownName, groupPath, createNoteAction, true)
        }
    }

    Kirigami.Action{
        id: renameAction

        property bool isActive: false
        property string name: mainToolBar.currentClickedItem ? mainToolBar.currentClickedItem.text : ""

        shortcut: "Ctrl+R"
        icon.name: "edit-rename-symbolic"

        onNameChanged: if (isActive) {
            treeView.model.rename(currentModelIndex, name)
            isActive = false
            useCurrentItem()
        }
        onTriggered: {
            isActive = true

            const useCase = mainToolBar.currentClickedItem.useCase
            const parentPath = KleverUtility.getParentPath(mainToolBar.currentClickedItem.path)
            mainToolBar.getName(useCase, name, parentPath, renameAction, false)
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

    function getName(useCase, shownName, parentPath, callingAction, newItem){
        namingDialog.useCase = useCase
        namingDialog.shownName = shownName
        namingDialog.textFieldText = shownName
        namingDialog.parentPath = parentPath
        namingDialog.callingAction = callingAction
        namingDialog.newItem = newItem
        namingDialog.open()
    }

    function forceError(error) {
        namingDialog.throwError(error) 
    }

    function setClickedItemInfo(clickedItem, clickedModelIndex) {
        currentClickedItem = clickedItem
        currentModelIndex = clickedModelIndex
    }

    function useCurrentItem() {
        const currentModelIndex = treeView.getModelIndex(treeView.currentIndex)
        setClickedItemInfo(treeView.currentItem, currentModelIndex)
    }

    function askForFocus(itemModelIndex) {
        applicationWindow().globalDrawer.askForFocus(itemModelIndex)
    }
}
