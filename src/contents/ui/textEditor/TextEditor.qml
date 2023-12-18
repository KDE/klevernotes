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
