// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

Kirigami.AbstractCard {
    id: card

    padding: 0
    implicitWidth: cardsView.width

    contentItem: RowLayout {
        spacing: 0
        // Tried to used a FormCheckDelegate with a custom trailing, doesn't work :/
        FormCard.AbstractFormDelegate {
            id: check

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: parent.width - editButton.width
            Layout.alignment: Qt.AlignTop
            Layout.margins: 0

            contentItem: RowLayout {
                Controls.CheckBox {
                    id: checkbox

                    checked: todoChecked
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                    onCheckedChanged: {
                        todoModel.setProperty(index, "todoChecked", checked)
                        root.saveTodos()
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Kirigami.Heading {
                        id: displayTitle

                        text: todoTitle
                        level: 1
                        elide: Text.ElideRight

                        Layout.fillWidth: true
                    }

                    Kirigami.Separator {
                        id: centralSeperator

                        Layout.fillWidth: true
                    }

                    Controls.Label {
                        id: descriptionLabel

                        text: todoDesc
                        elide: Text.ElideRight
                        wrapMode: Text.WrapAnywhere
                        maximumLineCount: 1

                        // Both are required the center part of the whole delegate goes crazy without this
                        Layout.fillWidth: true

                        Behavior on maximumLineCount {
                            NumberAnimation { duration: Kirigami.Units.shortDuration }
                        }
                    }
                }
            }

            onClicked: {
                checkbox.checked = !checkbox.checked
            }
        }

        FormCard.AbstractFormDelegate {
            id: editButton

            Layout.fillHeight: true
            Layout.preferredWidth: Kirigami.Units.iconSizes.large

            // Can't put the icon directly, won't let me change the size
            contentItem: Item {
                anchors.fill: parent
                Kirigami.Icon {
                    x: width / 2
                    y: Math.round((parent.height - width) / 2)
                    width: Math.round(parent.width / 2)
                    height: Math.round(parent.width / 2)

                    source: "edit-entry"
                }
            }
        }
    }

    footer: FormCard.AbstractFormDelegate {
        focusPolicy: Qt.StrongFocus
        visible: descriptionLabel.truncated || descriptionLabel.maximumLineCount > 1

        contentItem: RowLayout {
            spacing: 0

            FormCard.FormArrow {
                direction: descriptionLabel.truncated ? Qt.DownArrow : Qt.UpArrow
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }

        Accessible.onPressAction: action ? action.trigger() : root.clicked()

        onClicked: {
            descriptionLabel.maximumLineCount = descriptionLabel.truncated
                ? 12
                : 1
        }
    }
}
