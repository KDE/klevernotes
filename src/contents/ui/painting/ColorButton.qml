// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami

AbstractButton {
    id: button

    property bool multicolor: false
    property alias color: colorRectangle.color
    property alias border: colorRectangle.border

    signal primaryColorChanged
    signal secondaryColorChanged
    signal openColorPicker

    width: Kirigami.Units.gridUnit * 1.5
    height: Kirigami.Units.gridUnit * 1.5

    background: Rectangle {
        id: colorRectangle
    }

    TapHandler {
        onTapped: {
            if (!button.multicolor) {
                button.primaryColorChanged();
                return;
            }
            button.openColorPicker();
        }
    }

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: {
            if (!button.multicolor) {
                button.secondaryColorChanged();
                return;
            }
            button.openColorPicker();
        }
    }
}
