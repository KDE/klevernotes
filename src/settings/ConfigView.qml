// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.settings as KirigamiSettings

Kirigami.ConfigurationView {
    id: root

    modules: [
        KirigamiSettings.ConfigurationModule {
            moduleId: "general"
            text: i18nc("@label, tab (inside tab bar), general settings of the app", "General")
            icon.name: "settings-configure-symbolic" 
            page: () => Qt.createComponent("org.kde.klevernote.settings", "GeneralPage")
        },
        KirigamiSettings.ConfigurationModule {
            moduleId: "appearance"
            text: i18nc("@label, tab (inside tab bar), all settings related to the rendered preview of the note", "Appearance")
            icon.name: "preferences-desktop-theme-global-symbolic"
            page: () => Qt.createComponent("org.kde.klevernote.settings", "AppearancePage")
        },
        KirigamiSettings.ConfigurationModule {
            text: i18nc("@label, tab (inside tab bar), all settings related to the text editor", "Editor")
            icon.name: "accessories-text-editor"
            moduleId: "editor"
            page: () => Qt.createComponent("org.kde.klevernote.settings", "EditorPage")
        },
        KirigamiSettings.ConfigurationModule {
            text: i18nc("@label, tab (inside tab bar), all things related to plugins", "Plugins")
            icon.name: "plugins-symbolic"
            page: () => Qt.createComponent("org.kde.klevernote.settings", "PluginsPage")
        }
    ]
}
