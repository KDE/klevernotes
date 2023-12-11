// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import Qt.labs.platform 1.1

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"

GridLayout{
    id: root

    required property string path
    readonly property TextEditor editor: editor
    readonly property TextDisplay display: display
    readonly property QtObject imagePickerDialog: toolbar.imagePickerDialog
    readonly property QtObject cheatSheet: cheatSheet

    property list<Kirigami.Action> actions: [
        Kirigami.Action {
            id: linkedNotesAction
            enabled: Config.noteMapEnabled
            visible: enabled
            shortcut: "Ctrl+M"
            tooltip: i18nc("@tooltip, will be followed by the shortcut", "Linked notes") + " (" + shortcut + ")"
            icon.name: "gnumeric-link-internal"
            onTriggered: noteMapLoader.item.open()
        },
        Kirigami.Action {
            id: pdfPrinter
            shortcut: "Ctrl+P"
            tooltip: i18nc("@tooltip, Print action, will be followed by the shortcut", "Print") + " (" + shortcut + ")"
            icon.name: "viewpdf"
            onTriggered: applicationWindow().switchToPage('Printing')
        },
        Kirigami.Action {
            id: editorToggler
            shortcut: "Ctrl+Shift+8"
            tooltip: i18nc("@tooltip, will be followed by the shortcut", "View/Hide editor") + " (" + shortcut + ")"
            checkable: true
            icon.name: editorToggler.checked ? "text-flow-into-frame" : "text-unflow"
            checked: true
            onTriggered: if (!editorToggler.checked && !viewToggler.checked) editorToggler.checked = true
        },
        Kirigami.Action {
            id: viewToggler
            shortcut: "Ctrl+Shift+9"
            tooltip: i18nc("@tooltip, display as in 'the note preview', will be followed by the shortcut", "View/Hide preview") + " (" + shortcut + ")"
            checkable: true
            icon.name: viewToggler.checked ? "quickview" : "view-hidden"
            checked: applicationWindow().wideScreen
            onTriggered: if (!viewToggler.checked && !editorToggler.checked) viewToggler.checked = true
        }
    ]

    FileSaverDialog {
        id: pdfSaver

        caller: pdfPrinter
        noteName: root.noteName
    }

    rows: 4
    columns: 2

    TextToolBar {
        id: toolbar

        editorTextArea: root.editor.textArea
        notePath: root.path

        Layout.row: 0
    }

    // This item can be seen as useless but it prevent a weird bug with the height not being adjusted
    Item {
        Layout.row: 1
        Layout.fillHeight: true
        Layout.fillWidth: true

        GridLayout {
            anchors.fill:parent

            columns: parent.width > Kirigami.Units.gridUnit * 30 ? 2 : 1
            rows: columns > 1 ? 1 : 2

            TextEditor{
                id: editor

                visible: editorToggler.checked

                path: root.path

                onTextChanged: display.text = text //Workaround

                Layout.fillWidth:true
                Layout.fillHeight:true
                Layout.preferredHeight: parent.columns === 2 ? parent.height : parent.height/2
                Layout.preferredWidth: parent.columns === 2 ? parent.width/2 : parent.width
            }

            TextDisplay{
                id: display

                visible: viewToggler.checked

                text: editor.text //Doesn't update ?!
                path: root.path.replace("note.md", "")

                Layout.fillWidth:true
                Layout.fillHeight:true
                Layout.preferredHeight: parent.columns === 2 ? parent.height : parent.height/2
                Layout.preferredWidth: parent.columns === 2 ? parent.width/2 : parent.width
            }
        }
    }

    CheatSheet {
        id: cheatSheet
    }

    Loader {
        id: noteMapLoader

        sourceComponent: NotesMap {
            id: linkedNotesMap

            parser: display.parser
        }
        active: Config.noteMapEnabled
    }
}
