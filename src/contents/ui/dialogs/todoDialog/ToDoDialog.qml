// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

FormCard.FormCardDialog {
    id: promptDialog

    property int callerModelIndex: -1
    property alias name: nameField.text
    property alias description: descriptionField.text

    title: i18nc("@title:dialog", "Add Todo")

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    FormCard.FormTextFieldDelegate {
        id: nameField

        label: i18nc("@label:textbox", "Title:")
        maximumLength: 40
        placeholderText: i18nc("@placeholderText:textbox", "Todo title (required)")

        onAccepted: {
            descriptionField.forceActiveFocus()
        }
    }

    FormCard.FormDelegateSeparator {}

    TextAreaDelegate {
        id: descriptionField

        label: i18nc("@label:textbox", "Description:")
        wrapMode: TextEdit.WrapAnywhere
        maximumLength: 240
        placeholderText: i18nc("@placeholderText:textbox", "Optional")

        Layout.preferredHeight: Kirigami.Units.gridUnit * 8
    }
}
