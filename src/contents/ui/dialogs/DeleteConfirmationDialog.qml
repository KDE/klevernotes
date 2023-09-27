// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami


Kirigami.PromptDialog {
    property string useCase
    readonly property var useCaseTrad: {
        "category": i18nc("Name, as in 'A note category'", "category"),
        "group": i18nc("Name, as in 'A note group'", "group"),
        "note": i18nc("Name, as in 'A note'", "note")
    }

    subtitle: i18nc("@subtitle:dialog, %1 can be 'category' (a note category), 'group' (a note group) or 'note' (a note)", "Are you sure you want to delete this %1 ?",useCaseTrad[useCase.toLowerCase()])
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
}
