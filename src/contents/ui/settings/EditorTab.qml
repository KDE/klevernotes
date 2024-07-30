// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

import "qrc:/contents/ui/sharedComponents"

ColumnLayout {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true
    
    FormCard.FormHeader {
        title: i18nc("@title, general text editor settings", "General")
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

    FormCard.FormCard {
        Layout.fillWidth: true

        ExpandingFormSwitch {
            id: spaceTabSwitch

            text: i18nc("@label:checkbox", "Use spaces for tab")
            checked: Config.useSpaceForTab

            onCheckedChanged: if (checked != Config.useSpaceForTab) {
                Config.useSpaceForTab = checked
            }

            FormCard.FormSpinBoxDelegate {
                id: spaceSpinBox

                property bool isInit: false

                label: i18nc("@label:combobox", "Number of spaces")

                from: 1
                to: 8

                Component.onCompleted: {
                    value = Config.spacesForTab
                    isInit = true
                }
                onValueChanged: {
                    if (value != Config.spacesForTab && isInit) Config.spacesForTab = value
                }
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title", "Editor highlighting")
        Layout.fillWidth: true
    }

    FormCard.FormCard {
        id: highlighterCard

        Layout.fillWidth: true

        ExpandingFormSwitch {
            id: editorHighlightCheck

            text: i18nc("@label:checkbox", "Enable editor highlighting")
            checked: Config.editorHighlightEnabled

            onCheckedChanged: if (checked != Config.editorHighlightEnabled) {
                Config.editorHighlightEnabled = checked
            }
        }
    }
}
