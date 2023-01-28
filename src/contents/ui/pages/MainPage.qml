// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0
import "qrc:/contents/ui/textEditor"

Kirigami.Page {
    id: page
    title: i18n("Main Page")

    property QtObject currentlySelected

    onCurrentlySelectedChanged: {
        if (currentlySelected.useCase === "Note") {
            if (editorLoader.status === Loader.Null) editorLoader.active = true;

            editorLoader.item.title = currentlySelected.displayedName
            editorLoader.item.path = currentlySelected.path+"/note.md"
        }
    }

    Loader{
        id: editorLoader
        sourceComponent: editorHolder
        active: false
        onLoaded: placeHolder.parent = null
        anchors.fill: parent
    }

    Component {
        id: editorHolder
        EditorView{
            id:editor
        }
    }

    Text{
        anchors.fill: parent
        id: placeHolder
        text: "En attente d'un truc"
    }

    footer: BottomToolBar{}
}
