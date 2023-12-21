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
    }

    FormCard.FormHeader {
        title: i18nc("@title, where you can view the renderer note", "Note preview")
        Layout.fillWidth: true
    }

    DisplayPreview {
        id: displayPreview
    }
}
