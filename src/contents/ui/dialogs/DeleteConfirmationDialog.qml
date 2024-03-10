// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15

import org.kde.kirigami 2.19 as Kirigami

Kirigami.PromptDialog {
    required property string useCase
    readonly property var useCaseTrad: {
        "category": i18nc("@subtitle:dialog, as in 'A note category'", "Are you sure you want to delete this category ?"),
        "group": i18nc("@subtitle:dialog, as in 'A note group'", "Are you sure you want to delete this group ?"),
        "note": i18nc("@subtitle:dialog", "Are you sure you want to delete this note ?") 
    }

    subtitle: useCase ? useCaseTrad[useCase.toLowerCase()] : ""
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
}
