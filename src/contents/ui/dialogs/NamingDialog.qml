// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

FormCard.FormCardDialog {
    id: textPromptDialog

    readonly property QtObject nameField: nameField

    property bool newItem
    property string useCase
    property string shownName
    property string parentPath
    property QtObject callingAction
    property alias textFieldText: nameField.text

    title: i18nc("@title:dialog", "Choose a name")

    standardButtons: Dialog.Apply | Dialog.Cancel

    width: Kirigami.Units.gridUnit * 20

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

        Keys.onPressed: function(event) {
            if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                textPromptDialog.applied()
            }
        }
    }

    function throwError(error): void {
        let component = Qt.createComponent("qrc:/contents/ui/dialogs/NamingErrorDialog.qml")

        if (component.status == Component.Ready) {
            var dialog = component.createObject(textPromptDialog);
            dialog.error = error
            dialog.useCase = useCase
            dialog.nameField = nameField
            dialog.open()
        } else {
            console.error(component.errorString())
        }
    }

    function checkName(): string {
        // The user just pressed apply without renaming the object
        if (textFieldText === shownName && !textPromptDialog.newItem) return ""

        const name = textFieldText.trim()
        const error = KleverUtility.isProperPath(parentPath,name)
        if (error !== "") return error

        if (name === Config.categoryDisplayName) return "exist"

        return ""
    }
}
