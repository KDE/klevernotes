// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs as QtDialogs

import org.kde.kirigami as Kirigami

ToolBar {
    id: root

    readonly property var colorsList: [
        "#191A1C", "#FFFFFF", "#DD324C", "#F4885E",
        "#F8E16E", "#94CE91", "#3699CB", "#9555A1"
    ]

    property color primaryColor: "#191A1C"
    property color secondaryColor: "#FFFFFF"
    property color pickerColor: caller ? caller.color : primaryColor
    property var caller

    Kirigami.Theme.colorSet: Kirigami.Theme.Header
    Kirigami.Theme.inherit: false

    position: ToolBar.Footer

    contentItem: RowLayout {
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
                    root.caller = secondaryButton;
                    callPicker()
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
                    root.caller = primaryButton;
                    callPicker()
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

    Component {
        id: colorWindowComponent

        Window { // QTBUG-119055
            id: window
            width: Kirigami.Units.gridUnit * 19
            height: Kirigami.Units.gridUnit * 23
            maximumWidth: width
            maximumHeight: height
            minimumWidth: width
            minimumHeight: height
            visible: true
            QtDialogs.ColorDialog {
                id: colorDialog
                selectedColor: root.pickerColor
                onAccepted: {
                    if (caller.color === root.primaryColor) {
                        root.primaryColor = selectedColor;
                    } else {
                        root.secondaryColor = selectedColor;
                    }

                    window.close();
                }
                onRejected: window.close()
            }
            onClosing: {
                destroy()
                root.caller = undefined
            }
            Component.onCompleted: colorDialog.open()
        }
    }

    function callPicker() {
        colorWindowComponent.createObject(root)
    }
}
