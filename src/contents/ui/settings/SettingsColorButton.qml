// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami

Column {
    id: root

    readonly property alias colorButton: colorButton
    required property string name
    required property string color
    property alias text: title.text

    spacing: Kirigami.Units.smallSpacing
    padding: Kirigami.Units.largeSpacing

    Controls.Label {
        id: title
    }

    Row {
        id: colorButton

        Controls.Button {
            id: textButton

            width: Kirigami.Units.gridUnit * 7

            anchors.bottom: resetText.bottom
            anchors.top: resetText.top
            anchors.margins: Kirigami.Units.smallSpacing

            background: Rectangle {
                color: root.color
                radius: Kirigami.Units.smallSpacing
            }

            onClicked: {
                colorPicker.caller = root
                colorPicker.open()
            }
        }

        Controls.Button {
            id: resetText
            icon.name: "edit-undo"

            onClicked: updateColor(root, "None")
        }
    }
}
