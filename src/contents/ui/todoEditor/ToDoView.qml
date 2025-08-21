// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as Components

import org.kde.klevernotes

Controls.ScrollView {
    id: root

    property list<Kirigami.Action> actions: [
        Kirigami.Action {
            id: clearTodosAction

            icon.name: "edit-clear-history-symbolic"
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

    data: ToDoDialog {
        id: todoDialog

        parent: applicationWindow().overlay

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
            close();
        }
        onRejected: close()
        onClosed: {
            name = ""
            description = ""
            callerModelIndex = -1
        }
    }

    ListView {
        id: todoView
        model: ListModel {
            id: todoModel
        }

        delegate: TodoDelegate {
            onSaveTodos: root.saveTodos()
            visibleSep: index < todoView.count - 1 
        }

        Components.FloatingButton {
            icon.name: "list-add-symbolic"
            text: i18nc("@action:button", "Add new todo")

            onClicked: {
                todoDialog.open()
            }

            anchors {
                right: parent.right
                rightMargin: Kirigami.Units.largeSpacing + (root.contentItem.Controls.ScrollBar && root.contentItem.Controls.ScrollBar.vertical ? parent.contentItem.Controls.ScrollBar.vertical.width : 0)
                bottom: parent.bottom
                bottomMargin: Kirigami.Units.largeSpacing
            }
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
