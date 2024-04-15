// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.delegates as Delegates

Kirigami.FlexColumn {
    id: root

    signal saveTodos()

    required property int index
    required property bool todoChecked
    required property string todoTitle
    required property string todoDesc
    required property bool visibleSep

    width: ListView.view.width

    maximumWidth: Kirigami.Units.gridUnit * 40
    padding: 0
    spacing: Kirigami.Units.smallSpacing

    Delegates.RoundedItemDelegate {
        Layout.fillWidth: true

        onClicked: {
            checkbox.checked = !checkbox.checked
        }

        contentItem: RowLayout {
            spacing: Kirigami.Units.smallSpacing

            Controls.CheckBox {
                id: checkbox

                checked: todoChecked
                Layout.alignment: root.todoDesc.length > 0 ? Qt.AlignTop : Qt.AlignVCenter

                onCheckedChanged: {
                    todoModel.setProperty(index, "todoChecked", checked)
                    root.saveTodos()
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Kirigami.Heading {
                    id: displayTitle

                    text: root.todoTitle
                    elide: Text.ElideRight
                    level: 2

                    Layout.fillWidth: true
                }

                Controls.Label {
                    id: descriptionLabel

                    text: root.todoDesc
                    elide: Text.ElideRight
                    wrapMode: Text.WrapAnywhere
                    maximumLineCount: 1
                    visible: text.length > 0

                    // Both are required the center part of the whole delegate goes crazy without this
                    Layout.fillWidth: true

                    Behavior on maximumLineCount {
                        NumberAnimation { duration: Kirigami.Units.shortDuration }
                    }
                }
            }

            Controls.ToolButton {
                enabled: descriptionLabel.truncated || descriptionLabel.maximumLineCount > 1
                opacity: enabled ? 1 : 0
                icon.name: checked ? "arrow-up" : "arrow-down"
                checkable: true
                onClicked: {
                    descriptionLabel.maximumLineCount = descriptionLabel.truncated ? 12 : 1;
                }

                Layout.alignment: Qt.AlignTop
            }

            Controls.ToolButton {
                icon.name: "edit-entry"
                onClicked: {
                    todoDialog.callerModelIndex = index
                    todoDialog.name = displayTitle.text
                    todoDialog.description = descriptionLabel.text
                    todoDialog.open()
                }

                Layout.alignment: Qt.AlignTop
            }
        }
    }

    Kirigami.Separator {
        id: sep
        visible: root.visibleSep
        Layout.fillWidth: true
        Layout.bottomMargin: Kirigami.Units.smallSpacing
    }
}
