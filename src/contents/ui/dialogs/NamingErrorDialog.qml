// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.19 as Kirigami

Kirigami.PromptDialog {
    readonly property var useCaseTrad: {
        "category": i18nc("Name, as in 'A note category'", "category"),
        "group": i18nc("Name, as in 'A note group'", "group"),
        "note": i18n("Name, as in 'A note'", "note")
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
        nameField.selectAll()
    }

    function setSubtitle() {
        if (error === "dot") {
            return i18nc("@subtitle:dialog, %1 can be 'category' (a note category), 'group' (a note group) or 'note' (a note)", "Your %1 name starts with a dot, "+
                        "this may cause problem and is therefore not allowed.\n"+
                        "Please remove the dot or choose another name.", useCaseTrad[useCase.toLowerCase()])
        }
        else if (error === "exist") {
            return i18nc("@subtitle:dialog, %1 can be 'category' (a note category), 'group' (a note group) or 'note' (a note)", "This %1 already exist.\nPlease choose another name for it.\n", useCaseTrad[useCase.toLowerCase()])
        }
    }
}
