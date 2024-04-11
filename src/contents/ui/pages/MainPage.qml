// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2

import org.kde.kirigami 2.19 as Kirigami

import "qrc:/contents/ui/textEditor"
import "qrc:/contents/ui/todoEditor"

import org.kde.Klever 1.0

Kirigami.Page {
    id: root

    readonly property CheatSheet cheatSheet: cheatSheet
    readonly property bool hasNote: currentlySelected && currentlySelected.useCase === "Note"

    property QtObject currentlySelected
    property QtObject editorView: editorLoader.item
    property QtObject todoView: todoLoader.item

    title: hasNote ? currentlySelected.text : i18nc("@title:page", "Welcome")

    actions: {
        if (hasNote) {
            // At first both Loaders item are "null"
            if (editorLoader.item && editorLoader.item.visible) { 
                return editorLoader.item.actions
            }
            return todoLoader.item ? todoLoader.item.actions : []
        }
        return []
    }

    onCurrentlySelectedChanged: if (root.hasNote) {
        const editor = editorView.editor
        const oldPath = editorView.path
        const text = editor.text
        editor.saveNote(text, oldPath)
    }

    Loader {
        id: editorLoader

        sourceComponent: EditorView {
            path: currentlySelected.path + "/note.md"
            visible: bottomToolBar.showNoteEditor
        }
        active: root.hasNote
        anchors.fill: parent
    }

    Loader {
        id: todoLoader

        sourceComponent: ToDoView {
            path: currentlySelected.path + "/todo.json"
            visible: !bottomToolBar.showNoteEditor
        }
        active: root.hasNote
        anchors.fill: parent
    }


    ColumnLayout {
        id: placeHolder

        anchors.fill: parent
        visible: !root.hasNote

        QQC2.Label {
            text: i18n("Welcome to KleverNotes!")
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 24

            Layout.margins: Kirigami.Units.largeSpacing * 2
            Layout.fillWidth: true
        }

        QQC2.Label {
            text: i18n("Create or select a note to start working !")
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 12

            Layout.margins: Kirigami.Units.largeSpacing * 2
            Layout.fillWidth: true
        }
    }

    footer: BottomToolBar{
        id: bottomToolBar

        visible: root.hasNote
    }

    CheatSheet {
        id: cheatSheet
    }
}
