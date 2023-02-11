// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

Kirigami.Card{
    id: toolbarHolder

    property QtObject textArea

    // This 'replicate' the DefaultCardBackground and just change the background color
    //(https://api.kde.org/frameworks/kirigami/html/DefaultCardBackground_8qml_source.html)
    background: Kirigami.ShadowedRectangle{
        Kirigami.Theme.colorSet: Kirigami.Theme.Header
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor
        radius: Kirigami.Units.smallSpacing
    }

    Kirigami.ActionToolBar {
        id: mainToolBar

        function applyInstructions(selectionEnd,info,specialChars,multiPlaceApply){
            const instructions = info.instructions
            const lines = info.lines
            let end = selectionEnd
            let applied = false

            console.log(info.lines)

            for (var i = lines.length-1 ; i >= 0; i--){
                const line = lines[i]
                const instruction = instructions[i]

                const start = end-line.length
                switch(instruction) {
                case "apply":
                    if (multiPlaceApply) toolbarHolder.textArea.insert(end,specialChars)
                    toolbarHolder.textArea.insert(start,specialChars)

                    applied = true
                    break;
                case "remove":
                    if (multiPlaceApply) toolbarHolder.textArea.remove(end-specialChars.length,end)
                    toolbarHolder.textArea.remove(start,start+specialChars.length)
                    break;
                default:
                    break
                }
                end = start-1
            }
            if (applied) toolbarHolder.textArea.select(toolbarHolder.textArea.selectionStart-specialChars.length,toolbarHolder.textArea.selectionEnd)
        }

        function handleAction(selectionStart,selectionEnd,specialChars,multiPlaceApply) {
            console.log(selectionStart,selectionEnd)
            const selectedText = textArea.getText(selectionStart,selectionEnd)
            const info = MDHandler.getInstructions(selectedText,specialChars,multiPlaceApply)

            const appliedSpecialChars = specialChars[0]
            mainToolBar.applyInstructions(selectionEnd,info,appliedSpecialChars,multiPlaceApply)
        }

        function getLinesBlock(selectionStart,selectionEnd) {
            const startingText = textArea.getText(0,textArea.selectionStart)
            const endingText = textArea.getText(textArea.selectionEnd,textArea.text.length)


            const startBlockIndex = startingText.lastIndexOf('\n')+1
            let endIndex = endingText.indexOf('\n')-1
            const endBlockIndex = (endIndex < 0) ? textArea.text.length : textArea.selectionEnd+endingText

            console.log(startBlockIndex,endBlockIndex)
            return [startBlockIndex,endBlockIndex]
        }

        actions: [
            Kirigami.Action {
                text: "𝐇"

                Kirigami.Action {
                    text: "𝐇𝟏"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(textArea.selectionStart,textArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd,["# "],false)
                    }
                }
                Kirigami.Action {
                    text: "𝐇𝟐"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(textArea.selectionStart,textArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd,["## "],false)
                    }
                }
                Kirigami.Action {
                    text: "𝐇𝟑"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(textArea.selectionStart,textArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd,["### "],false)
                    }
                }
                Kirigami.Action {
                    text: "𝐇𝟒"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(textArea.selectionStart,textArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd,["#### "],false)
                    }
                }
                Kirigami.Action {
                    text: "𝐇𝟓"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(textArea.selectionStart,textArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd,["###### "],false)
                    }
                }
                Kirigami.Action {
                    text: "𝐇𝟔"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(textArea.selectionStart,textArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd,["####### "],false)
                    }
                }

            },
            Kirigami.Action {
                id: boldAction
                icon.name: "format-text-bold"
                onTriggered: mainToolBar.handleAction(textArea.selectionStart,textArea.selectionEnd,["**","__"],true)
            },
            Kirigami.Action {
                id: italicAction
                icon.name: "format-text-italic"
                onTriggered: mainToolBar.handleAction(textArea.selectionStart,textArea.selectionEnd,["_","*"],true)
            },
            Kirigami.Action {
                id: strikethroughAction
                icon.name: "format-text-strikethrough"
                onTriggered: mainToolBar.handleAction(textArea.selectionStart,textArea.selectionEnd,["~~"],true)
            },
            Kirigami.Action {
                id: codeBlockAction
                icon.name: "format-text-code"
                onTriggered: mainToolBar.handleAction(textArea.selectionStart,textArea.selectionEnd,["\n```\n"],true)
            },
            Kirigami.Action {
                id: quotekAction
                icon.name: "format-text-blockquote"
                onTriggered: mainToolBar.handleAction(textArea.selectionStart,textArea.selectionEnd,["> "],false)
            }
        ]
    }
}
