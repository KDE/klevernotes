// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami

Kirigami.PromptDialog {
    id: dialog

    property alias checkbox: checkbox

    title: i18n("Warning")

    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing * 2

        Controls.Label {
            Layout.fillWidth: true
            text: i18n("This could cause visual artifact near the end of the pdf.")
            wrapMode: Text.WordWrap
        }

        // Move this to the footer as soon a this is possible
        Controls.CheckBox {
            id: checkbox
            Layout.fillWidth: true
            text: i18n("Do not show again")
        }
    }

    standardButtons: Kirigami.Dialog.Ok
}
