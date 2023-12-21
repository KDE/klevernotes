// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import org.kde.kirigami 2.19 as Kirigami

Kirigami.NavigationTabBar {
    id: root

    property string currentTab: "general"
    
    actions: [
        Kirigami.Action {
            text: i18nc("@label, tab (inside tab bar), general settings of the app", "General")
            icon.name: "settings"
            checked: root.currentTab === "general"
            onTriggered: {
                root.currentTab = "general"
            }
        },
        Kirigami.Action {
            text: i18nc("@label, tab (inside tab bar), all settings related to the general appearance of the app", "Appearance")
            icon.name: "preferences-desktop-theme-global"
            checked: root.currentTab === "appearance"
            onTriggered: {
                root.currentTab = "appearance"
            }
        },
        Kirigami.Action {
            text: i18nc("@label, tab (inside tab bar), all things related to plugins", "Plugins")
            icon.name: "plugins"
            checked: root.currentTab === "plugins"
            onTriggered: {
                root.currentTab = "plugins"
            }
        }
    ]
}
