// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.klevernotes

RowLayout {
    id: root

    height: Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing

    readonly property int transitionTime: Kirigami.Units.longDuration + Kirigami.Units.shortDuration * 1
    property bool showNoteEditor: true

    ReversibleButtonDelegate {
        id: todoButton
        icon.name: "view-task-symbolic"
        text: i18nc("@label:button", "TODO")

        visible: 0.4 < opacity
        opacity: 0
        Layout.fillHeight: true

        states: [
            State {
                name: "visible"
                when: root.showNoteEditor
                PropertyChanges { target: todoButton; Layout.preferredWidth: root.width }
                PropertyChanges { target: todoButton; opacity: 1 }
            },
            State {
                name: "invisible"
                when: !root.showNoteEditor
                PropertyChanges { target: editorView; Layout.preferredWidth: 0 }
                PropertyChanges { target: editorView; opacity: 0 }
            }
        ]

        Behavior on Layout.preferredWidth {
            NumberAnimation { duration: root.transitionTime ; easing.type: Easing.InOutCubic }
        }
        Behavior on opacity {
            NumberAnimation { duration: root.transitionTime }
        }

        onClicked: {
            root.showNoteEditor = false
        }
    }    

    ReversibleButtonDelegate {
        id: noteButton
        icon.name: "document-edit-symbolic"
        text: i18nc("@label:button, as in 'A note'", "Note")

        reversed: true
        visible: 0.4 < opacity
        opacity: 0
        Layout.fillHeight: true
        Layout.alignment: Qt.AlignRight

        states: [
            State {
                name: "visible"
                when: !root.showNoteEditor 
                PropertyChanges { target: noteButton; Layout.preferredWidth: root.width }
                PropertyChanges { target: noteButton; opacity: 1 }
            },
            State {
                name: "invisible"
                when: root.showNoteEditor 
                PropertyChanges { target: noteButton; Layout.preferredWidth: 0 }
                PropertyChanges { target: noteButton; opacity: 0 }
            }
        ]

        Behavior on Layout.preferredWidth {
            NumberAnimation { duration: root.transitionTime ; easing.type: Easing.InOutCubic }
        }
        Behavior on opacity {
            NumberAnimation { duration: root.transitionTime }
        }
        onClicked: {
            root.showNoteEditor = true
        }
    }
}
