// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami

import org.kde.Klever

import "qrc:/contents/ui/textEditor/components/"

ScrollView {
    id: view

    signal openImageDialog(string image)

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
