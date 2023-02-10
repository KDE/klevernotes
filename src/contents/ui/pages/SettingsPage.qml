// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

Kirigami.ScrollablePage {
    id:settingsPage
    title: i18nc("@title:window", "Settings")

    function checkForStorage(subtitle){
        var actualPath = Config.storagePath

        let component = Qt.createComponent("qrc:/contents/ui/dialogs/StorageDialog.qml")

        if (component.status == Component.Ready) {
            var dialog = component.createObject(settingsPage);

            if (subtitle) dialog.subtitle = i18n("Want to change your storage?\n\nPlease choose a location for your future KleverNotes storage or select an existing one.")

            dialog.parentObject = settingsPage
            dialog.drawer = applicationWindow().globalDrawer
            dialog.open()
            return
        }

        treeview.model = View.hierarchy(Config.storagePath,-1)
    }

    Kirigami.FormLayout {
        Row {
            Kirigami.FormData.label: "Storage path:"
            Controls.TextField {
                text: Config.storagePath
                readOnly: true
            }
            Controls.Button{
                icon.name: "folder-sync"
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                Controls.ToolTip.visible: hovered
                Controls.ToolTip.text: i18n("Change the storage path")
            }
            Controls.Button{
                icon.name: "folder-new"
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                Controls.ToolTip.visible: hovered
                Controls.ToolTip.text: i18n("Create a new storage")
            }
        }
        Item {
            // Kirigami.FormData.label: "Section Title"
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


