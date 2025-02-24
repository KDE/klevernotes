// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import QtQuick.Dialogs as QtDialogs

import org.kde.iconthemes as KIconThemes
import org.kde.kirigamiaddons.components as Components
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigami as Kirigami

FormCard.FormCardDialog {
    id: root

    property string itemTitle
    property alias iconName: currentIcon.source
    property alias color: colorButton.color

    title: xi18nc("@title", "<filename>%1</filename> properties", itemTitle)
    implicitWidth: Kirigami.Units.gridUnit * 16
    standardButtons: Kirigami.Dialog.Apply | Kirigami.Dialog.Cancel

    enum ResetValue {
        Icon,
        Color
    }
    signal resetProperty(val: int)

    RowLayout {
        spacing: 0
        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.fillWidth: true
        FormCard.AbstractFormDelegate {
            onClicked: root.resetProperty(ItemPropertiesDialog.ResetValue.Icon)
            contentItem: Kirigami.Icon {
                source: "edit-undo-symbolic"
                implicitWidth: Kirigami.Units.iconSizes.small
                implicitHeight: Kirigami.Units.iconSizes.small
            }
            Layout.preferredWidth: Kirigami.Units.gridUnit * 4
            Layout.preferredHeight: Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing
        }

        FormCard.AbstractFormDelegate {
            onClicked: iconDialog.open()

            Layout.preferredWidth: Kirigami.Units.gridUnit * 12
            Layout.preferredHeight: Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing
            contentItem: RowLayout {
                spacing: 0

                Kirigami.Icon {
                    id: currentIcon
                    Layout.rightMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                    implicitWidth: colorButton.icon.width
                    implicitHeight: colorButton.icon.height
                }

                Controls.Label {
                    text: i18nc("@action:button", "Icon")
                    Layout.fillWidth: true
                }

                FormCard.FormArrow {
                    Layout.leftMargin: Kirigami.Units.smallSpacing
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    direction: Qt.RightArrow
                }
            }
        }
    }

    FormCard.FormDelegateSeparator {
        Layout.fillWidth: true
    }

    RowLayout {
        spacing: 0
        Layout.fillWidth: true
        FormCard.AbstractFormDelegate {
            onClicked: root.resetProperty(ItemPropertiesDialog.ResetValue.Color)
            contentItem: Kirigami.Icon {
                source: "edit-undo-symbolic"
                implicitWidth: Kirigami.Units.iconSizes.small
                implicitHeight: Kirigami.Units.iconSizes.small
            }
            Layout.preferredWidth: Kirigami.Units.gridUnit * 4
            Layout.preferredHeight: Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing
        }

        FormCard.AbstractFormDelegate {
            id: colorButton

            property string color: "transparent"

            icon.name: "color-picker"
            onClicked: colorDialog.open()

            text: i18ndc("kirigami-addons6", "@action:button", "Color")
            Accessible.description: i18ndc("kirigami-addons6", "Accessible description", "Current color \"%1\"", color)

            Layout.preferredWidth: Kirigami.Units.gridUnit * 12
            Layout.preferredHeight: Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing
            contentItem: RowLayout {
                spacing: 0

                Kirigami.Icon {
                    source: "color-picker"
                    Layout.rightMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                    implicitWidth: colorButton.icon.width
                    implicitHeight: colorButton.icon.height
                }

                Controls.Label {
                    Layout.fillWidth: true
                    text: colorButton.text
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    maximumLineCount: 2
                    Accessible.ignored: true // base class sets this text on root already
                }

                Rectangle {
                    id: colorRect
                    radius: height
                    color: colorButton.color
                    Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
                    Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
                    Layout.rightMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                }

                FormCard.FormArrow {
                    Layout.leftMargin: Kirigami.Units.smallSpacing
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    direction: Qt.RightArrow
                    visible: root.background.visible
                }
            }
        }
    }

    KIconThemes.IconDialog {
        id: iconDialog
        onIconNameChanged: root.iconName = iconName
    }

    QtDialogs.ColorDialog {
        id: colorDialog
        selectedColor: root.color
        onAccepted: {
            root.color = selectedColor
        }
    }
}

