// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kitemmodels
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import "qrc:/contents/ui/sharedComponents"

FormCard.FormCardDialog {
    id: linkNoteDialog

    property alias linkText: linkTextField.text
    readonly property string path: noteComboBox.currentValue
    property string headerString: ""
    required property var listModel

    title: i18nc("@title:dialog", "Create your link")

    width: Kirigami.Units.gridUnit * 20

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onClosed: {
        linkText = ""
        headerSwitch.checked = false
        noteComboBox.currentIndex = 0
    }
    onRejected: {
        close()
    }

    FormCard.FormComboBoxDelegate {
        id: noteComboBox

        text: i18nc("@label:combobox, 'a note' (the noun)", "Note:")
        model: KSortFilterProxyModel {
            id: searchFilterProxyModel

            sourceModel: KDescendantsProxyModel {
                id: descendants
                model: linkNoteDialog.listModel 
            }
            filterRoleName: "useCase"
            filterString: "Note"
        }
        textRole: "fullName"
        valueRole: "path"
        displayMode: FormCard.FormComboBoxDelegate.Dialog

        onCurrentValueChanged: if (headerSwitch.checked) {
            headerComboBox.model = NoteMapper.getNoteHeaders(noteComboBox.currentValue)
        }
        Component.onCompleted: {
            currentIndex = 0
        }
    }

    ExpandingFormSwitch {
        id: headerSwitch

        text: i18nc("@label:switch", "Search headers")
        checked: false

        onCheckedChanged: if (checked) {
            headerComboBox.model = NoteMapper.getNoteHeaders(noteComboBox.currentValue)
        } else {
            linkNoteDialog.headerString = ""
        }

        FormCard.FormComboBoxDelegate {
            id: headerComboBox

            text: i18nc("@label:textbox", "Header:")

            textRole: "text"
            valueRole: "value"
            displayMode: FormCard.FormComboBoxDelegate.Dialog

            onCurrentValueChanged: {
                linkNoteDialog.headerString = currentValue
            }
        }
    }

    FormCard.FormTextFieldDelegate {
        id: linkTextField

        label: i18nc("@label:textbox, the displayed text of a link, in html: <a>This text</a> ", "Link text:")
        Layout.fillWidth: true
    }
}
