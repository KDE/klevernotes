// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import "qrc:/contents/ui/settings"

import org.kde.Klever

ToolBar {
    id: root

    enum Tool {
        Pen,
        Eraser,
        Text,
        Rectangle,
        Circle
    }

    property int selectedTool: DrawingToolBar.Tool.Pen
    property int lineWidth: selectedTool === DrawingToolBar.Tool.Text 
        ? sizeSpinBox.value // Replace by text...
        : sizeSpinBox.value

    position: ToolBar.Header

    ActionGroup {
        id: mainToolsGroup 

        Kirigami.Action {
            id: penToggler

            checked: true
            checkable: true
            icon.name: "draw-brush-symbolic"
            
            onTriggered: {
                root.selectedTool = DrawingToolBar.Tool.Pen
            }
        }

        Kirigami.Action {
            id: eraserToggler

            checkable: true
            icon.name: "draw-eraser-symbolic"
            
            onTriggered: {
                root.selectedTool = DrawingToolBar.Tool.Eraser
            }
        }

        Kirigami.Action {
            id: textToggler

            checkable: true
            icon.name: "text-field-symbolic"
            
            onTriggered: {
                root.selectedTool = DrawingToolBar.Tool.Text
            }
        }

        Kirigami.Action {
            id: rectToggler

            checkable: true
            icon.name: "draw-rectangle-symbolic"
            
            onTriggered: {
                root.selectedTool = DrawingToolBar.Tool.Rectangle
            }
        }

        Kirigami.Action {
            id: circleToggler

            checkable: true
            icon.name: "draw-circle-symbolic"
            
            onTriggered: {
                root.selectedTool = DrawingToolBar.Tool.Circle
            }
        }
    }

    ActionGroup {
        id: textFormatGroup

        exclusive: false

        Kirigami.Action {
            id: boldToggler

            checkable: true
            icon.name: "format-text-bold-symbolic"
            
            onTriggered: {
            }
        }
        
        Kirigami.Action {
            id: italicToggler

            checkable: true
            icon.name: "format-text-italic-symbolic"
            
            onTriggered: {
            }
        }

        Kirigami.Action {
            id: underlineToggler

            checkable: true
            icon.name: "format-text-underline-symbolic"
            
            onTriggered: {
            }
        }
    }

    contentItem: RowLayout {
        spacing: 0

        ToolButton {
            action: penToggler
        }

        ToolButton {
            action: eraserToggler
        }

        ToolButton {
            action: textToggler
        }

        ToolButton {
            action: rectToggler
        }

        ToolButton {
            action: circleToggler
        }

        Kirigami.Separator {
            Layout.fillHeight: true
        }

        Item { // Spacer
            Layout.fillWidth: true
        }

        FontPicker {
            label: i18nc("@label:textbox", "Font:")
            configFont: Config.viewFont

            visible: textToggler.checked

            Layout.fillWidth: false
            Layout.preferredWidth: Kirigami.Units.gridUnit * 10

            onNewFontChanged: if (text !== newFont) {
            }
        }

        ToolButton {
            action: boldToggler
            visible: textToggler.checked
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing / 2
        }

        ToolButton {
            action: italicToggler
            visible: textToggler.checked
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing / 2
        }

        ToolButton {
            action: underlineToggler
            visible: textToggler.checked
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing / 2
        }

        FormCard.FormSpinBoxDelegate {
            id: sizeSpinBox

            label: i18n("Width:")
            visible: !textToggler.checked
            from: 1
            to: 30
            value: 5
            stepSize: 1
            textFromValue: function(value, locale) {
                return `${sizeSpinBox.value}px`
            }
            Layout.fillWidth: false
            Layout.preferredWidth: Kirigami.Units.gridUnit * 10
        }

        Item { // Spacer
            Layout.preferredWidth: Kirigami.Units.gridUnit
            visible: !penToggler.checked && !eraserToggler.checked
        }

        ColumnLayout {
            visible: !penToggler.checked && !eraserToggler.checked
            spacing: Kirigami.Units.smallSpacing
            Layout.preferredWidth: Kirigami.Units.gridUnit * 4
            Layout.rightMargin: Kirigami.Units.gridUnit
            Label {
                text: i18n("Fill mode:")
                color: Kirigami.Theme.textColor
            }

            ComboBox {
                model: ["Outline", "Fill", "Outline and fill"]
            }
        }
    }
}
