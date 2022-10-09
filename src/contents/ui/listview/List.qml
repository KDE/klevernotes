/*
 *  SPDX-FileCopyrightText: 2020 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.5 as Kirigami
import org.kde.Klever 1.0

Flickable {
    id: flick
    anchors.fill: parent
    clip: true
    contentHeight: col.implicitHeight
    property var mymodel: {"content": {".BaseCategory":
                 {"content":
                     {"content":
                         {"TEST":
                             {"content": {"docu.html":
                                          {"content": {},
                                           "name": "docu.html",
                                           "path": "/home/louis/.KleverNotes/.BaseCategory/.BaseGroup/TEST/docu.html"},
                                          "note.html":
                                              {"content": {},
                                               "name": "note.html",
                                               "path": "/home/louis/.KleverNotes/.BaseCategory/.BaseGroup/TEST/note.html"},
                                          "todo.html":
                                              {"content": {},
                                               "name": "todo.html",
                                               "path": "/home/louis/.KleverNotes/.BaseCategory/.BaseGroup/TEST/todo.html"}},
                              "name": "TEST",
                              "path": "/home/louis/.KleverNotes/.BaseCategory/.BaseGroup/TEST"}},
                         "name": ".BaseGroup",
                         "path": "/home/louis/.KleverNotes/.BaseCategory/.BaseGroup"},
                  "name": ".BaseCategory",
                  "path": "/home/louis/.KleverNotes/.BaseCategory"}},
     "name": ".KleverNotes",
     "path": "/home/louis/.KleverNotes"}

    Column {
        id: col
        Component.onCompleted: componentListView.createObject(this, {"objmodel":flick.mymodel});
    }

    Component {
        id: componentListView
        Repeater {
            id: repeater
            property var objmodel: ({})
            model: Object.getOwnPropertyNames()
            Component.onCompleted:console.log(model)

            ColumnLayout {
                Layout.leftMargin: 50
                Controls.Button {
                    property var sprite: null
                    text: modelData
                    onClicked: {
                        if(sprite === null) {
                            if(typeof objmodel[modelData] === 'object')
                            sprite = componentListView.createObject(parent, {"objmodel":objmodel[modelData]});
                        }
                        else
                            sprite.destroy()

                    }
                }
            }
        }
    }
}
