// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

Components.MessageDialog {
    title: i18nc("@title:dialog", "Mandatory operation") 
    dialogType: Components.MessageDialog.Warning

    standardButtons: Kirigami.Dialog.Apply | Kirigami.Dialog.Close

    Controls.Label {
        text: i18nc("@subtitle:dialog, 'storage' as in 'the folder where all the notes will be stored'",
            "This storage appears to be using the old KleverNotes file structure which is no longer supported."
        ) + "\n\n" + i18nc("@subtitle:dialog, the standard Qt 'Apply' button", 
            "By pressing the 'Apply' button, you agree to convert the storage to the new structure (this should not be a destructive operation)."
        ) + "\n\n" + i18nc("@subtitle:dialog", 
            "You can cancel this operation by closing this dialog, which gives you time to back up your files if you want to."
        )

        Layout.fillWidth: true
        wrapMode: Text.WordWrap
    }

    Controls.Label {
        text: "\n\n" + i18nc("@subtitle:dialog", 
            "Note: Any locally saved images will be moved from the 'Images' subfolder to the note’s parent folder."
        ) + "\n" + i18nc("@subtitle:dialog", 
            "Simply remove '/Images' from their path to make them reappear."
        )

        Layout.fillWidth: true
        wrapMode: Text.WordWrap
    }
}
