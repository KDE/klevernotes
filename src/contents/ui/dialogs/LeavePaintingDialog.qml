// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

Components.MessageDialog {
    title: i18nc("@title:dialog", "KleverNotes painting")

    dialogType: Components.MessageDialog.Warning

    closePolicy: Popup.NoAutoClose
    standardButtons: Kirigami.Dialog.Save | Kirigami.Dialog.Discard

    Label {
        text: i18nc("@subtitle:dialog", "You're about to leave without saving your drawing!")
            + "\n" + i18nc("@subtitle:dialog", "Do you want to save it before leaving ?") 

        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }
}
