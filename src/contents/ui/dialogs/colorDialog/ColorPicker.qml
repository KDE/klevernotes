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

    property string selectedColor : "#ffffff"

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

                onSChanged: _hsva(h,s,valueSlider.v)
                onHChanged: _hsva(h,s,valueSlider.v)
            }

            // value picking slider
            ValueSlider {
                id: valueSlider
                width: 12
                Layout.rightMargin: 10
                Layout.fillHeight:true

                onVChanged: _hsva(hsPicker.h,hsPicker.s,v)
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

    //  creates color value from hue, saturation, brightness
    function _hsva(h, s, b) {
        var c = Qt.hsva(h, s, b,1)
        select.color = c

        selectedColor = c
    }
}
