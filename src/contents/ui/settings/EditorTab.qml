// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.klevernotes

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
            
            configFont: KleverConfig.editorFont
            label: i18nc("@label:textbox, the font used in the text editor", "Editor font:")

            onNewFontChanged: if (configFont !== newFont) {
                KleverConfig.editorFont = newFont
            }
        } 
    }

    FormCard.FormCard {
        Layout.fillWidth: true

        ExpandingFormSwitch {
            id: spaceTabSwitch

            text: i18nc("@label:checkbox", "Use spaces for tab")
            checked: KleverConfig.useSpaceForTab

            onCheckedChanged: if (checked != KleverConfig.useSpaceForTab) {
                KleverConfig.useSpaceForTab = checked
            }

            FormCard.FormSpinBoxDelegate {
                id: spaceSpinBox

                property bool isInit: false

                label: i18nc("@label:combobox", "Number of spaces")

                from: 1
                to: 8

                Component.onCompleted: {
                    value = KleverConfig.spacesForTab
                    isInit = true
                }
                onValueChanged: {
                    if (value != KleverConfig.spacesForTab && isInit) KleverConfig.spacesForTab = value
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
            checked: KleverConfig.editorHighlightEnabled

            onCheckedChanged: if (checked != KleverConfig.editorHighlightEnabled) {
                KleverConfig.editorHighlightEnabled = checked
            }

            EditorHighlightPreview {
                id: editorHighlightPreview
            }
        }
    }
}
