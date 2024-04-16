// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

Components.MessageDialog {
    required property string useCase
    required property string name

    readonly property var useCaseTrad: {
        "category": xi18nc("@info, as in 'A note category'", "Are you sure you want to delete the category <filename>%1</filename>?", name),
        "group": xi18nc("@info, as in 'A note group'", "Are you sure you want to delete the group <filename>%1</filename>?", name),
        "note": xi18nc("@info", "Are you sure you want to delete the note <filename>%1</filename>?", name)
    }

    readonly property var useCaseTitleTrad: {
        "category": i18nc("@title:dialog, as in 'A note category'", "Delete Category Confirmation"),
        "group": i18nc("@title:dialog, as in 'A note category'", "Delete Group Confirmation"),
        "note": i18nc("@title:dialog, as in 'A note category'", "Delete Note Confirmation"),
    }

    dialogType: Components.MessageDialog.Warning
    title: useCase ? useCaseTitleTrad[useCase.toLowerCase()] : ""

    Label {
        text: useCase ? useCaseTrad[useCase.toLowerCase()] : ""
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }

    Label {
        text: i18nc("@title:dialog", "This action can not be reverted.")
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }

    standardButtons: Dialog.Ok | Dialog.Cancel
}
