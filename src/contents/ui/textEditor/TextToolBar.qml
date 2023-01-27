// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

Kirigami.ActionToolBar {
    id: mainToolBar

    property QtObject textArea

    function applyInstructions(info,specialChars){
        const instructions = info.instructions
        const lines = info.lines
        let end = textArea.selectionEnd
        let applied = false

        for (var i = lines.length-1 ; i >= 0; i--){
            const line = lines[i]
            const instruction = instructions[i]

            const start = end-line.length
            switch(instruction) {
            case "apply":
                textArea.insert(end,specialChars)
                textArea.insert(start,specialChars)
                applied = true
                break;
            case "remove":
                textArea.remove(end-specialChars.length,end)
                textArea.remove(start,start+specialChars.length)
                break;
            default:
                break
            }
            end = start-1
        }
        if (applied) textArea.select(textArea.selectionStart-specialChars.length,textArea.selectionEnd)
    }


    actions: [
        Kirigami.Action {
            text: "𝐇"

            Kirigami.Action {
                text: "𝐇𝟏"
                onTriggered: showPassiveNotification("DEET!")
            }
            Kirigami.Action {
                text: "𝐇𝟐"
                onTriggered: showPassiveNotification("DEET!")
            }
            Kirigami.Action {
                text: "𝐇𝟑"
                onTriggered: showPassiveNotification("DEET!")
            }
            Kirigami.Action {
                text: "𝐇𝟒"
                onTriggered: showPassiveNotification("DEET!")
            }
            Kirigami.Action {
                text: "𝐇𝟓"
                onTriggered: showPassiveNotification("DEET!")
            }
            Kirigami.Action {
                text: "𝐇𝟔"
                onTriggered: showPassiveNotification("DEET!")
            }

        },
        Kirigami.Action {
            id: boldAction
            icon.name: "format-text-bold"
            onTriggered: {
                const info = MDHandler.getInstructions(textArea.selectedText,["**","__"])

                applyInstructions(info,"**")
            }
        },
        Kirigami.Action {
            id: italicAction
            icon.name: "format-text-italic"
            onTriggered: {
                const info = MDHandler.getInstructions(textArea.selectedText,["*","_"])

                applyInstructions(info,"_")
            }
        },
        Kirigami.Action {
            id: strikethroughAction
            icon.name: "format-text-strikethrough"
            onTriggered: {
                const info = MDHandler.getInstructions(textArea.selectedText,["~~"])

                applyInstructions(info,"~~")
            }
        }
    ]
}
/*
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
}*/
