// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

import "qrc:/contents/ui/textEditor/components/"

Kirigami.Dialog {
    id: cheatSheet

    title: i18nc("@window:title","Markdown Cheat Sheet")

    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor    
    }

    width: applicationWindow().width > Kirigami.Units.gridUnit * 31 ? Kirigami.Units.gridUnit * 30 : applicationWindow().width - Kirigami.Units.gridUnit
    height: applicationWindow().height - Kirigami.Units.gridUnit * 2//0 ? Kirigami.Units.gridUnit * 28

    standardButtons: Controls.Dialog.NoButton

    ColumnLayout {
        spacing: 0

        TextEdit { // Don't move this, weird height issue with the OverlaySheet otherwise
            id: clipboardHelper
            visible: false
            onTextChanged: if (length > 0) {
                selectAll()
                copy()
                showPassiveNotification(i18n("Copied !"))
                cheatSheet.close()
            }
        }

        FormCard.FormHeader {
            Layout.fillWidth: true
            title: i18nc("@title, cheat sheet section", "Basic syntax")
        }

        FormCard.FormCard {
            Layout.fillWidth: true

            CheatSheetEntry {
                element: "<h1>" + i18n("Heading") + "</h1>"
                syntax: "# " + i18nc("example","Heading1")
                onClicked: clipboardHelper.text = syntax
            }

            CheatSheetEntry {
                element: "<b>" + i18n("Bold") + "</b>"
                syntax: "**" + i18nc("example","Bold text") + "**"
                onClicked: clipboardHelper.text = syntax
            } 

            CheatSheetEntry {
                element: "<i>" + i18n("Italic") + "</i>"
                syntax: "_" + i18nc("example","Italic text") + "_"
                onClicked: clipboardHelper.text = syntax
            } 

            CheatSheetEntry {
                element: i18n("Blockquote")
                syntax: "> " + i18nc("example","Quote")
                onClicked: clipboardHelper.text = syntax
            } 

            CheatSheetEntry {
                element: i18n("Ordered list")
                syntax: "1. " + i18nc("example","First item") + "\n" +
                        "2. " + i18nc("example","Second item")
                onClicked: clipboardHelper.text = syntax
            } 

            CheatSheetEntry {
                element: i18n("Unordered list")
                syntax: "- " + i18nc("example","First item") + "\n" +
                        "- " + i18nc("example","Second item")
                onClicked: clipboardHelper.text = syntax
            } 

            CheatSheetEntry {
                element: i18n("Code")
                syntax: "`" + i18nc("example","Inline code") + "`"
                onClicked: clipboardHelper.text = syntax
            } 

            CheatSheetEntry {
                element: i18nc("@label, horizontal rule => <hr> html tag", "Rule")
                syntax: "---"
                onClicked: clipboardHelper.text = syntax
            } 

            CheatSheetEntry {
                element: i18n("Link")
                syntax: "[" + i18nc("example","title") + "](" + i18nc("example", "https://www.example.com") + ")"
                onClicked: clipboardHelper.text = syntax
            } 

            CheatSheetEntry {
                element: i18n("Image")
                syntax: "![" + i18nc("example","alt text") + "](" + i18nc("example", "image") + ".jpg)"
                onClicked: clipboardHelper.text = syntax
            } 
        }

        FormCard.FormHeader {
            Layout.fillWidth: true
            title: i18nc("@title, cheat sheet section", "Extended syntax")
        }

        FormCard.FormCard {
            Layout.fillWidth: true

            CheatSheetEntry {
                element: i18n("Table")
                syntax: "|" + i18nc("@label, noun, 'The left side of something'","Left") + "|" + i18nc("@label, verb, 'To center something'","Center") + "|" + i18nc("@label, noun, 'The right side of something", "Right") + "|\n" +
                        "| :------ | :------: | ------:|\n" +
                        "|" + i18nc("example","Cell 1") + "|" + i18nc("example", "Cell 2") + "|" + i18nc("example", "Cell 3") + "|"
                onClicked: clipboardHelper.text = syntax
            }

            CheatSheetEntry {
                element: i18n("Fenced code block")
                syntax: "```\n" +
                        i18nc("example","Sample text here...") + "\n" +
                        "```"
                onClicked: clipboardHelper.text = syntax
            }

            CheatSheetEntry {
                element: i18n("Strikethrough")
                syntax: "~~" + i18nc("example, something wrong","The world is flat.") + "~~"
                onClicked: clipboardHelper.text = syntax
            }

            CheatSheetEntry {
                element: i18n("Task list")
                syntax: "- [ ] " + i18nc("example", "Make more foo") + "\n" +
                        "- [x] " + i18nc("example", "Make more bar")
                onClicked: clipboardHelper.text = syntax
            }

            CheatSheetEntry {
                element: i18n("Highlight")
                syntax: "==" + i18nc("example", "highlighted text") + "=="
                onClicked: clipboardHelper.text = syntax
            }

            CheatSheetEntry {
                element: i18n("Subscript")
                syntax: "--" + i18nc("example", "subscript text") + "--"
                onClicked: clipboardHelper.text = syntax
            }

            CheatSheetEntry {
                element: i18n("Superscript")
                syntax: "^" + i18nc("example", "superscript text") + "^"
                onClicked: clipboardHelper.text = syntax
            }
        }

        FormCard.FormHeader {
            Layout.fillWidth: true
            title: i18nc("@title, cheat sheet section", "KleverNotes plugins")
            visible: Config.noteMapEnabled // Will change when more plugins are added
        }

        FormCard.FormCard {
            Layout.fillWidth: true
            visible: Config.noteMapEnabled

            CheatSheetEntry {
                element: i18n("Note link")
                syntax: "[[ " 
                    + i18nc("example, These are placeholders, not XML tags. Please translate to your language", "<path to note>/<note name>") 
                    /* TODO: fix this when reworking headers
                     + ":# " + i18nc("example", "header") */
                     + " | " + i18nc("example", "displayed name") + " ]]"
                onClicked: clipboardHelper.text = syntax
            }
        }
    }
}
