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

    readonly property date today: new Date()
    readonly property date reminderDate: new Date(reminder)

    required property int index
    required property bool todoChecked
    required property string todoTitle
    required property string todoDesc
    required property string reminder
    required property bool visibleSep

    width: ListView.view.width

    maximumWidth: Kirigami.Units.gridUnit * 40
    padding: 0
    spacing: Kirigami.Units.smallSpacing

    Component.onCompleted: {
        setDateRelativeInfos()
    }

    Delegates.RoundedItemDelegate {
        Layout.fillWidth: true

        onClicked: {
            checkbox.checked = !checkbox.checked
        }

        contentItem: RowLayout {
            spacing: Kirigami.Units.smallSpacing

            // Make the ToolTip work more consistantly
            Controls.Button {
                id: buttonTest

                property color iconColor: "#26AD5F"
                property string toolTipInfo: ""
                flat: true
                visible: 0 < reminder.length
                icon {
                    source: "view-calendar-symbolic"
                    color: iconColor
                }
                Controls.ToolTip.text: toolTipInfo
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                Controls.ToolTip.visible: hovered
            }

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
                    todoDialog.showReminder = 0 < reminder.length
                    todoDialog.value = reminderDate 
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

    function setDateRelativeInfos() {
        // #D94453, #F67300, #26AD5F // Color coming from 'data-error/warning/succes" icon
        const diff = root.reminderDate - root.today
        const absDiff = Math.abs(diff)
        const mins = Math.floor(absDiff / (1000 * 60))
        const hours = Math.floor(mins / 60)
        const days = Math.floor(hours / 24)
        if (diff < 0) {
            // Overdue
            buttonTest.iconColor = "#D94453"
            if (30 < days) { // Completly arbitrary, but at this points, who cares ?
                buttonTest.toolTipInfo = i18nc("@tooltip:button", "This task is long overdue")
                return
            }
            if (0 < days) {
                buttonTest.toolTipInfo = i18ncp("@tooltip:button", "This task is 1 day overdue", "This task is %1 days overdue", days)
                return
            }
            if (0 < hours) {
                buttonTest.toolTipInfo = i18ncp("@tooltip:button", "This task is 1 hour overdue", "This task is %1 hours overdue", hours)
                return
            }
            if (15 < mins) { // Still arbitrary
                buttonTest.toolTipInfo = i18ncp("@tooltip:button", "This task is 1 minute overdue", "This task is %1 minutes overdue", mins)
                return 
            }
        } else if (0 < diff) {
            buttonTest.iconColor = "#26AD5F"
            if (2 < days) {
                buttonTest.toolTipInfo = i18ncp("@tooltip:button", "This task is scheduled in 1 day", "This task is scheduled in %1 days", days)
                return
            }

            buttonTest.iconColor = "#F67300" 
            if (0 < days) {
                buttonTest.toolTipInfo = i18ncp("@tooltip:button", "This task is scheduled in 1 day", "This task is scheduled in %1 days", days)
                return
            }
            if (0 < hours) {
                buttonTest.toolTipInfo = i18ncp("@tooltip:button", "This task is scheduled in 1 hour", "This task is scheduled in %1 hours", hours)
                return
            }
            if (0 < mins) {
                buttonTest.toolTipInfo = i18ncp("@tooltip:button", "This task is scheduled in 1 minute", "This task is scheduled in %1 minutes", mins)
                return 
            }
        }
        buttonTest.iconColor = "#D94453"
        buttonTest.toolTipInfo = i18nc("@tooltip:button", "This task is barely overdue")
    }
}
