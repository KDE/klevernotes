// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.2

import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

TextArea {
    id: root

    required property bool vimModeOn

    // Enum wont work, don't know why
    property var mode: {
        "Normal": 1,
        "Insert": 2,
        "Visual": 3,
    }

    property int __currentMode: mode.Normal

    font: Config.editorFont
    wrapMode: TextEdit.Wrap
    persistentSelection: true

    // NOTE: temporary solution
    background: Rectangle {
        border.color: switch (__currentMode) {
            case 1:
                "#8CB039"
                break
            case 2:
                "#3B9DEF"
                break
            case 3:
                "#D7A656"
                break                
        }
        border.width: 3
        color: "transparent"
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

    VimHandler {
        textArea: root
        vimOn: root.vimModeOn
    }
}
