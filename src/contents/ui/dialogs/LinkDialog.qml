// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardDialog {
    id: textPromptDialog

    property alias urlText: urlTextField.text
    property alias linkText: linkTextField.text

    title: i18nc("@title:dialog", "Create your link")

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onOpened: {
        urlTextField.forceActiveFocus()
    }
    onClosed: {
        urlText = ""
        linkText = ""
    }
    onRejected: {
        close()
    }

    FormCard.FormTextFieldDelegate {
        id: urlTextField

        label: i18nc("@label:textbox, link URL, like the 'href' of an html <a> ", "URL:")
        Layout.fillWidth: true
    }

    FormCard.FormTextFieldDelegate {
        id: linkTextField

        label: i18nc("@label:textbox, the displayed text of a link, in html: <a>This text</a> ", "Link text:")
        Layout.fillWidth: true
    }
}
