// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.PromptDialog {
    title: "Klever Notes Storage"

    // Dirty workaround to prevent weird height
    height: header.height + footer.height + topPadding + bottomPadding + mainItem.height

    showCloseButton: false
    closePolicy: Controls.Popup.NoAutoClose
    standardButtons: Kirigami.Dialog.Ok

    property string useCase
    property QtObject nameField

    readonly property var useCaseTrad: {
        "Category": i18n("category"),
        "Group": i18n("group"),
        "Note": i18n("note")
    }

    subtitle: i18n("This "+useCaseTrad[useCase]+" already exist.\nPlease choose another name for it.\n")

    onAccepted: nameField.selectAll()
}
