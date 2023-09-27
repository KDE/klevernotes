// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

Kirigami.PromptDialog {
    id: textPromptDialog

    title: i18nc("@title:dialog", "Create your link")

    property alias urlText: urlTextField.text
    property alias linkText: linkTextField.text

    ColumnLayout {
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

    onOpened: urlTextField.forceActiveFocus()

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
}


