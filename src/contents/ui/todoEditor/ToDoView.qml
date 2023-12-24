// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs/todoDialog"

ColumnLayout {
    id: root

    property list<Kirigami.Action> actions: [
        Kirigami.Action {
            id: clearTodosAction

            icon.name: "edit-clear-history"
            text: i18nc("@label:button, checked as in 'the todos that are checked'", "Clear checked")

            onTriggered: clearCheckedTodos()
        }
    ]
    property string path
    property int alreadySavedCount: 0

    onPathChanged: {
        todoModel.clear()
        setTodos()
    }

    ToDoDialog {
        id: todoDialog

        onAccepted: {
            if (name.length > 0) {
                if (callerModelIndex < 0) {
                    todoModel.append({
                        "todoTitle": name,
                        "todoDesc": description.trim(),
                        "todoChecked": false
                    })
                } else {
                    todoModel.setProperty(callerModelIndex, "todoTitle", name)
                    todoModel.setProperty(callerModelIndex, "todoDesc", description)
                }
            }
            saveTodos()
            name = ""
            description = ""
            callerModelIndex = -1
        }
    }

    Kirigami.Card {
        Layout.fillWidth: true
        Layout.fillHeight: true

        Kirigami.CardsListView {
            id: cardsView

            anchors.fill: parent

            model: todoModel
            delegate: TodoDelegate {}
        }

        ListModel {
            id: todoModel
        }
    }
    
    FormCard.AbstractFormDelegate {
        Layout.fillWidth: true

        contentItem: RowLayout {
            spacing: 0

            Kirigami.Icon {
                width: Kirigami.Units.iconSizes.medium
                height: Kirigami.Units.iconSizes.medium

                source: "list-add-symbolic"

                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }

        onClicked: {
            todoDialog.open()
        }
    }

    function clearCheckedTodos() {
        for (var idx = todoModel.count-1 ; idx >= 0 ; idx--){
            const model = todoModel.get(idx)
            if (model.todoChecked) {
                todoModel.remove(idx,1)
            }
        }
        saveTodos()
    }

    function setTodos() {
        const todos = DocumentHandler.getJson(root.path).todos
        if (!todos) return
        root.alreadySavedCount = todos.length

        todos.forEach(todo => todoModel.append(todo))
    }

    function saveTodos() {
        let json = {"todos":[]}

        for (var idx = 0 ; idx < todoModel.count ; idx++){
            const model = todoModel.get(idx)
            json.todos.push(model)
        }
        DocumentHandler.saveJson(json, root.path)
    }
}
