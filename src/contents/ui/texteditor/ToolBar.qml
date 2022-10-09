// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.1
import org.kde.kirigami 2.19 as Kirigami

ToolBar {
    id: mainToolBar

    property var document;
    readonly property Action alignLeftAction:alignLeftAction
    readonly property Action alignCenterAction:alignCenterAction
    readonly property Action alignRightAction:alignRightAction
    readonly property Action alignJustifyAction:alignJustifyAction
    readonly property Action boldAction:boldAction
    readonly property Action italicAction:italicAction
    readonly property Action underlineAction:underlineAction
    readonly property ColorDialog colorDialog:colorDialog
    readonly property Item fontSizeSpinBox:fontSizeSpinBox

    Kirigami.Action {
        id: alignLeftAction
        icon.name: "format-justify-left"
        shortcut: "ctrl+l"
        onTriggered: document.alignment = Qt.AlignLeft
        checkable: true
        checked: document.alignment == Qt.AlignLeft
    }
    Kirigami.Action {
        id: alignCenterAction
        icon.name: "format-justify-center"
        onTriggered: document.alignment = Qt.AlignHCenter
        checkable: true
        checked: document.alignment == Qt.AlignHCenter
    }
    Kirigami.Action {
        id: alignRightAction
        icon.name: "format-justify-right"
        onTriggered: document.alignment = Qt.AlignRight
        checkable: true
        checked: document.alignment == Qt.AlignRight
    }
    Kirigami.Action {
        id: alignJustifyAction
        icon.name: "format-justify-fill"
        onTriggered: document.alignment = Qt.AlignJustify
        checkable: true
        checked: document.alignment == Qt.AlignJustify
    }

    Kirigami.Action {
        id: boldAction
        icon.name: "format-text-bold"
        onTriggered: document.bold = !document.bold
        checkable: true
        checked: document.bold
    }

    Kirigami.Action {
        id: italicAction
        icon.name: "format-text-italic"
        onTriggered: document.italic = !document.italic
        checkable: true
        checked: document.italic
    }
    Kirigami.Action {
        id: underlineAction
        icon.name: "format-text-underline"
        onTriggered: document.underline = !document.underline
        checkable: true
        checked: document.underline
    }

    ColorDialog {
        id: colorDialog
        color: "black"
    }


    RowLayout {
        anchors.fill: parent
        spacing: 0


        ToolButton { action: boldAction }
        ToolButton { action: italicAction }
        ToolButton { action: underlineAction }

        ToolButton { action: alignLeftAction }
        ToolButton { action: alignCenterAction }
        ToolButton { action: alignRightAction }
        ToolButton { action: alignJustifyAction }


        ToolButton {
            id: colorButton
            property var color : document.textColor
            Rectangle {
                id: colorRect
                anchors.fill: parent
                anchors.margins: 8
                color: colorButton.color
                border.width: 1
                border.color: Qt.darker(colorRect.color, 2)
            }
            onClicked: {
                colorDialog.color = document.textColor
                colorDialog.open()
            }
        }

        SpinBox {
            id: fontSizeSpinBox
            implicitWidth: 50
            value: 0
            property bool valueGuard: true
            onValueChanged: if (valueGuard) document.fontSize = value
        }

        Item { Layout.fillWidth: true
        }
    }
}
