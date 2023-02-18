// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as Controls

import org.kde.Klever 1.0

Kirigami.PromptDialog {
    id: textPromptDialog

    title: "Choose a name"

    property string useCase
    property string shownName
    property string realName
    property string parentPath
    property bool newItem
    property bool sideBarAction
    property QtObject callingAction

    readonly property QtObject nameField : nameField


    function throwError(){
        let component = Qt.createComponent("qrc:/contents/ui/dialogs/RenameErrorDialog.qml")

        if (component.status == Component.Ready) {
            var dialog = component.createObject(textPromptDialog);
            dialog.useCase = useCase
            dialog.nameField = nameField
            dialog.open()
        }
    }

    function checkName(){
        // The user just pressed apply without renaming the object
        if (nameField.text === shownName && !textPromptDialog.newItem) return true

        let exist = false
        if (sideBarAction) {
            const newPath = parentPath+"/"+nameField.text
            exist = KleverUtility.exist(newPath)
        }

        const isDisplay = (nameField.text === Config.categoryDisplayName)

        return !(exist || isDisplay)
    }


    Controls.TextField {
        id:nameField

        text: shownName

        onSelectedTextChanged: nameField.forceActiveFocus()

        Keys.onPressed: if ((event.key === Qt.Key_Return) || (event.key=== Qt.Key_Enter)) applyAction.trigger()
    }

    standardButtons: Kirigami.Dialog.NoButton

    customFooterActions: [
        Kirigami.Action {
            id:applyAction
            text: i18n("Apply")
            iconName: "dialog-ok"
            onTriggered: {
                if (checkName()){
                    textPromptDialog.close()
                    callingAction.name = nameField.text
                    return
                }
                throwError()
                nameField.selectAll()
            }
        },
        Kirigami.Action {
            text: i18n("Cancel")
            iconName: "dialog-cancel"

            onTriggered: textPromptDialog.close()
        }
    ]
}
