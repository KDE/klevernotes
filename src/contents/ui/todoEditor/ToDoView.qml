// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"

ColumnLayout {
    id: root

    property list<Kirigami.Action> actions: [
        Kirigami.Action {
            id: clearTODOS

            icon.name: "edit-clear-history"
            text: i18n("Clear checked")

            onTriggered: console.log("test")
        }
    ]
    property string path
    property bool buffer: false

    onPathChanged: setTodos()

    ToDoDialog {
        id: todoDialog

        onAccepted: {
            if (name.length > 0) {
                if (!caller) {
                    todoModel.append({
                        title: name,
                        desc: description.trim(),
                        checked: false
                    })
                } else {
                    caller.todoTitle = name
                    caller.todoDesc = description
                }

            }
            name = ""
            description = ""
            // can't put it
            caller = null
        }
    }

    Kirigami.Card {
        Layout.fillWidth: true
        Layout.fillHeight: true

        Kirigami.CardsListView {
            id: todoList

            anchors {
                fill: parent
                margins: Kirigami.Units.largeSpacing * 2
            }
            clip: true
            model: todoModel
            delegate: todoDelegate

            Controls.ScrollBar.vertical: Controls.ScrollBar {
                active: true
            }
        }


        ListModel {
            id: todoModel
        }

        Component {
            id: todoDelegate
            Kirigami.AbstractCard {
                id: card

                property string todoTitle: title
                property string todoDesc: desc
                property alias todoCheck: check.checked
                onTodoCheckChanged: console.log(todoCheck)
                contentItem: Item {
                    id: holder
                    implicitWidth: parent.width
                    implicitHeight: Kirigami.Units.iconSizes.large

                    RowLayout {
                        id: delegateLayout

                        anchors.fill: parent

                        Controls.CheckBox {
                            id: check

                            checked: checked
                            Layout.alignment: Qt.AlignVCenter
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Kirigami.Heading {
                                Layout.fillWidth: true
                                Layout.rightMargin: Kirigami.Units.smallSpacing
                                elide: Text.ElideRight
                                level: 2
                                text: todoTitle
                            }
                            Kirigami.Separator {
                                Layout.fillWidth: true
                                Layout.rightMargin: Kirigami.Units.smallSpacing
                                visible: todoDesc.length > 0
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
                                todoDialog.caller = card
                                todoDialog.name = todoTitle
                                todoDialog.description = todoDesc
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
                // Component.onCompleted: if (!root.buffer) saveTodos()
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

/*
    Timer {
        id: noteSaverTimer

        interval: 5000
        // running: true
        // repeat: true
        onTriggered: {
            for(var childIdx = 0; childIdx < todoList.count ; childIdx++){
                const child = todoList.itemAtIndex(childIdx)
                console.log(child.todoTitle, child.todoDesc, child.todoCheck)
            }
        }
    }*/

    function setTodos() {
        const todos = TodoHandler.readTodos(root.path).todos
        root.buffer = true
        todos.forEach(todo => todoModel.append(todo))
        root.buffer = false
    }

    function saveTodos() {
        let json = {"todos":[]}

        for(var childIdx = 0; childIdx < todoList.count ; childIdx++){
            const child = todoList.itemAtIndex(childIdx)
            console.log(child)
            const jsonObject = {
                "title": child.todoTitle,
                "desc": child.todoDesc,
                "checked": child.todoCheck
            }
            json.todos.push(jsonObject)
        }
        TodoHandler.writeTodos(json, root.path)
    }
}
