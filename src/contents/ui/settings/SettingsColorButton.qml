// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami


import org.kde.kirigamiaddons.formcard 1.0 as FormCard

FormCard.AbstractFormDelegate {
    id: root

    readonly property alias colorButton: colorButton
    required property string name
    required property string color
    property alias title: title.text

    onClicked: callPicker()

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

                Layout.fillWidth: true
                Layout.preferredHeight: holder.height / 2

                background: Rectangle {
                    color: root.color
                    radius: Kirigami.Units.smallSpacing
                }

                onClicked: callPicker()
            }

            Controls.Button {
                id: resetText
                icon.name: "edit-undo"

                onClicked: updateColor(root, "None")
            }
        }
    }

    function callPicker() {
        colorPicker.caller = root
        colorPicker.open()
    }
}
