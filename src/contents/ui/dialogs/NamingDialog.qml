// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

FormCard.FormCardDialog {
    id: textPromptDialog

    readonly property QtObject nameField: nameField

    property bool newItem
    property bool isNote
    property string shownName
    property string parentPath
    property QtObject callingAction
    property var callBackFunc
    property alias textFieldText: nameField.text

    title: i18nc("@title:dialog", "Choose a name")

    standardButtons: Dialog.Apply | Dialog.Cancel

    width: Kirigami.Units.gridUnit * 20

    onApplied: {
        const error = checkName()
        if (error.length === 0) {
            textPromptDialog.close()
            if (callingAction) { // Remove the use of this
                callingAction.name = nameField.text.trim()
            }
            if (callBackFunc) {
                callBackFunc(nameField.text.trim())
            }
            return
        }
        throwError(error)
        nameField.forceActiveFocus()
    }
    onOpened: {
        nameField.forceActiveFocus()
    }
    onRejected: {
        if (callingAction) { // Remove the use of this
            callingAction.isActive = false
        }
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
            dialog.isNote = isNote
            dialog.nameField = nameField
            dialog.open()
        } else {
            console.error(component.errorString())
        }
    }

    function checkName(): string {
        // The user just pressed apply without renaming the object
        if (textFieldText === shownName && !textPromptDialog.newItem) return ""

        const name = textFieldText.trim() + (isNote ? ".md" : "")
        const error = KleverUtility.isProperPath(parentPath, name)
        if (error.length !== 0) return error

        return ""
    }
}
