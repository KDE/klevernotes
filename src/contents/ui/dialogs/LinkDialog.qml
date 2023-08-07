// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls

Kirigami.PromptDialog {
    id: textPromptDialog

    title: i18n("Create your link")

    property alias urlText: urlTextField.text
    property alias linkText: linkTextField.text

    Kirigami.FormLayout {
        Row {
            Kirigami.FormData.label: i18n("URL:")

            Controls.TextField {
                id: urlTextField

                focus: true
            }
        }

        Row {
            Kirigami.FormData.label: i18n("Link text:")

            Controls.TextField {
                id: linkTextField
            }
        }
    }

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
}


