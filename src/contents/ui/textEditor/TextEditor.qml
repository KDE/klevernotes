// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.2

import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

ScrollView {
    id: view

    signal openImageDialog(string image)
    signal textAreaInit(QtObject document)

    readonly property TextArea textArea: textArea

    property string path
    property string text: textArea.text
    property bool modified : false

    onPathChanged: {
        textArea.tempBuff = true ;
        textArea.text = DocumentHandler.readFile(path) ;
        modified = false ;
    }

    TextArea{
        id: textArea

        property bool tempBuff

        font: Config.editorFont
        wrapMode: TextEdit.Wrap
        persistentSelection: true

        background: Item {}

        Component.onCompleted: {
            textAreaInit(textArea.textDocument)
        }    
        onTextChanged: {
            if (!tempBuff) {
                modified = true
            } else {
                cursorPosition = length
                tempBuff = false
            }
        }
        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_V && (event.modifiers === Qt.ControlModifier || event.modifiers === Qt.ShiftModifier | Qt.ControlModifier)) {
                const tempPath = path.slice(0, -7) + "tempImage.png"
                if (!canPaste && KleverUtility.checkPaste(tempPath)) {

                    openImageDialog(tempPath)
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
