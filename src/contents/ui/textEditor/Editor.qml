// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

ColumnLayout{
    property alias title: header.text
    property alias path: display.path
    anchors.fill:parent

    Kirigami.Heading {
        id:header
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        type: Kirigami.Heading.Secondary
    }

    TextToolBar{
        id:toolbar
        document:display.document
        Layout.fillWidth:true
    }

    TextDisplay{
        id: display

        Layout.fillWidth:true
        Layout.fillHeight:true
    }
}
