// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

Kirigami.Dialog {
    id: setupPopup

    width: Kirigami.Units.gridUnit * 22

    closePolicy: Controls.Popup.NoAutoClose
    standardButtons: Kirigami.Dialog.NoButton

    property string subtitle: i18nc("@subtitle:dialog, Storage as in 'the folder where all the notes will be stored'", "It looks like this is your first time using this app!\n\nPlease choose a location for your future KleverNotes storage or select an existing one.")
    readonly property string existingStorage: i18nc("Storage as in 'the folder where all the notes will be stored'; this text will be followed by the path to this folder", "Existing storage chosen at ")
    readonly property string newStorage: i18nc("Storage as in 'the folder where all the notes will be stored'; this text will be followed by the path to this folder", "Storage created at ")
    property bool firstSetup: true
    property string folder
    property string userChoice
    showCloseButton: !firstSetup

    ColumnLayout{
        width: parent.width

        Controls.Label {
            text: subtitle
            wrapMode: Text.Wrap

            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.smallSpacing
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Controls.Button{
                icon.name: "folder-sync"
                text: i18nc("@label:button, Storage as in 'the folder where all the notes will be stored'", "Existing storage")

                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                Controls.ToolTip.visible: hovered
                Controls.ToolTip.text: i18nc("@label:button, Storage as in 'the folder where all the notes will be stored'", "Change the storage path")

                Layout.fillWidth: true
                Layout.margins: Kirigami.Units.smallSpacing

                onClicked: {
                    setupPopup.userChoice = setupPopup.existingStorage
                    getFolder()
                }
            }

            Controls.Button{
                icon.name: "folder-new"
                text: i18nc("@label:button, Storage as in 'the folder where all the notes will be stored'", "Create storage")

                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                Controls.ToolTip.visible: hovered
                Controls.ToolTip.text: i18nc("Storage as in 'the folder where all the notes will be stored'", "Create a new storage")

                Layout.fillWidth: true
                Layout.margins: Kirigami.Units.smallSpacing

                onClicked: {
                    setupPopup.userChoice = setupPopup.newStorage
                    getFolder()
                }
            }
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

    onFolderChanged: {
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
