// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls 2.2

import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

TextArea {
    id: root

    required property bool vimModeOn

    readonly property int __isVisual: __currentMode === 259
    property int __currentMode: 257
    property string __cursorText

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

    cursorDelegate: Rectangle {
        id: cursorBackground
        readonly property bool isBlock: Config.cursorStyle === "block"
        color: root.__isVisual && root.vimModeOn
            ? Kirigami.Theme.highlightColor
            : isBlock 
                ? Kirigami.Theme.textColor 
                : Kirigami.Theme.backgroundColor
        width: t_metrics.tightBoundingRect.width
        height: t_metrics.tightBoundingRect.height

        Rectangle {
            id: underlineAndBeam

            y: Config.cursorStyle === "beam" 
                ? 0
                : parent.height - height
            width: Config.cursorStyle === "beam"
                ? Math.round(parent.width / 10)
                : parent.width
            height: Config.cursorStyle === "beam"
                ? parent.height
                : Math.round(parent.height / 10)
            visible: !parent.isBlock
        }

        Text {
            id: cursor_text
            text: root.__cursorText
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

    Keys.onPressed: (event) => {
        if (root.vimModeOn) {
            if (event.modifiers === Qt.ShiftModifier | event.modifiers === Qt.NoModifier) {
                event.accepted = editorHandler.handleKeyPress(event.key, event.modifiers) 
            }
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
    onCursorPositionChanged: {
        setCursorText()
    }

    EditorHandler {
        id: editorHandler

        property int visualStart
        property int visualEnd
        property bool selectLeft: false

        document: root.textDocument
        cursorPosition: root.cursorPosition
        selectionStart: root.selectionStart
        selectionEnd: root.selectionEnd

        onModeChanged: (mode) => {
            root.__currentMode = mode
            if (mode === 259) {
                visualStart = root.cursorPosition
                visualEnd = root.cursorPosition + 1 <= root.length
                    ? root.cursorPosition + 1
                    : root.cursorPosition
            }
        }
        onCursorPositionChanged: (position) => {
            root.cursorPosition = position
        }
        onMoveSelection: (position) => {
            const goLeft = position < visualStart
            if (goLeft && selectionEnd !== visualEnd) {
                root.cursorPosition = visualEnd
                editorHandler.selectLeft = true
            } else if (!goLeft && editorHandler.selectLeft) {
                root.cursorPosition = visualStart
                editorHandler.selectLeft = false
            }
            root.moveCursorSelection(position)
        }
        onDeselect: {
            root.deselect()
        }
        onCut: {
            root.cut()
            setCursorText()
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

    function setCursorText() {
        __cursorText = root.getText(root.cursorPosition, root.cursorPosition + 1)
    }
}
