// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.11

Item{
    id: lightSlider
    property int cursorHeight: 7
    property real l
    Rectangle {
        id: gradientRect
        y: 10
        width: parent.width
        height: parent.height - 20


        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FFFFFFFF" }
            GradientStop { position: 1.0; color: "#FF000000" }
        }

        Item {
            id: pickerCursor
            width: parent.width
            Rectangle {
                x: -2;
                property real cal: (gradientRect.height - (gradientRect.height * lightSlider.l))

                y: Math.min(cal, gradientRect.height)

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
                    const currentY = Math.max(0, Math.min(height, mouse.y))

                    lightSlider.l = (height-currentY)/height
                }
            }
            onPositionChanged: {
                handleMouse(mouse)
            }
            onPressed: handleMouse(mouse)
        }
    }
}
