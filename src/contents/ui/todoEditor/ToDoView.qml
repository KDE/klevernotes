// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

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
            id: layout

            anchors.fill: parent

            model: todoModel
            delegate: todoDelegate
        }

        ListModel {
            id: todoModel
        }

        Component {
            id: todoDelegate
            Kirigami.AbstractCard {
                id: card

                width: parent.width

                contentItem: Item {
                    id: holder
                    implicitWidth: parent.width
                    implicitHeight: Kirigami.Units.iconSizes.large

                    RowLayout {
                        id: delegateLayout

                        anchors.fill: parent

                        Controls.CheckBox {
                            id: check

                            checked: todoChecked
                            onCheckedChanged: {
                                todoModel.setProperty(index, "todoChecked", checked)
                                root.saveTodos()
                            }
                            Layout.alignment: Qt.AlignVCenter
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Kirigami.Heading {
                                id: displayTitle

                                text: todoTitle
                                level: 2
                                elide: Text.ElideRight

                                Layout.fillWidth: true
                                Layout.rightMargin: Kirigami.Units.smallSpacing
                            }
                            Kirigami.Separator {
                                visible: descriptionLabel.text.length > 0

                                Layout.fillWidth: true
                                Layout.rightMargin: Kirigami.Units.smallSpacing
                            }

                            ColumnLayout {
                                id: expendable

                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.margins: 0
                                spacing: 0

                                Controls.Label {
                                    id: descriptionLabel

                                    text: todoDesc
                                    wrapMode: Text.WordWrap
                                    elide: Text.ElideRight

                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.margins: 0
                                }
                            }
                        }

                        Controls.Button {
                            id: editButton

                            icon.name: "document-edit"
                            icon.height: Kirigami.Units.iconSizes.small

                            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                            onClicked: {
                                todoDialog.callerModelIndex = index
                                todoDialog.name = displayTitle.text
                                todoDialog.description = descriptionLabel.text
                                todoDialog.open()
                            }
                        }
                    }
                }

                footer: Controls.Button{
                    id: expend

                    height: Kirigami.Units.iconSizes.small * 2

                    icon.name: descriptionLabel.implicitHeight > expendable.height ? "expand" : "collapse"
                    icon.height: Kirigami.Units.iconSizes.small

                    visible: (descriptionLabel.implicitHeight - 10 > expendable.height ||
                    holder.implicitHeight > Kirigami.Units.iconSizes.large)

                    onClicked: holder.implicitHeight =
                                holder.implicitHeight == Kirigami.Units.iconSizes.large
                                    ? delegateLayout.implicitHeight
                                    : Kirigami.Units.iconSizes.large
                }
            }
        }
    }

    Controls.Button {
        icon.name: "list-add-symbolic"
        icon.width: Kirigami.Units.iconSizes.medium
        icon.height: Kirigami.Units.iconSizes.medium

        Layout.fillWidth: true

        onClicked: todoDialog.open()
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
        root.alreadySavedCount = todos.length

        todos.forEach(todo => todoModel.append(todo))
    }

    function saveTodos() {
        let json = {"todos":[]}

        for (var idx = 0 ; idx < todoModel.count ; idx++){
            const model = todoModel.get(idx)
            json.todos.push(model)
        }
        DocumentHandler.writeTodos(json, root.path)
    }
}
