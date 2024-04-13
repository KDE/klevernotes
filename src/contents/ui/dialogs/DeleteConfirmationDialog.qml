// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

Components.MessageDialog {
    required property string useCase

    readonly property var useCaseTrad: {
        "category": i18nc("@title:dialog, as in 'A note category'", "Are you sure you want to delete this category?"),
        "group": i18nc("@title:dialog, as in 'A note group'", "Are you sure you want to delete this group?"),
        "note": i18nc("@title:dialog", "Are you sure you want to delete this note?") 
    }

    dialogType: Components.MessageDialog.Warning
    title: useCase ? useCaseTrad[useCase.toLowerCase()] : ""

    Label {
        text: i18nc("@title:dialog", "This action can not be reverted.")
        wrapMode: Text.WordWrap
    }

    standardButtons: Dialog.Ok | Dialog.Cancel
}
