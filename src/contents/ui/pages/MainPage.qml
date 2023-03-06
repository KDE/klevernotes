// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0
import "qrc:/contents/ui/textEditor"

Kirigami.Page {
    id: root

    property QtObject currentlySelected
    property QtObject editorView: editorLoader.item
    readonly property bool hasNote: currentlySelected && currentlySelected.useCase === "Note"

    title: hasNote ? currentlySelected.displayedName : i18n("Welcome")

    actions.contextualActions: hasNote && editorView ? editorView.actions : []

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
        }
        active: root.hasNote
        anchors.fill: parent
    }

    Kirigami.Card {
        id: placeHolder

        visible: !root.hasNote

        anchors {
            fill: parent
            margins: Kirigami.Units.largeSpacing * 2
        }

        ColumnLayout {
            anchors.fill: parent
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false

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
    }

    // footer: BottomToolBar{}
}
