// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.Klever 1.0

ColumnLayout {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    FormCard.FormHeader {
        title: i18nc("@title, general settings", "General")
        Layout.fillWidth: true
    }

    FormCard.FormCard {
        Layout.fillWidth: true

        FormCard.FormTextFieldDelegate {
            id: storageField

            text: Config.storagePath
            label: i18nc("@label:textbox, Storage as in 'the folder where all the notes will be stored'", "Storage path:")

            Layout.margins: 0
            Layout.fillWidth: true
            
            // workaround to make it readOnly
            onTextChanged: {
                text = Config.storagePath
            }

            MouseArea {
                anchors.fill: parent
                onClicked: storageDialog.open()
            }
        }

        FormCard.FormDelegateSeparator { above: storageField; below: newCategoryField }

        FormCard.FormTextFieldDelegate {
            id: newCategoryField

            property string name
            property bool isActive: false

            text: Config.defaultCategoryName
            label: i18nc("@label:textbox, the default note category name", "New Category name:")

            Layout.margins: 0
            Layout.fillWidth: true

            onNameChanged: if (isActive) {
                text = name
                Config.defaultCategoryName = name
                isActive = false
                name = ""
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    newCategoryField.isActive = true
                    updateName(newCategoryField.text, newCategoryField)
                }
            }
        }

        FormCard.FormDelegateSeparator { above: newCategoryField; below: newGroupField }

        FormCard.FormTextFieldDelegate {
            id: newGroupField

            property string name
            property bool isActive: false

            text: Config.defaultGroupName
            label: i18nc("@label:textbox, the default note group name", "New Group name:")

            Layout.margins: 0
            Layout.fillWidth: true

            onNameChanged: if (isActive) {
                text = name
                Config.defaultGroupName = name
                isActive = false
                name = ""
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    newGroupField.isActive = true
                    updateName(newGroupField.text, newGroupField)
                }
            }
        }

        FormCard.FormDelegateSeparator { above: newGroupField; below: newNoteField }

        FormCard.FormTextFieldDelegate {
            id: newNoteField

            property string name
            property bool isActive: false

            text: Config.defaultNoteName
            label: i18nc("@label:textbox, the default note name", "New Note name:")

            Layout.margins: 0
            Layout.fillWidth: true

            onNameChanged: if (isActive) {
                text = name
                Config.defaultNoteName = name
                isActive = false
                name = ""
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    newNoteField.isActive = true
                    updateName(newNoteField.text, newNoteField)
                }
            }
        }

        FormCard.FormDelegateSeparator { above: newNoteField; below: textEditorFont }

        FontPicker {
            id: textEditorFont
            
            configFont: Config.editorFont
            label: i18nc("@label:textbox, the font used in the text editor", "Editor font:")

            onNewFontChanged: if (text !== newFont) {
                Config.editorFont = newFont
            }
        }
    }
}
