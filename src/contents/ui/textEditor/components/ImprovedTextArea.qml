// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.2

import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

TextArea {
    id: root

    required property bool vimModeOn

    readonly property bool __isVisual: __currentMode === 259 
    property int __currentMode: vimHandler.mode

    font: Config.editorFont
    wrapMode: TextEdit.Wrap
    persistentSelection: true

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    // NOTE: temporary solution
    background: Rectangle {
        border.color: switch (__currentMode) {
            case 257: //Qt::UserRole + 1
                "#8CB039"
                break
            case 258:
                "#3B9DEF"
                break
            case 259:
                "#D7A656"
                break                
        }
        border.width: 3
        color: Kirigami.Theme.backgroundColor
    }

    cursorDelegate: Config.cursorStyle !== "beam" ? customCursor : null
    
    Component {
        id: customCursor
        Rectangle {
            id: cursorBackground
            readonly property bool isBlock: Config.cursorStyle === "block"
            color: isBlock ? Kirigami.Theme.textColor : Kirigami.Theme.backgroundColor
            width: t_metrics.tightBoundingRect.width
            height: t_metrics.tightBoundingRect.height

            Rectangle {
                id: underline

                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }

                y: parent.height - height
                height: Math.round(parent.height / 10)
                visible: !parent.isBlock
            }

            Text {
                id: a_text
                text: root.getText(root.cursorPosition, root.cursorPosition + 1)
                color: cursorBackground.isBlock ? Kirigami.Theme.backgroundColor : Kirigami.Theme.textColor
                font: root.font
                anchors.centerIn: parent
            }

            TextMetrics {
                id: t_metrics
                font: root.font
                // Workaround: One of the largest letter, in case a monospace font is not used
                text: "W" 
            }
        }
    }

    Keys.onPressed: (event) => {
        if (root.vimModeOn) {
            event.accepted = vimHandler.handleKeyPress(event.key, event.modifiers, __isVisual) 
        }
    }
    Keys.onTabPressed: {
        handleTabPressed(false)
    }
    Keys.onBacktabPressed: {
        handleTabPressed(true)
    }
    Keys.onReturnPressed: {
        const [blockStart, blockEnd] = MDHandler.getBlockLimits(selectionStart, selectionEnd, text)
        const newString = MDHandler.getLineFromPrevious(getText(blockStart, blockEnd))
        insert(selectionEnd, newString)
    }

    VimHandler {
        id: vimHandler

        vimOn: root.vimModeOn
        textArea: root
        document: root.textDocument
        cursorPosition: root.cursorPosition
        selectionStart: root.selectionStart
        selectionEnd: root.selectionEnd

        onCursorPositionChanged: (position) => {
            root.cursorPosition = position
        }
    }

    function handleTabPressed(backtab) {
        const [blockStart, blockEnd] = MDHandler.getBlockLimits(selectionStart, selectionEnd, text)
        const chars = Config.useSpaceForTab ? " " : '\t'

        if (selectionStart !== selectionEnd) {
            const goalCharsRep = Config.useSpaceForTab ? Config.spacesForTab : 1
            const instruction = backtab ? 258 : 257 // Instructions::Remove and Instructions::Apply 

            const selectedText = getText(blockStart, blockEnd)
            const newString = MDHandler.getNewText(selectedText, chars, false, false, false, goalCharsRep, instruction)

            remove(blockStart, blockEnd)
            insert(blockStart, newString)
            select(blockStart, blockStart + newString.length)
        } else if (!backtab) {
            insert(selectionStart, Config.useSpaceForTab ? chars.repeat(Config.spacesForTab) : chars)
        }
    }
}
