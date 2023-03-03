// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0
import "qrc:/contents/ui/textEditor"

Kirigami.Page {
    id: page

    title: i18n("Main Page")

    property QtObject currentlySelected
    property QtObject editorView : editorLoader.item

    onCurrentlySelectedChanged: {
        if (currentlySelected.useCase === "Note") {
            if (editorLoader.status === Loader.Null) editorLoader.active = true;

            const editor = editorView.editor
            const oldPath = editorView.path
            const text = editor.text
            editor.saveNote(text, oldPath)

            editorView.title = currentlySelected.displayedName
            editorView.path = currentlySelected.path+"/note.md"
            editorView.visible = true
            placeHolder.visible = false
            return
        }

        editorView.visible = false
        placeHolder.visible = true
    }

    Loader{
        id: editorLoader

        sourceComponent: editorHolder
        active: false
        anchors.fill: parent
    }

    Component {
        id: editorHolder

        EditorView{}
    }

    Kirigami.Card {
        id: placeHolder

        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing * 2
        ColumnLayout {
            anchors.fill: parent
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false

            Text {
                text: i18n("Welcome to KleverNotes!")
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                color: Kirigami.Theme.textColor
                font.pointSize: 24

                Layout.margins: Kirigami.Units.largeSpacing * 2
                Layout.fillWidth: true
            }
            Text {
                text: i18n("Create or select a note to start working !")
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                color: Kirigami.Theme.textColor
                font.pointSize: 12

                Layout.margins: Kirigami.Units.largeSpacing * 2
                Layout.fillWidth: true
            }
        }
    }

    // footer: BottomToolBar{}
}
