// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

/*
 * THIS COMPONENT AND INNER COMPONENTS ARE LOOSELY BASED ON :
 * https://github.com/rshest/qml-colorpicker/tree/master/colorpicker
 */
import QtQuick 2.11
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.4

Rectangle {
    id: colorPicker
    color: "transparent"

    property color selectedColor

    onSelectedColorChanged: if (!visible) {
        hsPicker.h = selectedColor.hslHue
        hsPicker.s = selectedColor.hslSaturation
        lightnessSlider.l = selectedColor.hslLightness
    }

    ColumnLayout{
        anchors.fill:parent
        spacing:0
        RowLayout {
            id: picker

            width: parent.width
            height:parent.height-15

            // hue and saturation
            HSPicker {
                id: hsPicker
                Layout.fillWidth:true
                Layout.fillHeight:true

                onSChanged: _hsla(h,s,lightnessSlider.l)
                onHChanged: _hsla(h,s,lightnessSlider.l)
            }

            // lightness picking slider
            LightnessSlider {
                id: lightnessSlider
                width: 12
                Layout.rightMargin: 10
                Layout.fillHeight:true

                onLChanged: _hsla(hsPicker.h,hsPicker.s,l)
            }
        }
        Rectangle{
            id:select
            Layout.preferredWidth: hsPicker.width - 20
            height: 15
            Layout.leftMargin: 10
            Layout.bottomMargin: 10
        }
    }

    //  creates color value from hue, saturation, lightness
    function _hsla(h, s, l) {
        var c = Qt.hsla(h, s, l, 1)
        select.color = c

        if (visible) selectedColor = c
    }
}
