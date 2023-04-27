// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.1
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"

ToolBar {
    id: mainToolBar

    property QtObject treeView: undefined
    readonly property var treeModel: treeView.descendantsModel
    readonly property QtObject renameAction: renameAction
    readonly property QtObject createNoteAction: createNoteAction
    readonly property QtObject createGroupAction: createGroupAction
    readonly property QtObject createCategoryAction: createCategoryAction
    readonly property var currentModelIndex: treeModel.mapToSource(treeModel.index(treeView.currentIndex, 0))

    function getName(useCase, shownName, parentPath, callingAction, newItem){
        namingDialog.useCase = useCase
        namingDialog.shownName = shownName
        namingDialog.textFieldText = shownName
        namingDialog.parentPath = parentPath
        namingDialog.callingAction = callingAction
        namingDialog.newItem = newItem
        namingDialog.open()
    }

    function makeRow(creatingPath, name, rowLevel, parentModelIndex) {
        let isCreated = false
        switch(rowLevel) {
            case 0:
                isCreated = StorageHandler.makeCategory(creatingPath, name)
                break
            case 1:
                isCreated = StorageHandler.makeGroup(creatingPath, name)
                break
            case 2:
                isCreated = StorageHandler.makeNote(creatingPath, name)
                break
        }
        if (!isCreated) return

        const rowPath = creatingPath+"/"+name

        if (parentModelIndex) {
            treeView.model.addRow(rowPath, rowLevel, parentModelIndex)}
        else {treeView.model.addRow(rowPath, rowLevel)}
    }

    NamingDialog {
        id: namingDialog
    }

    Kirigami.Action {
        id: createCategoryAction

        icon.name: "journal-new"

        property bool isActive : false
        property string name

        onNameChanged: {
            if (isActive) {
                const creatingPath = Config.storagePath
                const rowLevel = 0
                const parentLevel = -1
                makeRow(creatingPath, name, rowLevel)
                isActive = false
                name = ""
            }
        }

        onTriggered: {
            isActive = true
            const objectName = Config.defaultCategoryName
            mainToolBar.getName("Category", objectName, Config.storagePath, createCategoryAction, true)
        }
    }

    Kirigami.Action {
        id: createGroupAction

        icon.name: "folder-new"

        property bool isActive : false
        property string categoryPath
        property string name
        property var parentModelIndex

        onNameChanged: {
            if (isActive) {
                makeRow(categoryPath, name, 1, parentModelIndex)
                isActive = false
                name = ""
            }
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

                    parentModelIndex = treeView.model.getParentIndex(currentModelIndex)
                    break;
                case "Note":
                    const groupPath = KleverUtility.getParentPath(treeView.currentItem.path)
                    categoryPath = KleverUtility.getParentPath(groupPath)

                    if (groupPath.endsWith(".BaseGroup")) {
                        parentModelIndex = treeView.model.getParentIndex(currentModelIndex)
                    } else {
                        const groupModelIndex = treeView.model.getParentIndex(currentModelIndex)
                        parentModelIndex = treeView.model.getParentIndex(groupModelIndex)
                    }
                    break;
            }
            const objectName = Config.defaultGroupName
            mainToolBar.getName("Group", objectName, categoryPath, createGroupAction, true)
        }
    }

    Kirigami.Action {
        id: createNoteAction

        icon.name: "document-new"

        property bool isActive : false
        property string groupPath
        property string name
        property var parentModelIndex

        onNameChanged: {
            if (isActive) {
                const rowLevel = 2
                makeRow(groupPath, name, rowLevel, parentModelIndex)
                isActive = false
                name = ""
            }
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
                    parentModelIndex = treeView.model.getParentIndex(currentModelIndex)
                    break;
            }
            const shownName = Config.defaultNoteName

            mainToolBar.getName("Note", shownName, groupPath, createNoteAction, true)
        }
    }

    Kirigami.Action{
        id: renameAction

        icon.name: "edit-rename"

        property bool isActive : false
        property string name : treeView.currentItem.label


        onNameChanged: {
            if (isActive) {
                if (treeView.currentItem.path.endsWith(".BaseCategory")) {
                    Config.categoryDisplayName = name
                } else {
                    const oldPath = treeView.currentItem.path
                    const newPath = KleverUtility.getParentPath(treeView.currentItem.path)+"/"+name
                    StorageHandler.rename(oldPath, newPath)
                }

                const currentModelIndex = treeModel.mapToSource(treeModel.index(treeView.currentIndex, 0))
                treeView.model.renameRow(currentModelIndex, name)
                isActive = false
            }
        }

        onTriggered: {
            isActive = true

            const useCase = treeView.currentItem.useCase
            const parentPath = KleverUtility.getParentPath(treeView.currentItem.path)
            mainToolBar.getName(useCase, name, parentPath, renameAction, false)
        }
    }


    RowLayout {
        anchors.fill: parent
        spacing: 0

        ToolButton {
            action: createCategoryAction
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered
            ToolTip.text: i18n("Create a new category")
        }

        ToolButton {
            action: createGroupAction
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered
            ToolTip.text: i18n("Create a new group")
        }

        ToolButton {
            action: createNoteAction
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered
            ToolTip.text: i18n("Create a new note")
        }

        ToolButton {
            action: renameAction
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: hovered
            ToolTip.text: i18n("Rename")
        }

        Item {Layout.fillWidth: true}
    }
}
