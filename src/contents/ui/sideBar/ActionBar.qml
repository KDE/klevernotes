// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.1
import org.kde.kirigami 2.19 as Kirigami
import org.kde.Klever 1.0

ToolBar {
    id: mainToolBar

    property QtObject treeView;


    Kirigami.Action {
        id: createCategoryAction
        icon.name: "journal-new"
        // shortcut: "ctrl+l"
        onTriggered: {
            // console.log()
            // treeView.currentlySelected.useCase
            console.log("Create Cat",Config.path)}
    }
    Kirigami.Action {
        id: createGroupAction
        icon.name: "folder-new"
        onTriggered: {
            let categoryPath;

            const useCase = treeView.currentlySelected.useCase
            switch(useCase) {
                case "Category":
                    categoryPath = treeView.currentlySelected.path
                    break;
                case "Group":
                    categoryPath = treeView.currentlySelected.parentRow.path
                    break;
                case "Note":
                    const parentRow = treeView.currentlySelected.parentRow
                    // A note can be inside a group or a category
                    categoryPath = (parentRow.useCase == "Group") ? parentRow.parentRow.path : parentRow.path
                    break;
            }
            console.log("create group",categoryPath)
        }
    }

    Kirigami.Action {
        id: createNoteAction
        icon.name: "document-new"
        onTriggered: {
            let groupPath;

            const useCase = treeView.currentlySelected.useCase
            switch(useCase) {
                case "Category":
                    groupPath = treeView.currentlySelected.path+"/.BaseGroup"
                    break;
                case "Group":
                    groupPath = treeView.currentlySelected.path
                    break;
                case "Note":
                    const parentRow = treeView.currentlySelected.parentRow
                    // A note can be inside a group or a category
                    groupPath = (parentRow.useCase == "Group") ? parentRow.path : parentRow.path+"/.BaseGroup"
                    break;
            }
            console.log("create note",groupPath)
        }
    }

    Kirigami.Action{
        id: rename
        icon.name: "edit-rename"
        onTriggered: {
            const mouseArea= treeView.currentlySelected.mouseArea
            mouseArea.enabled = !mouseArea.enabled

            const textDisplay = treeView.currentlySelected.textDisplay

            textDisplay.readOnly = !textDisplay.readOnly
            console.log(mouseArea.enabled,textDisplay.readOnly)
            // console.log("rename",treeView.currentlySelected.name)
            // treeView.currentlySelected.name = "GOTRENAMED"
            // console.log("rename",treeView.currentlySelected.name)
        }
    }


    RowLayout {
        anchors.fill: parent
        spacing: 0


        ToolButton {
            action: createCategoryAction
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: i18n("Create a new category")
        }
        ToolButton {
            action: createGroupAction
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: i18n("Create a new group")
        }
        ToolButton {
            action: createNoteAction
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: i18n("Create a new note")
        }
        ToolButton {
            action: rename
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: i18n("Rename")
        }

        Item { Layout.fillWidth: true
        }
    }
}
