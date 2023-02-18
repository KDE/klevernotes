// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami


Kirigami.PromptDialog {
    property string useCase
    readonly property var useCaseTrad: {
        "Category": i18n("category"),
        "Group": i18n("group"),
        "Note": i18n("note")
    }
    subtitle: "Are you sure you want to delete this "+useCaseTrad[useCase]+"?"
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
}
