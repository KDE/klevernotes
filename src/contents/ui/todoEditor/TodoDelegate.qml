// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

Kirigami.AbstractCard {
    id: card

    width: parent.width

    contentItem: Item {
        id: holder

        implicitWidth: parent.width
        implicitHeight: Kirigami.Units.iconSizes.large

        RowLayout {
            id: delegateLayout

            anchors.fill: parent

            Controls.CheckBox {
                id: check

                checked: todoChecked
                Layout.alignment: Qt.AlignVCenter

                onCheckedChanged: {
                    todoModel.setProperty(index, "todoChecked", checked)
                    root.saveTodos()
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Kirigami.Heading {
                    id: displayTitle

                    text: todoTitle
                    level: 2
                    elide: Text.ElideRight

                    Layout.fillWidth: true
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                }
                Kirigami.Separator {
                    visible: descriptionLabel.text.length > 0

                    Layout.fillWidth: true
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                }

                ColumnLayout {
                    id: expendable

                    spacing: 0

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 0

                    Controls.Label {
                        id: descriptionLabel

                        text: todoDesc
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.margins: 0
                    }
                }
            }

            Controls.Button {
                id: editButton

                icon.name: "document-edit"
                icon.height: Kirigami.Units.iconSizes.small

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                onClicked: {
                    todoDialog.callerModelIndex = index
                    todoDialog.name = displayTitle.text
                    todoDialog.description = descriptionLabel.text
                    todoDialog.open()
                }
            }
        }
    }

    footer: Controls.Button{
        id: expend

        height: Kirigami.Units.iconSizes.small * 2

        icon.name: descriptionLabel.implicitHeight > expendable.height ? "expand" : "collapse"
        icon.height: Kirigami.Units.iconSizes.small

        visible: (descriptionLabel.implicitHeight - 10 > expendable.height ||
        holder.implicitHeight > Kirigami.Units.iconSizes.large)

        onClicked: {
            holder.implicitHeight = holder.implicitHeight == Kirigami.Units.iconSizes.large
                ? delegateLayout.implicitHeight
                : Kirigami.Units.iconSizes.large
        }
    }
}
