// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.1
import org.qtproject.example 1.0

Item {
    id:root
    readonly property var document:document
    property string path

    TextArea {
        Accessible.name: "document"
        id: textArea
        anchors.fill:parent

        baseUrl: "qrc:/"
        persistentSelection: true
        text: document.text
        textFormat: Qt.RichText
    }

    MessageDialog {
        id: errorDialog
    }

    DocumentHandler {
        id: document
        target: textArea
        cursorPosition: textArea.cursorPosition
        selectionStart: textArea.selectionStart
        selectionEnd: textArea.selectionEnd
        textColor: toolbar.colorDialog.color
        Component.onCompleted: document.fileUrl = root.path
        onFontSizeChanged: {
            toolbar.fontSizeSpinBox.valueGuard = false
            toolbar.fontSizeSpinBox.value = document.fontSize
            toolbar.fontSizeSpinBox.valueGuard = true
        }

        onError: {
            errorDialog.text = message
            errorDialog.visible = true
        }
    }
}
