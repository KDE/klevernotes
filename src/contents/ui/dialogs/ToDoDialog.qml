// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as Controls

Kirigami.PromptDialog {
    id: promptDialog

    title: i18n("Add Todo")

    property alias name: nameField.text
    property alias description: descriptionField.text

    ColumnLayout {
        Kirigami.Heading {
            text: i18nc("@label:textbox", "Title:")
            level: 2

            Layout.fillWidth: true
        }

        Controls.TextField {
            id: nameField

            placeholderText: i18n("Todo title (required)")
            onAccepted: descriptionField.forceActiveFocus()

            Layout.fillWidth: true
        }

        Kirigami.Heading {
            text: i18n("Description:")
            level: 2

            Layout.fillWidth: true
        }

        Controls.ScrollView {
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 4

            Controls.TextArea {
                id: descriptionField

                placeholderText: i18n("Optional")
                wrapMode: TextEdit.WrapAnywhere
            }
        }
    }

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
}

