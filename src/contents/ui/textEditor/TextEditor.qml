// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.2

import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

import "qrc:/contents/ui/textEditor/components/"

ScrollView {
    id: view

    readonly property TextArea textArea: textArea

    property string path
    property string text: textArea.text
    property bool modified : false

    onPathChanged: {
        textArea.tempBuff = true ;
        textArea.text = DocumentHandler.readFile(path) ;
        modified = false ;
    }

    ImprovedTextArea {
        id: textArea

        property bool tempBuff

        vimModeOn: true

        onTextChanged: {
            if (!tempBuff) {
                modified = true
            } else {
                cursorPosition = length
                tempBuff = false
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
