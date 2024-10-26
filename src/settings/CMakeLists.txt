# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

qt_add_library(settings STATIC)

ecm_add_qml_module(settings GENERATE_PLUGIN_SOURCE
    URI org.kde.klevernotes.settings
    QML_FILES
        GeneralPage.qml
        ConfigView.qml
        EditorPage.qml
        PluginsPage.qml
        AppearancePage.qml
)
