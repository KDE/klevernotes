// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

Controls.Popup {
    id: setupPopup

    modal: true
    focus: true
    width: Kirigami.Units.largeSpacing * 40

    parent: Controls.Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    closePolicy: Controls.Popup.NoAutoClose
    padding: 0

    background: Kirigami.ShadowedRectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor
        radius: Kirigami.Units.smallSpacing
        anchors.fill: parent

        shadow.size: Kirigami.Units.largeSpacing
        shadow.color: Qt.rgba(0.0, 0.0, 0.0, 0.15)
        shadow.yOffset: Kirigami.Units.devicePixelRatio * 2
    }

    property string subtitle: i18n("It looks like this is your first time using this app!\n\nPlease choose a location for your future KleverNotes storage or select an existing one.\n")
    property bool firstSetup: true
    property string folder
    property string userChoice

    GridLayout {
        anchors.fill: parent
        columns: 2
        rows: 3
        rowSpacing: Kirigami.Units.largeSpacing * 2

        Text {
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: Kirigami.Theme.textColor
            text: subtitle
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap

            Layout.row: 0
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing
        }

        Controls.Button{
            Layout.row: 1
            Layout.column: 0
            icon.name: "folder-sync"
            text: i18n("Existing storage")
            Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            Controls.ToolTip.visible: hovered
            Controls.ToolTip.text: i18n("Change the storage path")
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                setupPopup.userChoice = "Existing storage chosen at "
                getFolder()
            }
        }
        Controls.Button{
            Layout.row: 1
            Layout.column: 1
            icon.name: "folder-new"
            text: i18n("Create storage")
            Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            Controls.ToolTip.visible: hovered
            Controls.ToolTip.text: i18n("Create a new storage")
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                setupPopup.userChoice = "Storage created at "
                getFolder()
            }
        }

        Rectangle {
            id: bottomRowsquare
            color: bottomRowRounded.color

            Layout.columnSpan: 2
            Layout.row: 2
            Layout.fillWidth: true
            Layout.preferredHeight: bottomRowRounded.height-bottomRowRounded.radius
            Layout.alignment: Qt.AlignTop
        }

        Rectangle {
            id: bottomRowRounded
            color: Kirigami.Theme.backgroundColor
            radius: setupPopup.background.radius
            visible: !firstSetup

            Layout.columnSpan: 2
            Layout.row: 2
            Layout.fillWidth: true
            Layout.preferredHeight: close.height + Kirigami.Units.smallSpacing * 2

            RowLayout {
                anchors.fill: parent
                anchors.margins: Kirigami.Units.smallSpacing

                Controls.Button {
                    id: close
                    text: i18n("Cancel")
                    icon.name: "dialog-cancel"

                    Layout.alignment: Qt.AlignRight

                    onClicked: setupPopup.close()
                }
            }
        }
    }

    function getFolder() {
        let component = Qt.createComponent("qrc:/contents/ui/dialogs/FolderChooserDialog.qml")

        if (component.status == Component.Ready) {
            var dialog = component.createObject(setupPopup);
            dialog.parent = setupPopup
            dialog.open()
        }
    }

    onFolderChanged: {
        let folderPath = KleverUtility.getPath(setupPopup.folder)
        if (userChoice === "Storage created at "){
            folderPath = folderPath.concat("/.KleverNotes")
            StorageHandler.makeStorage(folderPath)
        }

        var pathEnd = folderPath.substring(folderPath.length,folderPath.length-13)

        if (pathEnd !== "/.KleverNotes"){
            subtitle = i18n("It looks like the selected folder is not a KleverNotes storage.\n\nPlease choose a location for your future KleverNotes storage or select an existing one.")
            return
        }
        Config.storagePath = folderPath

        const fullNotification = setupPopup.userChoice+folderPath

        showPassiveNotification(fullNotification);
        setupPopup.close();
    }
}
