// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

import "qrc:/contents/ui/textEditor"
import "qrc:/contents/ui/todoEditor"

import org.kde.Klever

Kirigami.Page {
    id: root

    readonly property CheatSheet cheatSheet: cheatSheet
    readonly property bool hasNote: currentlySelected && currentlySelected.isNote
    readonly property bool isVisible: applicationWindow().isMainPage()

    property QtObject currentlySelected
    property alias editorView: editorView
    property alias todoView: todoView

    title: hasNote ? currentlySelected.text : i18nc("@title:page", "Welcome")

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    padding: 0

    actions: !hasNote 
                ? []
                : bottomToolBar.showNoteEditor 
                    ? editorView.actions 
                    : todoView.actions
    
    onCurrentlySelectedChanged: if (currentlySelected.isNote) {
        EditorHandler.noteDir = currentlySelected.dir
        const editor = editorView.editor
        const oldPath = editorView.path
        const text = editor.text
        editor.saveNote(text, oldPath)
    }

    RowLayout {
        id: viewLayout
        anchors.fill: parent
        readonly property int transitionTime: Kirigami.Units.longDuration + Kirigami.Units.shortDuration * 1

        EditorView {
            id: editorView
            path: hasNote ? currentlySelected.path : ""
            visible: 0.5 < opacity
            opacity: 0
            Layout.fillHeight: true

            states: [
                State {
                    name: "visible"
                    when: bottomToolBar.showNoteEditor && root.hasNote
                    PropertyChanges { target: editorView; Layout.preferredWidth: viewLayout.width }
                    PropertyChanges { target: editorView; opacity: 1 }
                },
                State {
                    name: "invisible"
                    when: !bottomToolBar.showNoteEditor && root.hasNote
                    PropertyChanges { target: editorView; Layout.preferredWidth: 0 }
                    PropertyChanges { target: editorView; opacity: 0 }
                }
            ]

            Behavior on Layout.preferredWidth {
                NumberAnimation { duration: viewLayout.transitionTime ; easing.type: Easing.InOutCubic }
            }
            Behavior on opacity {
                NumberAnimation { duration: viewLayout.transitionTime }
            }
        }

        ToDoView {
            id: todoView
            path: hasNote ? currentlySelected.path + "/todo.json" : ""
            visible: 0.5 < opacity
            opacity: 0
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignRight

            states: [
                State {
                    name: "visible"
                    when: !bottomToolBar.showNoteEditor 
                    PropertyChanges { target: todoView; Layout.preferredWidth: viewLayout.width }
                    PropertyChanges { target: todoView; opacity: 1 }
                },
                State {
                    name: "invisible"
                    when: bottomToolBar.showNoteEditor 
                    PropertyChanges { target: todoView; Layout.preferredWidth: 0 }
                    PropertyChanges { target: todoView; opacity: 0 }
                }
            ]

            Behavior on Layout.preferredWidth {
                NumberAnimation { duration: viewLayout.transitionTime ; easing.type: Easing.InOutCubic }
            }
            Behavior on opacity {
                NumberAnimation { duration: viewLayout.transitionTime }
            }
        }
    }

    ColumnLayout {
        id: placeHolder

        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
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

    Kirigami.Separator {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
    }

    footer: BottomToolBar {
        id: bottomToolBar

        Kirigami.Theme.backgroundColor: applicationWindow().sideBarColor
        visible: root.hasNote
    }

    CheatSheet {
        id: cheatSheet
    }
}
