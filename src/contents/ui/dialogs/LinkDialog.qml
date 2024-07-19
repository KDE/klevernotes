// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardDialog {
    id: root

    property alias urlText: urlTextField.text
    property alias linkText: linkTextField.text
    property string path

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
    onPathChanged: {
        const toRemove = "file://"
        urlTextField.text = path.substring(toRemove.length)
    }

    FilePickerDialog {
        id: filePickerDialog

        caller: root
        nameFilters: []
    }
    
    FormCard.FormSwitchDelegate {
        id: localFileCheck

        text: i18nc("@label:checkbox", "Link local file")
        checked: false

        onCheckedChanged: {
            urlTextField.clear()
            forceActiveFocus()
        }
    }

    Kirigami.Separator {
        Layout.fillWidth: true
    }

    FormCard.FormTextFieldDelegate {
        id: urlTextField

        label: i18nc("@label:textbox, link URL, like the 'href' of an html <a> ", "URL:")
        Layout.fillWidth: true

        MouseArea {
            enabled: localFileCheck.checked
            anchors.fill: parent
            onClicked: filePickerDialog.open()
        }
    }

    FormCard.FormTextFieldDelegate {
        id: linkTextField

        label: i18nc("@label:textbox, the displayed text of a link, in html: <a>This text</a> ", "Link text:")
        Layout.fillWidth: true
    }
}
