// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.Klever 1.0

ColumnLayout {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    FormCard.FormHeader {
        title: i18nc("@title, as in text editor", "Editor")
        Layout.fillWidth: true
    }
    
    FormCard.FormCard {
        Layout.fillWidth: true 

        FontPicker {
            id: textEditorFont
            
            configFont: Config.editorFont
            label: i18nc("@label:textbox, the font used in the text editor", "Editor font:")

            onNewFontChanged: if (text !== newFont) {
                Config.editorFont = newFont
            }
        } 

        FormCard.FormComboBoxDelegate {
            text: i18nc("@label:combobox", "Cursor style:")
            description: i18nc("@description:combobox", "Advice: For styles other than I-Beam, it is recommended to use a monospaced font.")

            model: [
                { text: i18nc("@entry:combobox, cursor style", "Block"), value: "block" },
                { text: i18nc("@entry:combobox, alignment name", "I-Beam"), value: "beam" },
                { text: i18nc("@entry:combobox, alignment name", "Underline"), value: "underline" }
            ]
            textRole: "text"
            valueRole: "value"

            Component.onCompleted: {
                currentIndex = indexOfValue(Config.cursorStyle)
            }

            onCurrentValueChanged: {
                if (Config.cursorStyle !== currentValue) Config.cursorStyle = currentValue;
            }
        } 
    }

    FormCard.FormHeader {
        title: i18nc("@title, where you can view the renderer note", "Note preview")
        Layout.fillWidth: true
    }

    DisplayPreview {
        id: displayPreview
    }
}
