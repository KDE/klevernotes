// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>


import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.1
import org.kde.kirigami 2.19 as Kirigami

Kirigami.NavigationTabBar {
    id: tool

    property bool showNoteEditor: true

    actions: [
        Kirigami.Action {
            iconName: "document-edit"
            text: i18n("Note")

            onTriggered: showNoteEditor = true
        },
        Kirigami.Action {
            iconName: "dino-double-tick-symbolic"
            text: i18n("TODO")

            onTriggered: showNoteEditor = false
        }
    ]
}

