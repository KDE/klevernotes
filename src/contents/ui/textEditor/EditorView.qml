// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import Qt.labs.platform 1.1

import "qrc:/contents/ui/dialogs"

GridLayout{
    id: root

    readonly property QtObject imagePickerDialog: toolbar.imagePickerDialog
    readonly property TextEditor editor: editor
    required property string path
    required property string noteName

    property list<Kirigami.Action> actions: [
        Kirigami.Action {
            id: pdfPrinter

            property string path
            onPathChanged: if (path.length > 0) {
                display.makePdf(path)
                path = ""
            }

            icon.name: "viewpdf"
            onTriggered: {
                //pdfSaver.open()
                display.makePdf()
            }
        },
        Kirigami.Action {
            id: editorToggler
            checkable: true
            icon.name: checked ? "text-flow-into-frame" : "text-unflow"
            checked: true
            onCheckedChanged: if (!checked && !viewToggler.checked) checked = true
        },
        Kirigami.Action {
            id: viewToggler
            checkable: true
            icon.name: checked ? "quickview" : "view-hidden"
            checked: applicationWindow().wideScreen
            onCheckedChanged: if (!checked && !editorToggler.checked) checked = true
        }
    ]

    FileSaverDialog {
        id: pdfSaver

        caller: pdfPrinter
        noteName: root.noteName
    }

    rows: 4
    columns: 2

    TextToolBar {
        id: toolbar

        editorTextArea: root.editor.textArea
        notePath: root.path

        Layout.preferredHeight: childrenRect.height
        Layout.column: 0
        Layout.columnSpan: 2
    }

    // This item can be seen as useless but it prevent a weird bug with the height not being adjusted
    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.row: 3
        Layout.column: 0
        Layout.columnSpan: 2

        GridLayout {
            anchors.fill:parent

            columns: parent.width > Kirigami.Units.gridUnit * 30 ? 2 : 1
            rows: columns > 1 ? 1 : 2

            TextEditor{
                id: editor

                visible: editorToggler.checked

                path: root.path

                Layout.fillWidth:true
                Layout.fillHeight:true
                Layout.preferredHeight: parent.columns === 2 ? parent.height : parent.height/2
                Layout.preferredWidth: parent.columns === 2 ? parent.width/2 : parent.width
            }

            TextDisplay{
                id: display

                visible: viewToggler.checked

                text: editor.text
                path: root.path.replace("note.md", "")

                Layout.fillWidth:true
                Layout.fillHeight:true
                Layout.preferredHeight: parent.columns === 2 ? parent.height : parent.height/2
                Layout.preferredWidth: parent.columns === 2 ? parent.width/2 : parent.width
            }
        }
    }
}
