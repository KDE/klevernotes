// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import org.kde.kirigami

Action {
    required property string actionName
    property string description

    property var triggerFunction: function(){}

    onTriggered: triggerFunction()
}
