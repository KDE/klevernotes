// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0
/*
Kirigami.PromptDialog {
    id: textPromptDialog
    title: "Klever Notes Storage"

    subtitle:i18n("It looks like this is your first time using this app!\n\nPlease choose a location for your future KleverNotes storage or select an existing one.")

    property string folder
    property string userChoice
    property QtObject parentObject
    property QtObject drawer
    property bool initialSetup: true
    standardButtons: Kirigami.Dialog.NoButton

    function getFolder() {
        let component = Qt.createComponent("qrc:/contents/ui/dialogs/FolderChooserDialog.qml")

        if (component.status == Component.Ready) {
            var dialog = component.createObject(textPromptDialog);
            dialog.parent = textPromptDialog
            dialog.open()
        }
    }

    customFooterActions: [
        Kirigami.Action {
            text: i18n("Existing storage")
            onTriggered: {
                textPromptDialog.userChoice = "Existing storage chosen at "
                getFolder()
            }
        },
        Kirigami.Action {
            text: i18n("Create storage")
            onTriggered: {
                textPromptDialog.userChoice = "Storage created at "
                getFolder()
            }
        }
    ]

    onFolderChanged: {
        let folderPath = KleverUtility.getPath(textPromptDialog.folder)
        if (userChoice === "Storage created at "){
            folderPath = folderPath.concat("/.KleverNotes")
            StorageHandler.makeStorage(folderPath)
        }

        var pathEnd = folderPath.substring(folderPath.length,folderPath.length-13)

        if (pathEnd !== "/.KleverNotes"){
            textPromptDialog.close();
            const subtitle = i18n("It looks like the selected folder is not a KleverNotes storage.\n\nPlease choose a location for your future KleverNotes storage or select an existing one.")
            parentObject.handleStorage(subtitle)
            return
        }
        Config.storagePath = folderPath

        const fullNotification = textPromptDialog.userChoice+folderPath

        showPassiveNotification(fullNotification);
        textPromptDialog.close();

        // drawer.treeModel = View.hierarchy(Config.storagePath,-1)
    }

    onRejected:{
        if (initialSetup){
            const subtitle = i18n("This step is mandatory, please don't skip it.\n\nChoose a location to your future KleverNotes storage or select an existing one.")
            // parentObject.handleStorage(subtitle)
        }
    }
    onClosed: if (Config.storagePath !== "None") drawer.treeModel = View.hierarchy(Config.storagePath,-1)
}
*/

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.12 as Kirigami


Popup {
    id: setupPopup

    Kirigami.Theme.colorSet: Kirigami.Theme.Window

    modal: true
    focus: true
    width: parent.width - Kirigami.Units.largeSpacing * 10
    height: parent.height - Kirigami.Units.largeSpacing * 10
    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    closePolicy: Popup.NoAutoClose
    padding: Kirigami.Units.largeSpacing

    background: Kirigami.ShadowedRectangle {
        color: Kirigami.Theme.backgroundColor
        radius: Kirigami.Units.largeSpacing
        anchors.fill: parent

        shadow.size: Kirigami.Units.largeSpacing
        shadow.color: Qt.rgba(0.0, 0.0, 0.0, 0.15)
        shadow.yOffset: Kirigami.Units.devicePixelRatio * 2
    }
}
