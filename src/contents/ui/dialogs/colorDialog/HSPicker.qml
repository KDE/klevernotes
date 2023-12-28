// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.11

Item {
    id: root

    property real h
    property real s
    property int cursorRadius: 10

    Rectangle {
        // This way the middle of the cursor can trully be in a corner
        x : cursorRadius
        y : cursorRadius + parent.height - 2 * cursorRadius
        width: parent.height - 2 * cursorRadius
        height: parent.width - 2 * cursorRadius
        rotation: -90
        transformOrigin: Item.TopLeft
        gradient: Gradient {
            GradientStop { position: 0.0;  color: "#FF0000" }
            GradientStop { position: 0.16; color: "#FFFF00" }
            GradientStop { position: 0.33; color: "#00FF00" }
            GradientStop { position: 0.5;  color: "#00FFFF" }
            GradientStop { position: 0.76; color: "#0000FF" }
            GradientStop { position: 0.85; color: "#FF00FF" }
            GradientStop { position: 1.0;  color: "#FF0000" }
        }
    }

    Rectangle {
        id: bouderies

        x: cursorRadius
        y: cursorRadius
        width: parent.width - 2 * cursorRadius
        height: parent.height - 2 * cursorRadius

        gradient: Gradient {
            GradientStop { position: 1.0; color: "#FFFFFFFF" }
            GradientStop { position: 0.0; color: "#00000000" }
        }
    }

    Rectangle {
        id: pickerCursor

        x: root.h * bouderies.width
        y: (root.s * bouderies.height * -1) + bouderies.height
        width: cursorRadius*2
        height: cursorRadius*2

        color: "transparent"
        visible: !mouseArea.containsPress

        Rectangle {
            id: north

            x: cursorRadius - 1
            width: 2
            height: cursorRadius
            anchors.top: parent.top

            color: "black"
        }

        Rectangle {
            id: east

            y: cursorRadius - 1
            width: cursorRadius
            height: 2
            anchors.right: parent.right

            color: "black"
        }

        Rectangle {
            id: south

            x: cursorRadius - 1
            width: 2
            height: cursorRadius
            anchors.bottom: parent.bottom

            color: "black"
        }

        Rectangle {
            id: west

            y: cursorRadius - 1
            width: cursorRadius
            height: 2
            anchors.left: parent.left

            color: "black"
        }

    }

    MouseArea {
        id: mouseArea

        anchors.fill: bouderies

        cursorShape: (containsPress) ? Qt.CrossCursor : Qt.ArrowCursor

        onPressed: function (mouse) {
            handleMouse(mouse)
        }
        onPositionChanged: function (mouse) {
            handleMouse(mouse)
        }

        function handleMouse(mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                const currentX = Math.max(0, Math.min(bouderies.width,  mouse.x));
                const currentY = Math.max(0, Math.min(bouderies.height, mouse.y));

                root.h = (currentX/bouderies.width)
                root.s = ((bouderies.height-currentY)/bouderies.height)
            }
        }
    }
}
