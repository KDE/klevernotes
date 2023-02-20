// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.11

Item{
    id: lightSlider
    property int cursorHeight: 7
    property real l
    Rectangle {
        y: 10
        height: parent.height - 20
        width:parent.width


        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FFFFFFFF" }
            GradientStop { position: 1.0; color: "#FF000000" }
        }

        Item {
            id: pickerCursor
            width: parent.width
            Rectangle {
                x: -2; y: -height*0.5
                width: parent.width + 4; height: cursorHeight
                border.color: (lightSlider.l > 50) ? "black" : "white"
                border.width: 2
                color: "transparent"
            }
        }

        MouseArea{
            id:mouseArea
            anchors.fill:parent

            function handleMouse(mouse) {
                if (mouse.buttons & Qt.LeftButton) {
                    pickerCursor.y = Math.max(0, Math.min(height, mouse.y))

                    lightSlider.l = ((height-pickerCursor.y)/height)
                }
            }
            onPositionChanged: {
                handleMouse(mouse)
            }
            onPressed: handleMouse(mouse)
        }
    }
}
