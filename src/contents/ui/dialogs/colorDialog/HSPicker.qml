// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.11

Item {
    id: root

    property int cursorRadius : 10
    property real h : 1
    property real s : 0

    Rectangle {
//      This way the middle of the cursor can trully be in a corner
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
        id:bouderies
        x: cursorRadius
        y: cursorRadius
        width: parent.width - 2 * cursorRadius
        height: parent.height - 2 * cursorRadius
        gradient: Gradient {
            GradientStop { position: 1.0; color: "#FFFFFFFF" }
            GradientStop { position: 0.0; color: "#00000000" }
        }
    }

    Rectangle{
        id:pickerCursor
        visible: !mouseArea.containsPress
        color:"transparent"
        width: cursorRadius*2
        height: cursorRadius*2
        x: bouderies.width
        y: bouderies.height

        Rectangle{
            id:north
            color:"black"
            x: cursorRadius-1
            anchors.top: parent.top
            height: cursorRadius
            width:2
        }

        Rectangle{
            id:east
            color:"black"
            y:cursorRadius-1
            anchors.right: parent.right
            height: 2
            width:cursorRadius
        }

        Rectangle{
            id:south
            color:"black"
            x:cursorRadius-1
            anchors.bottom: parent.bottom
            height: cursorRadius
            width:2
        }

        Rectangle{
            id:west
            color:"black"
            y:cursorRadius-1
            anchors.left: parent.left
            height: 2
            width:cursorRadius
        }

    }

    MouseArea {
        id:mouseArea
        anchors.fill: bouderies

        cursorShape: (containsPress) ? Qt.CrossCursor : Qt.ArrowCursor

        function handleMouse(mouse) {
            if (mouse.buttons & Qt.LeftButton) {
                pickerCursor.x = Math.max(0, Math.min(bouderies.width,  mouse.x));
                pickerCursor.y = Math.max(0, Math.min(bouderies.height, mouse.y));

                root.h = (pickerCursor.x/bouderies.width)
                root.s = ((bouderies.height-pickerCursor.y)/bouderies.height)

                // root.h = Math.round(xPercent*3.6)
                // root.s = Math.round(yPercent)

            }
        }
        onPositionChanged: handleMouse(mouse)
        onPressed: handleMouse(mouse)
    }
}

