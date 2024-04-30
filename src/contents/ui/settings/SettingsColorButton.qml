// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import QtQuick.Dialogs as QtDialogs

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.AbstractFormDelegate {
    id: root

    readonly property alias colorButton: colorButton
    required property string name

    property string color
    property alias title: title.text

    contentItem: ColumnLayout {
        id: holder

        Controls.Label {
            id: title

            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
        }

        RowLayout {
            id: colorButton

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: Kirigami.Units.smallSpacing

            Controls.Button {
                id: textButton

                background: Rectangle {
                    color: root.color
                    radius: Kirigami.Units.smallSpacing
                }

                Layout.fillWidth: true
                Layout.preferredHeight: holder.height / 2

                onClicked: {
                    callPicker()
                }
            }

            Controls.Button {
                id: resetText

                icon.name: "edit-undo-symbolic"

                onClicked: {
                    updateColor(root, "None")
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
                selectedColor: root.color
                onAccepted: {
                    updateColor(root, selectedColor)
                    window.destroy();
                }
                onRejected: window.destroy()
            }
            onClosing: destroy()
            Component.onCompleted: colorDialog.open()
        }
    }

    onClicked: {
        callPicker()
    }

    function callPicker() {
        colorWindowComponent.createObject(root)
    }
}
