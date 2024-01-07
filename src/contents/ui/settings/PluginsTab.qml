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
        title: i18nc("@title", "Utility plugins")
        Layout.fillWidth: true
    }

    FormCard.FormCard {
        id: utilityCard

        Layout.fillWidth: true

        FormCard.FormCheckDelegate {
            id: noteMapperCheck

            text: i18nc("@label:checkbox", "Enable note linking")
            description: i18nc("@description:checkbox", "Note linking allows you to create a link from one note to another.") 
                + "\n" + i18nc("@description:checkbox", "Advice: restart the app once activated.")
            checked: Config.noteMapEnabled

            onCheckedChanged: if (checked != Config.noteMapEnabled) {
                Config.noteMapEnabled = checked
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title", "Cosmetic plugins")
        Layout.fillWidth: true
    }

    FormCard.FormCard {
        id: appearanceCard

        Layout.fillWidth: true

        ExpendingFormCheckBox {
            id: highlitingCheck

            text: i18nc("@label:checkbox", "Enable code syntax highlighting")
            description: "<a href='https://invent.kde.org/office/klevernotes#syntax-highlighting'>" + i18nc("@description:checkbox", "List of supported highlighters") + "</a>"
            checked: Config.codeSynthaxHighlightEnabled

            onCheckedChanged: if (checked != Config.codeSynthaxHighlightEnabled) {
                Config.codeSynthaxHighlightEnabled = checked
            }

            FormCard.FormComboBoxDelegate {
                id: highlighterCombobox

                text: i18nc("@label:combobox", "Highlighter")
                model: HighlightHelper.highlighters 

                onModelChanged: if (model.length !== 0) {
                    const baseIndex = 0;

                    if (Config.codeSynthaxHighlighter.length === 0) {
                        highlighterCombobox.currentIndex = baseIndex
                        return
                    }

                    const inModelIndex = indexOfValue(Config.codeSynthaxHighlighter)

                    highlighterCombobox.currentIndex = inModelIndex === -1
                        ? baseIndex
                        : inModelIndex
                }
                onCurrentValueChanged: {
                    const highlighter = highlighterCombobox.currentValue
                    if (highlighter != Config.codeSynthaxHighlighter) Config.codeSynthaxHighlighter = highlighter
                }
            } 

            FormCard.FormComboBoxDelegate {
                id: styleCombobox
                
                property bool configStyleSet: false

                text: i18nc("@label:combobox", "Highlighter style")
                model: HighlightHelper.getHighlighterStyle(highlighterCombobox.currentValue)

                onCurrentValueChanged: {
                    if (!styleCombobox.configStyleSet) {
                        const baseIndex = 0;

                        if (Config.codeSynthaxHighlighterStyle.length === 0) {
                            styleCombobox.currentIndex = baseIndex
                            return
                        }

                        const inModelIndex = indexOfValue(Config.codeSynthaxHighlighterStyle)

                        styleCombobox.currentIndex = inModelIndex === -1
                            ? baseIndex
                            : inModelIndex

                        styleCombobox.configStyleSet = true
                    }
                    if (currentValue != Config.codeSynthaxHighlighterStyle) Config.codeSynthaxHighlighterStyle = currentValue
                }
            }
        }
    
        FormCard.FormDelegateSeparator { above: highlitingCheck; below: emojiCheck }

        ExpendingFormCheckBox {
            id: emojiCheck

            text: i18nc("@label:checkbox", "Enable quick emoji")
            description: i18nc("@description:checkbox, will be followed by the corresponding syntax, spacing between the end of the sentence and the syntax is already there", 
                "Quickly write emoji using the following syntax") + " :<i>" + i18nc("@exemple, something representing a possible emoji short name", "emoji_name") + "</i>:"
            checked: Config.quickEmojiEnabled

            onCheckedChanged: if (checked != Config.quickEmojiEnabled) {
                Config.quickEmojiEnabled = checked
            }

            FormCard.FormComboBoxDelegate {
                id: tonesCombobox

                textRole: "text"
                valueRole: "value"
                model: [
                    { value: "None", text: i18nc("@combobox:entry, As in 'default skin tone' (for an emoji), see '🫶'", "Default") + " 🫶" },
                    { value: "light skin tone", text: i18nc("@combobox:entry, As in 'light skin tone' (for an emoji), see '🫶🏻'", "Light") + " 🫶🏻" },
                    { value: "medium-light skin tone", text: i18nc("@combobox:entry, As in 'medium light skin tone' (for an emoji), see '🫶🏼'", "Medium light") + " 🫶🏼" },
                    { value: "medium skin tone", text: i18nc("@combobox:entry, As in 'Medium skin tone' (for an emoji), see '🫶🏽'", "Medium") + " 🫶🏽" },
                    { value: "medium-dark skin tone", text: i18nc("@combobox:entry, As in 'Medium-dark skin tone' (for an emoji), see '🫶🏾'", "Medium dark") + " 🫶🏾" },
                    { value: "dark skin tone", text: i18nc("@combobox:entry, As in 'dark skin tone' (for an emoji), see '🫶🏿'", "Dark") + " 🫶🏿" }
                ]

                Component.onCompleted: {
                    currentIndex = indexOfValue(Config.emojiTone)
                }
                onCurrentValueChanged: {
                    if (currentValue != Config.emojiTone) Config.emojiTone = currentValue
                }
            }
        }
    }
}
