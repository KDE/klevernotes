// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

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
        title: i18nc("@title", "Utility plugins")
        Layout.fillWidth: true
    }

    FormCard.FormCard {
        id: utilityCard

        Layout.fillWidth: true

        FormCard.FormSwitchDelegate {
            id: noteMapperCheck

            text: i18nc("@label:checkbox", "Enable note linking")
            description: i18nc("@description:checkbox", "Note linking allows you to create a link from one note to another.") 
                + "\n" + i18nc("@description:checkbox", "Advice: restart the app once activated.")
            checked: KleverConfig.noteMapEnabled

            onCheckedChanged: if (checked != KleverConfig.noteMapEnabled) {
                KleverConfig.noteMapEnabled = checked
            }
        }

        ExpandingFormSwitch {
            id: pumlCheck

            text: i18nc("@label:checkbox", "Enable PlantUML")
            description: i18nc("@description:checkbox", "PlantUML lets you create diagrams.")
                + "\n" + i18nc("@description:checkbox", "Note: creating diagrams can be slow. If you have a large number of diagrams, displaying them as images would be easier.")
            checked: KleverConfig.pumlEnabled

            onCheckedChanged: if (checked != KleverConfig.pumlEnabled) {
                KleverConfig.pumlEnabled = checked
            }

            FormCard.FormSwitchDelegate {
                id: pumlDarkSwitch

                text: i18nc("@lable:switch", "Enable dark background")
                description: i18nc("@description:switch", "PlantUML diagram will have a dark background.")
                checked: KleverConfig.pumlDark

                onCheckedChanged: if (checked != KleverConfig.pumlDark) {
                    KleverConfig.pumlDark = checked
                }
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

        ExpandingFormSwitch {
            id: highlitingCheck

            text: i18nc("@label:checkbox", "Enable code syntax highlighting")
            description: "<a href='https://invent.kde.org/office/klevernotes#syntax-highlighting'>" + i18nc("@description:checkbox", "List of supported highlighters") + "</a>"
            checked: KleverConfig.codeSynthaxHighlightEnabled

            onCheckedChanged: if (checked != KleverConfig.codeSynthaxHighlightEnabled) {
                KleverConfig.codeSynthaxHighlightEnabled = checked
            }

            FormCard.FormComboBoxDelegate {
                id: highlighterCombobox

                text: i18nc("@label:combobox", "Highlighter")
                model: HighlightHelper.highlighters 

                onModelChanged: if (model.length !== 0) {
                    const baseIndex = 0;

                    if (KleverConfig.codeSynthaxHighlighter.length === 0) {
                        highlighterCombobox.currentIndex = baseIndex
                        return
                    }

                    const inModelIndex = model.indexOf(KleverConfig.codeSynthaxHighlighter)

                    highlighterCombobox.currentIndex = inModelIndex === -1
                        ? baseIndex
                        : inModelIndex
                }
                onCurrentValueChanged: {
                    const highlighter = highlighterCombobox.currentValue
                    if (highlighter != KleverConfig.codeSynthaxHighlighter) KleverConfig.codeSynthaxHighlighter = highlighter
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

                        if (KleverConfig.codeSynthaxHighlighterStyle.length === 0) {
                            styleCombobox.currentIndex = baseIndex
                        } else {
                            const inModelIndex = model.indexOf(KleverConfig.codeSynthaxHighlighterStyle)

                            styleCombobox.currentIndex = inModelIndex === -1
                                ? baseIndex
                                : inModelIndex
                        }
                        styleCombobox.configStyleSet = true
                    }
                    if (currentValue != KleverConfig.codeSynthaxHighlighterStyle) {
                        KleverConfig.codeSynthaxHighlighterStyle = currentValue
                    }
                }
            }
        }
    
        FormCard.FormDelegateSeparator { above: highlitingCheck; below: emojiCheck }

        ExpandingFormSwitch {
            id: emojiCheck

            text: i18nc("@label:checkbox", "Enable quick emoji")
            description: i18nc("@description:checkbox, will be followed by the corresponding syntax, spacing between the end of the sentence and the syntax is already there", 
                "Quickly write emoji using the following syntax") + " :<i>" + i18nc("@example, something representing a possible emoji short name", "emoji_name") + "</i>:"
            checked: KleverConfig.quickEmojiEnabled

            onCheckedChanged: if (checked != KleverConfig.quickEmojiEnabled) {
                KleverConfig.quickEmojiEnabled = checked
            }

            FormCard.FormComboBoxDelegate {
                id: tonesCombobox


                text: i18nc("@label:combobox", "Default emoji tone")
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
                    currentIndex = indexOfValue(KleverConfig.emojiTone)
                }
                onCurrentValueChanged: {
                    if (currentValue != KleverConfig.emojiTone) KleverConfig.emojiTone = currentValue
                }
            }

            FormCard.FormSwitchDelegate {
                id: quickEmojiDialogSwitch

                text: i18nc("@lable:switch", "Enable quick emoji dialog")
                description: i18nc("@description:switch", "The emoji dialog insert the 'quick emoji' syntax instead of the actual emoji inside the text.")
                checked: KleverConfig.quickEmojiDialogEnabled

                onCheckedChanged: if (checked != KleverConfig.quickEmojiDialogEnabled) {
                    KleverConfig.quickEmojiDialogEnabled = checked
                }
            }
        }
    }
}
