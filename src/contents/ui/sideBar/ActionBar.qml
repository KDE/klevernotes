// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs @QTQUICKDIALOG_VERSION@

import org.kde.kitemmodels 1.0
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"

ToolBar {
    id: mainToolBar

    required property TreeView treeView
    readonly property QtObject renameAction: renameAction
    readonly property QtObject createNoteAction: createNoteAction
    readonly property QtObject createGroupAction: createGroupAction
    readonly property QtObject createCategoryAction: createCategoryAction

    property var currentModelIndex

    NamingDialog {
        id: namingDialog
    }

    RowLayout {
        id: barLayout

        property bool searching: false

        anchors.fill: parent
        spacing: 0

        ToolButton {
            action: createCategoryAction
            visible: !barLayout.searching
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered
            ToolTip.text: i18nc("as in 'A note category'", "Create a new category") + " (" + createCategoryAction.shortcut + ")"
        }

        ToolButton {
            action: createGroupAction
            visible: !barLayout.searching
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered
            ToolTip.text: i18nc("as in 'A note group'", "Create a new group") + " (" + createGroupAction.shortcut + ")"
        }

        ToolButton {
            action: createNoteAction
            visible: !barLayout.searching
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered
            ToolTip.text: i18nc("as in 'A note'", "Create a new note") + " (" + createNoteAction.shortcut + ")"
        }

        ToolButton {
            action: renameAction
            visible: !barLayout.searching
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered
            ToolTip.text: i18n("Rename") + " (" + renameAction.shortcut + ")"
        }

        Item {Layout.fillWidth: true; visible: !barLayout.searching}

        ToolButton {
            visible: !barLayout.searching
            icon.name: "search-symbolic"
            onClicked: {
                barLayout.searching = true
                searchBar.contentItem.forceActiveFocus()
            }
        }

        SearchBar {
            id: searchBar

            visible: barLayout.searching
            listModel: treeview.model

            Layout.fillWidth: true

            popup.onClosed: {
                barLayout.searching = false
                text = ""
            }
            onClickedIndexChanged: if (clickedIndex) {
                applicationWindow().globalDrawer.askForFocus(clickedIndex)
                searchBar.popup.close()
                return;
            }
        }
    }

    Kirigami.Action {
        id: createCategoryAction

        property string name
        property bool isActive : false

        shortcut: "Ctrl+Alt+C"
        icon.name: "journal-new"

        onNameChanged: if (isActive) {
            treeView.model.addRow(name, Config.storagePath, 1)
            isActive = false
            name = ""
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
        icon.name: "folder-new"

        onNameChanged: if (isActive) {
            treeView.model.addRow(name, categoryPath, 2, parentModelIndex)
            isActive = false
            name = ""
        }
        onTriggered: {
            isActive = true

            switch(treeView.currentItem.useCase) {
                case "Category":
                    categoryPath = treeView.currentItem.path
                    parentModelIndex = currentModelIndex
                    break;
                case "Group":
                    categoryPath = KleverUtility.getParentPath(treeView.currentItem.path)

                    parentModelIndex = treeView.model.parent(currentModelIndex)
                    break;
                case "Note":
                    const groupPath = KleverUtility.getParentPath(treeView.currentItem.path)
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
        icon.name: "document-new"

        onNameChanged: if (isActive) {
            treeView.model.addRow(name, groupPath, 3, parentModelIndex)
            isActive = false
            name = ""
        }
        onTriggered: {
            isActive = true

            switch(treeView.currentItem.useCase) {
                case "Category":
                    groupPath = treeView.currentItem.path+"/.BaseGroup"
                    parentModelIndex = currentModelIndex
                    break;
                case "Group":
                    groupPath = treeView.currentItem.path
                    parentModelIndex = currentModelIndex
                    break;
                case "Note":
                    groupPath = KleverUtility.getParentPath(treeView.currentItem.path)
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
        property string name: treeView.currentItem ? treeView.currentItem.text : ""

        shortcut: "Ctrl+R"
        icon.name: "edit-rename"

        onNameChanged: if (isActive) {
            treeView.model.rename(currentModelIndex, name)
            isActive = false
        }
        onTriggered: {
            isActive = true

            const useCase = treeView.currentItem.useCase
            const parentPath = KleverUtility.getParentPath(treeView.currentItem.path)
            mainToolBar.getName(useCase, name, parentPath, renameAction, false)
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
}
