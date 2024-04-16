// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardDialog {
    id: textPromptDialog

    property QtObject caller

    title: i18nc("@title:dialog", "Choose an URL")

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onAccepted: {
        caller.path = urlField.text
        close()
    }
    onRejected: {
        close()
    }
    onClosed: {
        urlField.clear()
    }

    FormCard.FormTextFieldDelegate {
        id: urlField

        label: i18nc("@label:textbox, link URL, like the 'href' of an html <a> ", "URL:")

        Keys.onPressed: if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
            textPromptDialog.accept()
        }
    }
}
