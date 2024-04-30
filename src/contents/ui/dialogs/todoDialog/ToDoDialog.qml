// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import "qrc:/contents/ui/sharedComponents"

FormCard.FormCardDialog {
    id: promptDialog

    property int callerModelIndex: -1
    property alias name: nameField.text
    property alias description: descriptionField.text
    property alias initialValue: calendar.initialValue
    property alias showReminder: calendarSwitch.checked
    readonly property string reminder: calendarSwitch.checked ? calendar.value.toUTCString() : ""

    title: i18nc("@title:dialog", "Add Todo")
    width: Kirigami.Units.gridUnit * 20

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    FormCard.FormTextFieldDelegate {
        id: nameField

        label: i18nc("@label:textbox", "Title:")
        maximumLength: 40
        placeholderText: i18nc("@placeholderText:textbox", "Todo title (required)")

        onAccepted: {
            descriptionField.forceActiveFocus()
        }
    }

    FormCard.FormDelegateSeparator {}

    TextAreaDelegate {
        id: descriptionField

        label: i18nc("@label:textbox", "Description:")
        wrapMode: TextEdit.WrapAnywhere
        maximumLength: 240
        placeholderText: i18nc("@placeholderText:textbox", "Optional")

        Layout.preferredHeight: Kirigami.Units.gridUnit * 8
    }

    ExpandingFormSwitch {
        id: calendarSwitch
        text: i18nc("@label:switch", "Set a reminder")
        FormCard.FormDateTimeDelegate {
            id: calendar
        }
    }
}
