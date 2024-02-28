// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15

import org.kde.kirigami 2.19 as Kirigami

Rectangle {
    id: button

    property bool multicolor: false

    signal primaryColorChanged
    signal secondaryColorChanged
    signal openColorPicker

    width: Kirigami.Units.gridUnit * 1.5
    height: Kirigami.Units.gridUnit * 1.5

    MouseArea {
        anchors.fill: parent

        enabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: function (mouse) {
            if (!button.multicolor) {
                mouse.button === Qt.LeftButton ? button.primaryColorChanged() : button.secondaryColorChanged()
                return
            }
            button.openColorPicker()
        }
    }
}
