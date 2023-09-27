// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

Kirigami.PromptDialog {
    id: promptDialog

    title: i18nc("@title:dialog", "Add Todo")

    property int callerModelIndex: -1
    property alias name: nameField.text
    property alias description: descriptionField.text

    ColumnLayout {
        FormCard.FormTextFieldDelegate {
            id: nameField

            label: i18nc("@label:textbox", "Title:")
            maximumLength: 40
            placeholderText: i18nc("@placeholderText:textbox", "Todo title (required)")
            Layout.fillWidth: true

            onAccepted: descriptionField.forceActiveFocus()
        }

        TextAreaDelegate {
            id: descriptionField

            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 8

            label: i18nc("@label:textbox", "Description:")
            wrapMode: TextEdit.WrapAnywhere
            placeholderText: i18nc("@placeholderText:textbox", "Optional")
            maximumLength: 240
        }
    }

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
}

