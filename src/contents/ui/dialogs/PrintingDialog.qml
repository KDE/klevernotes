// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever 1.0

FormCard.FormCardDialog {
    id: textPromptDialog

    signal pdf()
    signal html()

    property string path

    title: i18nc("@title:dialog", "Printing Dialog")

    standardButtons: Dialog.Cancel

    onAccepted: {
        caller.path = urlField.text
    }
    onRejected: {
        close()
    }
    onClosed: {
        path = ""
    }
 
    FormCard.FormButtonDelegate {
        text: i18nc("@label:button", "Print html")
        Layout.fillWidth: true
        onClicked: {
            html()
        }
    }

    FormCard.FormDelegateSeparator {}

    FormCard.FormButtonDelegate {
        text: i18nc("@label:button", "Print to PDF")
        description: enabled ? "" : i18nc("@label:button", "Disabled due to Flatpak limitations.")
        enabled: !KleverUtility.isFlatpak()
        Layout.fillWidth: true
        onClicked: {
            pdf()
            close()
        }
    }
}
