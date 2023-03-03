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
    readonly property QtObject infoRow: treeView.currentlySelected

    function getName(useCase,shownName,realName,parentPath,callingAction,newItem){
        namingDialog.useCase = useCase
        namingDialog.shownName = shownName
        namingDialog.realName = realName
        namingDialog.parentPath = parentPath
        namingDialog.callingAction = callingAction
        namingDialog.newItem = newItem
        namingDialog.open()
        namingDialog.nameField.selectAll()
        namingDialog.nameField.forceActiveFocus()
    }

    function makeRow(subEntryColumn,useCase,creatingPath,name,forcedLvl){
        const newPath = creatingPath+"/"+name
        let lvl
        switch(useCase) {
            case "Category":
                StorageHandler.makeCategory(creatingPath,name)
                lvl = 0
                break
            case "Group":
                StorageHandler.makeGroup(creatingPath,name)
                lvl = 1
                break
            case "Note":
                StorageHandler.makeNote(creatingPath,name)
                lvl = 2
                break
        }
        const caller = subEntryColumn

        forcedLvl = (forcedLvl) ? forcedLvl : Infinity
        const info = [forcedLvl, true]

        treeView.hierarchyAsker.push([caller,info])
        View.hierarchySupplier(newPath,lvl)
    }

    NamingDialog {
        id: namingDialog

        sideBarAction: true
    }

    Kirigami.Action {
        id: createCategoryAction

        icon.name: "journal-new"

        property bool isActive : false
        property string name

        onNameChanged: {
            if (isActive) {
                makeRow(treeView.subEntryColumn,"Category",Config.storagePath,name)
                isActive = false
                name = ""
            }
        }

        onTriggered: {
            isActive = true
            const objectName = Config.defaultCategoryName
            mainToolBar.getName("Category",objectName,objectName,Config.storagePath,createCategoryAction,true)
        }
    }
    Kirigami.Action {
        id: createGroupAction

        icon.name: "folder-new"

        property bool isActive : false
        property string categoryPath
        property QtObject subEntryColumn
        property string name

        onNameChanged: {
            if (isActive) {
                makeRow(subEntryColumn,"Group",categoryPath,name)
                isActive = false
                name = ""
            }
        }

        onTriggered: {
            isActive = true
            const parentRow = infoRow.parentRow

            switch(infoRow.useCase) {
                case "Category":
                    categoryPath = infoRow.path
                    subEntryColumn = infoRow.subEntryColumn
                    break;
                case "Group":
                    categoryPath = infoRow.parentPath
                    subEntryColumn = parentRow.subEntryColumn
                    break;
                case "Note":
                    // A note can be inside a group or a category
                    if (parentRow.useCase == "Group"){
                        categoryPath = parentRow.parentPath
                        subEntryColumn = parentRow.parentRow.subEntryColumn
                    }
                    else {
                        categoryPath = parentRow.path
                        subEntryColumn = parentRow.subEntryColumn
                    }
                    break;
            }

            const objectName = Config.defaultGroupName
            mainToolBar.getName("Group",objectName,objectName,categoryPath,createGroupAction,true)
        }
    }

    Kirigami.Action {
        id: createNoteAction

        icon.name: "document-new"

        property bool isActive : false
        property string groupPath
        property QtObject subEntryColumn
        property string name
        property int forcedLvl

        onNameChanged: {
            if (isActive) {
                makeRow(subEntryColumn,"Note",groupPath,name,forcedLvl)
                isActive = false
                name = ""
                forcedLvl = Infinity
            }
        }

        onTriggered: {
            isActive = true
            switch(infoRow.useCase) {
                case "Category":
                    groupPath = infoRow.path+"/.BaseGroup"
                    subEntryColumn = infoRow.subEntryColumn
                    forcedLvl = 1
                    break;
                case "Group":
                    groupPath = infoRow.path
                    subEntryColumn = infoRow.subEntryColumn
                    break;
                case "Note":
                    groupPath = infoRow.parentPath
                    if (groupPath.endsWith("/.BaseGroup")) forcedLvl = 1
                    subEntryColumn = infoRow.parentRow.subEntryColumn
                    break;
            }

            const objectName = Config.defaultNoteName
            mainToolBar.getName("Note",objectName,objectName,groupPath,createNoteAction,true)
        }
    }

    Kirigami.Action{
        id: renameAction

        icon.name: "edit-rename"

        property bool isActive : false
        property string name : infoRow.textDisplay.text

        onNameChanged: {
            if (isActive) {
                infoRow.displayedName = name
                if (infoRow.name == ".BaseCategory") {
                    Config.categoryDisplayName = name
                }
                else {
                    const oldPath = infoRow.path
                    const newPath = infoRow.parentPath+"/"+name
                    StorageHandler.rename(oldPath,newPath)
                }
                isActive = false
            }
        }

        onTriggered: {
            isActive = true
            mainToolBar.getName(infoRow.useCase,name,infoRow.name,infoRow.parentPath,renameAction,false)
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

        Item { Layout.fillWidth: true}
    }
}
