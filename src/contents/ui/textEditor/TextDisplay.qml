// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.1
import org.qtproject.example 1.0
/*
Item{
    id:holder
    readonly property var document:document
    property string path

    GridLayout{
        anchors.fill : parent
        columns: (parent.width > 600) ? 2 : 1
        rows : (columns > 1) ? 1 : 2
        TextArea{
            // Accessible.name: "document"
            id: textArea

            persistentSelection: true
            // text: document.text

            Layout.preferredHeight: (parent.columns === 2) ? parent.height : parent.height/2
            Layout.preferredWidth: (parent.columns === 2) ? parent.width/2 : parent.width

            wrapMode: TextEdit.WrapAnywhere
        }

        TextArea{
            // Accessible.name: "document"
            id: viewArea

            text: textArea.text
            textFormat: Qt.MarkdownText
            Layout.preferredHeight: (parent.columns === 2) ? parent.height : parent.height/2
            Layout.preferredWidth: (parent.columns === 2) ? parent.width/2 : parent.width
            readOnly: true
            wrapMode: TextEdit.WrapAnywhere
        }
    }
}
*/

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
