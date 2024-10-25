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

    width: Kirigami.Units.gridUnit * 2
    height: Kirigami.Units.gridUnit * 2

    background: Kirigami.ShadowedRectangle {
        id: colorRectangle

        radius: Kirigami.Units.mediumSpacing
        shadow {
            size: Kirigami.Units.largeSpacing
            color: Qt.rgba(0.0, 0.0, 0.0, 0.3)
            yOffset: 2
        }
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
