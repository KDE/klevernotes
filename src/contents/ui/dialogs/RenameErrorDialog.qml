// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami

/*
Kirigami.PromptDialog {
    id: textPromptDialog
    title: "Klever Notes Storage"

    property string useCase
    property QtObject nameField
    subtitle:i18n("This "+useCase+" already exist.\nPlease choose another name for it.")

    onRejected: nameField.selectAll()
}*/
// Temporary use of simple Dialog instead of PromptDialog
//since PromptDialog seems to have issues with settings correct height
Kirigami.Dialog {
    id: textPromptDialog

    title: "Klever Notes Storage"
    standardButtons: Kirigami.Dialog.Cancel

    property string useCase
    property QtObject nameField

    readonly property var useCaseTrad: {
        "Category": i18n("category"),
        "Group": i18n("group"),
        "Note": i18n("note")
    }

    Text {
        Kirigami.Theme.colorSet: Kirigami.Theme.Button
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.textColor
        text: i18n("This "+useCaseTrad[useCase]+" already exist.\nPlease choose another name for it.\n")
    }

    onRejected: nameField.selectAll()
}
