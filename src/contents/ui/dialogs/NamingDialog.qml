// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.Klever 1.0

Kirigami.PromptDialog {
    id: textPromptDialog

    readonly property QtObject nameField: nameField

    property bool newItem
    property string useCase
    property string shownName
    property string parentPath
    property QtObject callingAction
    property alias textFieldText: nameField.text

    title: i18nc("@title:dialog", "Choose a name")

    standardButtons: Kirigami.Dialog.Apply | Kirigami.Dialog.Cancel

    onApplied: {
        const error = checkName()
        if (error === "") {
            textPromptDialog.close()
            callingAction.name = nameField.text.trim()
            return
        }
        throwError(error)
        nameField.forceActiveFocus()
    }
    onOpened: {
        nameField.forceActiveFocus()
    }
    onRejected: {
        callingAction.isActive = false
    }

    FormCard.FormTextFieldDelegate {
        id: nameField

        label: i18nc("As in 'a name'", "Name:")
        text: shownName
        maximumLength: 40

        leftPadding: Kirigami.Units.largeSpacing
        rightPadding: Kirigami.Units.largeSpacing

        Keys.onPressed: if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
            textPromptDialog.applied()
        }
    }

    function throwError(error) {
        let component = Qt.createComponent("qrc:/contents/ui/dialogs/NamingErrorDialog.qml")

        if (component.status == Component.Ready) {
            var dialog = component.createObject(textPromptDialog);
            dialog.error = error
            dialog.useCase = useCase
            dialog.nameField = nameField
            dialog.open()
        }
    }

    function checkName() {
        // The user just pressed apply without renaming the object
        if (textFieldText === shownName && !textPromptDialog.newItem) return ""

        const name = textFieldText.trim()
        const error = KleverUtility.isProperPath(parentPath,name)
        if (error !== "") return error

        if (name === Config.categoryDisplayName) return "exist"

        return ""
    }
}
