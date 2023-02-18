// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

/*
 * LOOSELY BASED ON :
 * https://gist.github.com/anonymous/85b53283c66395f98302
 */
import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"

Column {
    id: infoRow

    width: parent.width
    height: childrenRect.height

    property alias displayedName: textDisplay.text

    // Not super clean but this make it easier to handle rename
    property string parentPath: {
        // The parentRow is possibly the subEntryColumn, so we need to acces is parent
        const parPath = parentRow.path

        // The path is undifined if the parentRow is the treeview
        return (parPath != undefined)
                    ? (useCase === "Note" && parentRow.useCase === "Category")
                        ? parPath+"/.BaseGroup"
                        : parPath
                    : Config.storagePath
    }
    property QtObject parentRow
    property string name
    property bool expanded
    property string useCase
    property int lvl

    readonly property string path: parentPath+"/"+name
    readonly property QtObject mouseArea: controlRoot
    readonly property QtObject textDisplay: textDisplay
    readonly property QtObject subEntryColumn: subEntryColumn

    DeleteConfirmationDialog {
        id: deleteConfirmationDialog

        useCase: tree.currentlySelected.useCase

        onAccepted: {
            const removed = StorageHandler.remove(tree.currentlySelected.path)
            if (removed) treeview.model = View.hierarchy(Config.storagePath,-1)
        }
        onRejected: console.log("Rejected")
    }

    Rectangle{
        id:visualRow

        width: infoRow.width
        height: textDisplay.height
        visible: true

        color: (tree.currentlySelected === infoRow)
                    ? Kirigami.Theme.highlightColor
                    : "transparent"

        Kirigami.Icon {
            id: carot

            x: 20 * infoRow.lvl
            width: Kirigami.Units.iconSizes.small
            height: textDisplay.height
            source: infoRow.expanded ? "go-down-symbolic" : "go-next-symbolic"
            isMask: true
            color: !(tree.currentlySelected === infoRow)
                    ?(controlRoot.hovered)
                        ? Kirigami.Theme.highlightColor
                        : Kirigami.Theme.textColor
                    : Kirigami.Theme.textColor

            Behavior on color {
                ColorAnimation {
                    duration: Kirigami.Units.shortDuration;
                    easing.type: Easing.InOutQuad
                }
            }
            visible: infoRow.useCase != "Note"
        }


        Text {
            id: textDisplay

            x: carot.x + 20
            height: 25
            width: infoRow.width-x
            font.pointSize: 12

            color: !(tree.currentlySelected === infoRow)
                    ?(controlRoot.hovered)
                        ? Kirigami.Theme.highlightColor
                        : Kirigami.Theme.textColor
                    : Kirigami.Theme.textColor

            elide: Text.ElideRight
        }

        MouseArea {
            id: controlRoot

            anchors.fill: parent

            hoverEnabled: true
            enabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool hovered : false

            onEntered: hovered = true
            onExited: hovered = false
            onClicked: {
                if (infoRow.useCase !== "Note") infoRow.expanded = !infoRow.expanded;
                tree.currentlySelected = infoRow
                visualRow.forceActiveFocus()

                if (mouse.button === Qt.RightButton &&
                    infoRow.path !== Config.storagePath+"/.BaseCategory") contextMenu.popup()
            }

            Controls.Menu {
                id: contextMenu

                Controls.MenuItem {
                    icon.name: "user-trash-symbolic"
                    text: i18n("Delete")

                    onTriggered: deleteConfirmationDialog.open()
                }
            }
        }
//         For futur keyboard nav
        // Keys.onPressed: console.log("hey")
    }


    SubEntryColumn {
        id: subEntryColumn

        visible: opacity > 0
        opacity: infoRow.expanded ? 1 : 0
        delimiter: 0
    }
}
