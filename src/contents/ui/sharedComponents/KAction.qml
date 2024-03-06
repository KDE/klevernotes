// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import org.kde.kirigami 2.19

Action {
    required property string actionName
    property var triggerFunction

    onTriggered: triggerFunction()
}
