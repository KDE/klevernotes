// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever 1.0

Kirigami.PromptDialog {
    id: textPromptDialog
    
    signal pdf()
    signal html()

    property string path

    title: i18nc("@title:dialog", "Printing Dialog")

    standardButtons: Kirigami.Dialog.Cancel

    onAccepted: {
        caller.path = urlField.text
    }
    onClosed: {
        path = ""
    }
 
    ColumnLayout {
        FormCard.FormButtonDelegate {
            text: i18nc("@label:button", "Print html")
            Layout.fillWidth: true
            onClicked: {
                html()
            }    
        }

        FormCard.FormButtonDelegate {
            text: i18nc("@label:button", "Print to pdf")
            description: enabled ? "" : i18nc("@label:button", "Disable due to flatpak limitation.")
            enabled: !KleverUtility.isFlatpak()
            Layout.fillWidth: true
            onClicked: {
                pdf()
                close()
            }
        }
    }
}
