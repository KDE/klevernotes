// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

Kirigami.ScrollablePage {
    id:settingsPage

    title: i18nc("@title:window", "Settings")

    function updateStorage() {
        if (!Config.storagePath !== "None"){
            let component = Qt.createComponent("qrc:/contents/ui/dialogs/StorageDialog.qml")

            if (component.status == Component.Ready) {
                var dialog = component.createObject(applicationWindow());

                dialog.subtitle = i18n("Please choose a location for your future KleverNotes storage or select an existing one.\n")
                dialog.firstSetup = false

                dialog.open()
            }
        }
     }

    Kirigami.FormLayout {
        Row {
            Kirigami.FormData.label: "Storage path:"

            Controls.TextField {
                text: Config.storagePath
                readOnly: true
            }

            Controls.Button{
                text: i18n("Change storage path")
                onClicked: updateStorage()
            }
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        Controls.TextField {
            Kirigami.FormData.label: "New Category name:"
            text: Config.defaultCategoryName
        }

        Controls.TextField {
            Kirigami.FormData.label: "New Group name:"
            text: Config.defaultGroupName
        }

        Controls.TextField {
            Kirigami.FormData.label: "New Note name:"
            text: Config.defaultNoteName
        }
    }
}


