// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

ColumnLayout {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true
    
    FormCard.FormCard {
        Layout.fillWidth: true
        Layout.topMargin: Kirigami.Units.gridUnit

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
