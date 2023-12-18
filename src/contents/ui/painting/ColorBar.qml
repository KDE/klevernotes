// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami

import "qrc:/contents/ui/dialogs/colorDialog"

Kirigami.ShadowedRectangle {
    id: root

    readonly property var colorsList: [
        "#000000", "#808080", "#FFFFFF", "#FF0000", "#FF8000",
        "#FFFF00", "#00FF00", "#00FFFF", "#0000FF", "#FF00FF"
    ]

    property color primaryColor: "black"
    property color secondaryColor: "white"

    Kirigami.Theme.colorSet: Kirigami.Theme.Header
    Kirigami.Theme.inherit: false

    color: Kirigami.Theme.backgroundColor
    radius: Kirigami.Units.smallSpacing

    RowLayout {
        anchors.fill: parent

        spacing: Kirigami.Units.smallSpacing

        Rectangle {
            width: Kirigami.Units.gridUnit * 2.5
            height: Kirigami.Units.gridUnit * 2.5

            color: Qt.rgba(1, 1, 1, 0.2)
            radius: Kirigami.Units.smallSpacing

            Layout.leftMargin: Kirigami.Units.smallSpacing

            ColorButton {
                id: secondaryButton

                x: parent.width - width - Kirigami.Units.smallSpacing
                y: parent.height - height - Kirigami.Units.smallSpacing
                width: Kirigami.Units.gridUnit * 1.5
                height: Kirigami.Units.gridUnit * 1.5

                color: root.secondaryColor
                border {
                    color: "black"
                    width: 1
                }

                multicolor: true

                onOpenColorPicker: {
                    colorDialog.caller = secondaryButton
                    colorDialog.open()
                }
            }
            ColorButton {
                id: primaryButton

                x: Kirigami.Units.smallSpacing
                y: Kirigami.Units.smallSpacing
                width: Kirigami.Units.gridUnit * 1.5
                height: Kirigami.Units.gridUnit * 1.5

                color: root.primaryColor
                border {
                    color: "black"
                    width: 1
                }
                multicolor: true

                onOpenColorPicker: {
                    colorDialog.caller = primaryButton
                    colorDialog.open()
                }
            }
        }

        Repeater {
            model: colorsList
            ColorButton {
                color: modelData

                onPrimaryColorChanged: {
                    root.primaryColor = color
                }
                onSecondaryColorChanged: {
                    root.secondaryColor = color
                }
            }
        }
    }

    ColorDialog {
        id: colorDialog

        property QtObject caller

        selectedColor: caller ? caller.color : "black"

        onApplied: {
            colorDialog.close()
            if (caller.color === root.primaryColor) {
                root.primaryColor = selectedColor
                return
            }
            root.secondaryColor = selectedColor
        }
    }
}
