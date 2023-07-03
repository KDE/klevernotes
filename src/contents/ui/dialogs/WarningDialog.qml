// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami

Kirigami.PromptDialog {
    id: dialog

    property alias checkbox: checkbox

    title: i18n("Warning")

    Column {
        Controls.Label {
            width: dialog.width
            text: i18n("This could cause visual artifact near the end of the pdf.")
            wrapMode: Text.WordWrap
        }
        Item {
            width: 2
            height: Kirigami.Units.largeSpacing * 2
        }
        // Move this to the footer as soon a this is possible
        Controls.CheckBox {
            id: checkbox
            text: i18n("Do not show again")
        }
    }

    standardButtons: Kirigami.Dialog.Ok
}
