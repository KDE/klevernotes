// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.2

import org.kde.Klever 1.0

ScrollView {
    id: view

    readonly property TextArea textArea: textArea

    property string path
    property string text: textArea.text
    property bool modified : false

    // Without it the background is from Theme.Window instead of Theme.View
    background: Item {}

    onPathChanged: {
        textArea.tempBuff = true ;
        textArea.text = DocumentHandler.readFile(path) ;
        modified = false ;
        textArea.tempBuff = false
    }

    TextArea{
        id: textArea

        property bool tempBuff

        font: Config.editorFont
        wrapMode: TextEdit.Wrap
        persistentSelection: true

        onTextChanged: if (!tempBuff) {
            modified = true
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
            } else if (noControl) {
                insert(selectionStart, Config.useSpaceForTab ? chars.repeat(Config.spacesForTab) : chars)
            }
        }
    }

    Timer {
        id: noteSaverTimer

        interval: 10000

        onTriggered: {
            saveNote(textArea.text, view.path)
        }
    }

    function saveNote (text, path) {
        if (modified) {
            DocumentHandler.writeFile(text, path)
            modified = false
        }
    }
}
