// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

GridLayout{
    id: holder

    anchors.fill:parent
    rows: 4
    columns: 2

    readonly property TextEditor editor : editor
    property string path

    property list<Kirigami.Action> actions: [
        Kirigami.Action {
            id: editorToggler
            checkable: true
            icon.name: checked ? "text-flow-into-frame" :  "text-unflow"
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

    TextToolBar{
        id:toolbar

        textArea: editor.textArea
        notePath: display.path

        Layout.fillWidth:true
        Layout.preferredHeight: childrenRect.height
        Layout.row: (root.wideScreen) ? 1 : 2
        Layout.column: 0
        Layout.columnSpan: (root.wideScreen) ? 1 : 2
    }

    // This item can be seen as useless but it prevent a weird bug with the height not being adjusted
    Item{
        Layout.fillWidth:true
        Layout.fillHeight:true
        Layout.row: 3
        Layout.column: 0
        Layout.columnSpan: 2

        GridLayout {
            anchors.fill:parent

            columns: (parent.width > Kirigami.Units.gridUnit * 30) ? 2 : 1
            rows : (columns > 1) ? 1 : 2

            TextEditor{
                id: editor

                visible: editorToggler.checked
                path: holder.path

                Layout.fillWidth:true
                Layout.fillHeight:true
                Layout.preferredHeight: (parent.columns === 2) ? parent.height : parent.height/2
                Layout.preferredWidth: (parent.columns === 2) ? parent.width/2 : parent.width
            }

            TextDisplay{
                id: display

                text: editor.text
                visible: viewToggler.checked
                path: holder.path.replace("note.md","")

                Layout.fillWidth:true
                Layout.fillHeight:true
                Layout.preferredHeight: (parent.columns === 2) ? parent.height : parent.height/2
                Layout.preferredWidth: (parent.columns === 2) ? parent.width/2 : parent.width
            }
        }
    }
}
