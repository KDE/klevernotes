// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

Kirigami.PromptDialog {
    id: textPromptDialog

    title: i18nc("@title:dialog", "Choose an URL")

    property QtObject caller

    FormCard.FormTextFieldDelegate {
        id: urlField

        label: i18nc("@label:textbox, link URL, like the 'href' of an html <a> ", "URL:")

        Keys.onPressed: if ((event.key === Qt.Key_Return) || (event.key=== Qt.Key_Enter)) textPromptDialog.accept()
    }

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onAccepted: {
        caller.path = urlField.text
    }
}

