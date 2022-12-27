// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

Kirigami.PromptDialog {
    id: textPromptDialog
    title: "Klever Notes Storage"

    subtitle:i18n("It looks like this is your first time using this app!\n\nPlease choose a location for your future KleverNotes storage or select an existing one.")

    property string folder
    property string userChoice
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
            root.checkForStorage(subtitle)
            return
        }
        Config.storagePath = folderPath

        const fullNotification = textPromptDialog.userChoice+folderPath

        showPassiveNotification(fullNotification);
        textPromptDialog.close();
    }

    onRejected:{
        const subtitle = i18n("This step is mandatory, please don't skip it.\n\nChoose a location to your future KleverNotes storage or select an existing one.")
        root.checkForStorage(subtitle)
    }
}
