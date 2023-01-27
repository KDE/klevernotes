// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>


import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.1
import org.kde.kirigami 2.19 as Kirigami

Kirigami.NavigationTabBar {
    id: tool
    visible: !applicationWindow().wideScreen

    actions: [
        Kirigami.Action {
            iconName: "clock"
            text: i18n("Note")
        },
        Kirigami.Action {
            iconName: "player-time"
            text: i18n("TODO")
        },
        Kirigami.Action {
            iconName: "chronometer"
            text: i18n("Documentation")
        }
    ]
}

