// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

ColumnLayout{
    property alias title: header.text
    property alias path: editor.path
    anchors.fill:parent

    Kirigami.Heading {
        id:header
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        type: Kirigami.Heading.Secondary
    }

    TextToolBar{
        id:toolbar
        textArea: editor.textArea

        Layout.fillWidth:true
        Layout.preferredHeight: childrenRect.height
    }

    // This item can be seen as useless but it prevent a weird bug with the height not being adjusted
    Item{
        Layout.fillWidth:true
        Layout.fillHeight:true
        GridLayout {
            anchors.fill:parent

            columns: (parent.width > Kirigami.Units.gridUnit * 30) ? 2 : 1
            rows : (columns > 1) ? 1 : 2

            TextEditor{
                id: editor
                Layout.preferredHeight: (parent.columns === 2) ? parent.height : parent.height/2
                Layout.preferredWidth: (parent.columns === 2) ? parent.width/2 : parent.width
            }

            TextDisplay{
                id: display
                text: editor.text
                Layout.preferredHeight: (parent.columns === 2) ? parent.height : parent.height/2
                Layout.preferredWidth: (parent.columns === 2) ? parent.width/2 : parent.width
            }
        }
    }
}
