// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

Components.MessageDialog {
    property string error
    property bool isNote
    property QtObject nameField

    title: i18nc("@title:dialog, 'storage' as in 'the folder where all the notes will be stored'", "KleverNotes Storage")

    dialogType: Components.MessageDialog.Error

    closePolicy: Controls.Popup.NoAutoClose
    standardButtons: Kirigami.Dialog.Ok

    onAccepted: {
        nameField.selectAll()
        close();
    }

    function setSubtitle(): string {
        if (error === "dot") {
            return i18nc("@subtitle:dialog", "This name starts with a dot, this may cause problem and is therefore not allowed.") 
                + "\n" + i18nc("@subtitle:dialog", "Please remove the dot or choose another name.")
        } else if (error === "exist") {
            return (isNote 
                ? i18nc("@subtitle:dialog", "This note already exists.") 
                :  i18nc("@subtitle:dialog", "This folder already exists.")
            ) + "\n" 
            + i18nc("@subtitle:dialog", "Please choose another name for it.") + "\n"
        } else if (error === "/") {
            return i18nc("@subtitle:dialog", "This name contains '/', this may cause problem and is therefore not allowed.")
                + "\n" + i18nc("@subtitle:dialog", "Please remove the '/' or choose another name.")
        }

        return ""
    }

    Controls.Label {
        text: setSubtitle()
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
    }
}
