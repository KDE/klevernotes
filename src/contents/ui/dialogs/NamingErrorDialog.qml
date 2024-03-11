// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.19 as Kirigami

Kirigami.PromptDialog {
    readonly property var useCaseTrad: {
        "category": i18nc("@subtitle:dialog, as in 'A note category'", "This category already exists."),
        "group": i18nc("@subtitle:dialog, as in 'A note group'", "This group already exists."),
        "note": i18nc("@subtitle:dialog", "This note already exists.") 
    }

    property string error
    property string useCase
    property QtObject nameField

    title: i18nc("@title:dialog, 'storage' as in 'the folder where all the notes will be stored'", "KleverNotes Storage")
    subtitle: setSubtitle()

    showCloseButton: false
    closePolicy: Controls.Popup.NoAutoClose
    standardButtons: Kirigami.Dialog.Ok

    onAccepted: {
        // can't use selectAll or select on FormTextFieldDelegate :/
        // nameField.selectAll()
    }

    function setSubtitle() {
        if (error === "dot") {
            return i18nc("@subtitle:dialog", "This name starts with a dot, this may cause problem and is therefore not allowed.") 
                + "\n" + i18nc("@subtitle:dialog", "Please remove the dot or choose another name.")
        }
        else if (error === "exist") {
            return useCaseTrad[useCase.toLowerCase()] + "\n" + i18nc("@subtitle:dialog", "Please choose another name for it.") + "\n"
        }
        
        return ""
    }
}
