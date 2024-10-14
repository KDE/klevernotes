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

    required property color primaryColor
    required property color secondaryColor
    required property string mode 

    property int lineWidth: mode === "text" 
        ? sizeSpinBox.value // Replace by text...
        : sizeSpinBox.value

    padding: 0
    position: ToolBar.Header

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

        Item {
            Layout.fillWidth: true
        }

        FontPicker {
            label: i18nc("@label:textbox", "Font:")
            configFont: Config.viewFont

            visible: root.mode === "text"

            Layout.fillWidth: false
            Layout.preferredWidth: Kirigami.Units.gridUnit * 10

            onNewFontChanged: if (text !== newFont) {
            }
        }

        ToolButton {
            action: boldToggler
            visible: root.mode === "text"
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.largeSpacing + Math.round(Kirigami.Units.smallSpacing * 0.5)
        }

        ToolButton {
            action: italicToggler
            visible: root.mode === "text"
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.largeSpacing + Math.round(Kirigami.Units.smallSpacing * 0.5)
        }

        ToolButton {
            action: underlineToggler
            visible: root.mode === "text"
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.largeSpacing + Math.round(Kirigami.Units.smallSpacing * 0.5)
        }

        FormCard.FormSpinBoxDelegate {
            id: sizeSpinBox

            label: i18n("Width:")
            visible: root.mode !== "text"
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

        ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            visible: root.mode !== "draw"

            Layout.fillHeight: true
            Layout.rightMargin: Kirigami.Units.gridUnit
            Layout.leftMargin: root.mode === "text" ? Kirigami.Units.gridUnit : 0

            Label {
                text: i18n("Fill mode:")
                color: Kirigami.Theme.textColor
                elide: Text.ElideRight
            }

            ComboBox {
                model: ["Outline", "Fill", "Outline and fill"]
            }
        }

        ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            visible: root.mode !== "draw"

            Layout.rightMargin: Kirigami.Units.gridUnit

            Label {
                text: i18nc("@label, the color in used to fill the shape, can be 'Primary' or 'Secondary'", "Fill color") 
                + ": " 
                + (fillColorButton.fillColor === root.primaryColor 
                    ? i18nc("@label, as in 'the primary color'", "Primary")
                    : i18nc("@label, as in 'the secondary color'", "Secondary"))
                color: Kirigami.Theme.textColor
                elide: Text.ElideRight

                Layout.preferredWidth: Kirigami.Units.gridUnit * 8
            }

            AbstractButton {
                id: fillColorButton
                
                property bool usePrimary: true
                property color fillColor: usePrimary ? root.primaryColor : root.secondaryColor
                
                background: Rectangle { 
                    color: fillColorButton.fillColor
                    radius: Kirigami.Units.smallSpacing
                }

                width: Kirigami.Units.gridUnit * 8
                height: Math.round(Kirigami.Units.gridUnit * 1.6)

                onClicked: usePrimary = !usePrimary
            }
        }
    }
}
