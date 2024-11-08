// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import "qrc:/contents/ui/settings"
import "qrc:/contents/ui/dialogs"

import org.kde.Klever

ToolBar {
    id: root

    required property color primaryColor
    required property color secondaryColor
    required property string toolMode 

    readonly property int lineWidth: sizeSpinBox.value
    readonly property bool isBold: boldToggler.checked
    readonly property bool isItalic: italicToggler.checked
    readonly property string shapeStyle: shapeStyleCombobox.currentValue
    readonly property string fontFamily: fontPicker.fontFamily
    readonly property int fontPointSize: fontPicker.fontPointSize
    readonly property color fillColor: fillColorButton.fillColor

    padding: 0
    position: ToolBar.Header

    FontPickerDialog {
        id: fontDialog

        onAccepted: {
            caller.newFont = fontDialog.selectedFont
            fontDialog.close()
        }
    }

    contentItem: RowLayout {
        spacing: Kirigami.Units.largeSpacing

        Item {
            Layout.fillWidth: true
        }

        FormCard.FormSpinBoxDelegate {
            id: sizeSpinBox

            label: i18n("Width:")
            from: 1
            to: 30
            value: 5
            stepSize: 1
            textFromValue: function(value, locale) {
                return `${sizeSpinBox.value}px`
            }

            horizontalPadding: 0
            Layout.fillWidth: false
            Layout.fillHeight: true
            Layout.preferredWidth: Kirigami.Units.gridUnit * 7
            Layout.rightMargin: root.toolMode === "draw" || root.toolMode === "erase" ? Kirigami.Units.largeSpacing : 0
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }

        FontPicker {
            id: fontPicker

            label: i18nc("@label:textbox", "Font:")
            configFont: Config.viewFont

            visible: root.toolMode === "text"

            horizontalPadding: 0
            Layout.fillWidth: false
            Layout.preferredWidth: Kirigami.Units.gridUnit * 7

            Component.onCompleted: {
                fontDialog.caller = fontPicker
            }
            onNewFontChanged: if (configFont !== newFont) {
                configFont = newFont
            }
        }

        ToolButton {
            id: boldToggler

            checkable: true
            icon.name: "format-text-bold-symbolic"
            visible: root.toolMode === "text"
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
        }

        ToolButton {
            id: italicToggler

            checkable: true
            icon.name: "format-text-italic-symbolic"
            visible: root.toolMode === "text"
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
        }

        ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            visible: root.toolMode !== "draw"

            Layout.fillHeight: true
            Layout.preferredWidth: Kirigami.Units.gridUnit * 7

            Label {
                text: i18n("Fill toolMode:")
                color: Kirigami.Theme.textColor
                elide: Text.ElideRight
            }

            ComboBox {
                id: shapeStyleCombobox
                textRole: "text"
                valueRole: "value"
                model: [
                    { value: "fill", text: i18nc("@combobox:entry, shape style", "Fill") },
                    { value: "outline", text: i18nc("@combobox:entry, shape style", "Outline") },
                    { value: "both", text: i18nc("@combobox:entry, shape style", "Outline and fill") }
                ]
            }
        }

        ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            visible: root.toolMode !== "draw"

            Layout.preferredWidth: Kirigami.Units.gridUnit * 7
            Layout.rightMargin: Kirigami.Units.largeSpacing

            Label {
                text: i18nc("@label, the color in used to fill the shape, can be 'Primary' or 'Secondary'", "Fill color") 
                + ": " 
                + (fillColorButton.fillColor === root.primaryColor 
                    ? i18nc("@label, as in 'the primary color'", "Primary")
                    : i18nc("@label, as in 'the secondary color'", "Secondary"))
                color: Kirigami.Theme.textColor
                elide: Text.ElideRight

                Layout.preferredWidth: Kirigami.Units.gridUnit * 7
            }

            AbstractButton {
                id: fillColorButton
                
                property bool usePrimary: true
                property color fillColor: usePrimary ? root.primaryColor : root.secondaryColor
                
                background: Rectangle { 
                    color: fillColorButton.fillColor
                    radius: Kirigami.Units.smallSpacing
                }

                width: Kirigami.Units.gridUnit * 7
                height: Math.round(Kirigami.Units.gridUnit * 1.6)

                onClicked: usePrimary = !usePrimary
            }
        }
    }
}
