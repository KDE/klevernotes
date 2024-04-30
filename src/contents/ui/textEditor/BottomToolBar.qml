// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>


import QtQuick
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami

Kirigami.NavigationTabBar {
    id: tool

    property bool showNoteEditor: true

    actions: [
        Kirigami.Action {
            icon.name: "document-edit-symbolic"
            text: i18nc("@label:button, as in 'A note'", "Note")

            onTriggered: {
                showNoteEditor = true
            }
        },
        Kirigami.Action {
            icon.name: "view-task-symbolic"
            text: i18nc("@label:button", "TODO")

            onTriggered: {
                showNoteEditor = false
            }
        }
    ]
}
