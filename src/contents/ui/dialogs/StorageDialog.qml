import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami

// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import org.kde.Klever 1.0

Kirigami.PromptDialog {
    id: textPromptDialog
    title: "Klever Notes Storage"

    subtitle:i18n("It looks like this is your first time using this app!\n\nPlease choose a location for your future Klever Note storage or select an existing one.")

    property string folder
    property string userChoice
    standardButtons: Kirigami.Dialog.None

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
        var folderPath = textPromptDialog.folder
        if (userChoice === "Storage created at "){
            folderPath = textPromptDialog.folder.concat("/.KleverNotes")
        }

        var pathEnd = folderPath.substring(folderPath.length,folderPath.length-13)

        if (pathEnd !== "/.KleverNotes"){
            textPromptDialog.close();
            var subtitle = i18n("It looks like the selected folder is not a Klever Notes storage.\n\nPlease choose a location for your future Klever Note storage or select an existing one.")
            root.checkForStorage(subtitle)
        }
        else{
            StorageHandler.makeStorage(folderPath)
            Config.path = folderPath

            var fullNotification = textPromptDialog.userChoice+KleverUtility.getPath(folderPath)

            showPassiveNotification(fullNotification);
            textPromptDialog.close();
        }
    }

    onRejected:{
        var subtitle = i18n("This step is mandatory, please don't skip it.\n\nChoose a location to your future Klever Note storage or select an existing one.")
        root.checkForStorage(subtitle)
    }
}
