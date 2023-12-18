// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

/*
 * THIS COMPONENT AND INNER COMPONENTS ARE LOOSELY BASED ON :
 * https://github.com/rshest/qml-colorpicker/tree/master/colorpicker
 */

import QtQuick 2.11
import QtQuick.Layouts 1.1

Rectangle {
    id: colorPicker

    property color selectedColor

    color: "transparent"

    onSelectedColorChanged: if (!visible) {
        hsPicker.h = selectedColor.hslHue
        hsPicker.s = selectedColor.hslSaturation
        lightnessSlider.l = selectedColor.hslLightness
    }

    ColumnLayout {
        spacing: 0
        anchors.fill: parent

        RowLayout {
            id: picker

            width: parent.width
            height: parent.height - 15

            // hue and saturation
            HSPicker {
                id: hsPicker

                Layout.fillWidth: true
                Layout.fillHeight: true

                onSChanged: {
                    _hsla(h,s,lightnessSlider.l)
                }
                onHChanged: {
                    _hsla(h,s,lightnessSlider.l)
                }
            }

            // lightness picking slider
            LightnessSlider {
                id: lightnessSlider

                width: 12

                Layout.fillHeight: true
                Layout.rightMargin: 10

                onLChanged: {
                    _hsla(hsPicker.h,hsPicker.s,l)
                }
            }
        }

        Rectangle {
            id: select

            height: 15

            Layout.leftMargin: 10
            Layout.bottomMargin: 10
            Layout.preferredWidth: hsPicker.width - 20
        }
    }

    //  creates color value from hue, saturation, lightness
    function _hsla(h, s, l) {
        var c = Qt.hsla(h, s, l, 1)
        select.color = c

        if (visible) selectedColor = c
    }
}
