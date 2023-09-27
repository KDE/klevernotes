// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

Kirigami.PromptDialog {
    id: dialog

    property bool dontShow: false

    title: i18nc("@title:dialog", "Warning")

    Controls.Label {
        Layout.fillWidth: true
        text: i18n("This could cause visual artifact near the end of the pdf.")
        wrapMode: Text.WordWrap
    }

    footerLeadingComponent: FormCard.FormCheckDelegate {
        id: checkbox

        Layout.fillWidth: true

        text: i18nc("@label:checkbox", "Do not show again")
        onCheckedChanged: dialog.dontShow = checked
    }

    standardButtons: Kirigami.Dialog.Ok
}
