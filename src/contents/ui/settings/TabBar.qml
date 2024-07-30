// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import org.kde.kirigami as Kirigami

Kirigami.NavigationTabBar {
    id: root

    property string currentTab: "general"
    
    actions: [
        Kirigami.Action {
            text: i18nc("@label, tab (inside tab bar), general settings of the app", "General")
            icon.name: "settings-configure-symbolic" 
            checked: root.currentTab === "general"
            onTriggered: {
                root.currentTab = "general"
            }
        },
        Kirigami.Action {
            text: i18nc("@label, tab (inside tab bar), all settings related to the rendered preview of the note", "Note preview")
            icon.name: "preferences-desktop-theme-global-symbolic"
            checked: root.currentTab === "preview"
            onTriggered: {
                root.currentTab = "preview"
            }
        },
        Kirigami.Action {
            text: i18nc("@label, tab (inside tab bar), all things related to plugins", "Plugins")
            icon.name: "plugins-symbolic"
            checked: root.currentTab === "plugins"
            onTriggered: {
                root.currentTab = "plugins"
            }
        }
    ]
}
