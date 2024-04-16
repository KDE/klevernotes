// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

FormCard.FormCardDialog {
    id: setupPopup

    readonly property string newStorage: i18nc("Storage as in 'the folder where all the notes will be stored'; this text will be followed by the path to this folder", "Storage created at ")
    readonly property string existingStorage: i18nc("Storage as in 'the folder where all the notes will be stored'; this text will be followed by the path to this folder", "Existing storage chosen at ")

    property string folder
    property string userChoice
    property bool firstSetup: true
    property string subtitle: i18nc("@subtitle:dialog, Storage as in 'the folder where all the notes will be stored'", "It looks like this is your first time using this app!\n\nPlease choose a location for your future KleverNotes storage or select an existing one.")

    width: Kirigami.Units.gridUnit * 18

    title: i18nc("@title:dialog, Storage as in 'the folder where all the notes will be stored'", "KleverNotes Storage")
    closePolicy: firstSetup ? Controls.Popup.NoAutoClose : Controls.Popup.CloseOnReleaseOutside
    standardButtons: firstSetup ? Kirigami.Dialog.NoButton : Kirigami.Dialog.Cancel

    onFolderChanged: {
        setupStorage()
    }
    onRejected: {
        close()
    }

    Controls.Label {
        text: subtitle
        wrapMode: Text.Wrap

        Layout.margins: Kirigami.Units.largeSpacing * 2
        Layout.fillWidth: true
    }

    FormCard.FormButtonDelegate {
        icon.name: "folder-sync-symbolic"
        text: i18nc("@label:button, Storage as in 'the folder where all the notes will be stored'", "Existing storage")

        Controls.ToolTip.text: i18nc("@label:button, Storage as in 'the folder where all the notes will be stored'", "Change the storage path")
        Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
        Controls.ToolTip.visible: hovered

        Layout.fillWidth: true

        onClicked: {
            setupPopup.userChoice = setupPopup.existingStorage
            getFolder()
        }
    }

    FormCard.FormButtonDelegate {
        icon.name: "folder-new-symbolic"
        text: i18nc("@label:button, Storage as in 'the folder where all the notes will be stored'", "Create storage")

        Controls.ToolTip.text: i18nc("Storage as in 'the folder where all the notes will be stored'", "Create a new storage")
        Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
        Controls.ToolTip.visible: hovered

        Layout.fillWidth: true

        onClicked: {
            setupPopup.userChoice = setupPopup.newStorage
            getFolder()
        }
    }

    function getFolder() {
        let component = Qt.createComponent("qrc:/contents/ui/dialogs/FolderPickerDialog.qml")

        if (component.status == Component.Ready) {
            var dialog = component.createObject(setupPopup);
            dialog.parent = setupPopup
            dialog.open()
        }
    }

    function setupStorage() {
        let folderPath = KleverUtility.getPath(setupPopup.folder)
        if (userChoice === setupPopup.newStorage){
            folderPath = folderPath.concat("/klevernotes")
        }

        var pathEnd = folderPath.substring(folderPath.length,folderPath.length-11)

        if (pathEnd.toLowerCase() !== "klevernotes"){
            subtitle = i18nc("@subtitle:dialog, Storage as in 'the folder where all the notes will be stored'", "It looks like the selected folder is not a KleverNotes storage.\n\nPlease choose a location for your future KleverNotes storage or select an existing one.")
            return
        }
        Config.storagePath = folderPath

        const fullNotification = setupPopup.userChoice+folderPath

        showPassiveNotification(fullNotification);
        setupPopup.close();
    }
}
