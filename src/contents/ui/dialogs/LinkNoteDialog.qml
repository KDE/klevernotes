// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kitemmodels 1.0
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import "qrc:/contents/ui/sharedComponents"

Kirigami.PromptDialog {
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

    ColumnLayout {
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
                headerComboBox.model = applicationWindow().noteMapper.getNoteHeaders(noteComboBox.currentValue)
            }
            Component.onCompleted: {
                currentIndex = 0
            }
        }

        ExpendingFormSwitch {
            id: headerSwitch

            text: i18nc("@label:switch", "Search headers")
            checked: false

            onCheckedChanged: if (checked) {
                headerComboBox.model = applicationWindow().noteMapper.getNoteHeaders(noteComboBox.currentValue)
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
}