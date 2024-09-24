// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami

import org.kde.Klever

ScrollView {
    id: view

    signal openImageDialog(string image)

    readonly property TextArea textArea: textArea

    property string path
    property string text: textArea.text
    property bool modified : false

    onPathChanged: {
        EditorHandler.notePath = path
        textArea.tempBuff = true ;
        textArea.text = DocumentHandler.readFile(path);
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
            EditorHandler.document = textDocument
        }
        onCursorPositionChanged: {
            EditorHandler.cursorPosition = cursorPosition
        }
        onSelectionStartChanged: {
            EditorHandler.selectionStart = selectionStart
        }
        onSelectionEndChanged: {
            EditorHandler.selectionEnd = selectionEnd
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
            EditorHandler.handleTabPressed(backtab)
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
