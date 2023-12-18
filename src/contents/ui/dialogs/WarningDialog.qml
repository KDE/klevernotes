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

    footerLeadingComponent: FormCard.FormCheckDelegate {
        id: checkbox

        text: i18nc("@label:checkbox", "Do not show again")
        Layout.fillWidth: true

        onCheckedChanged: dialog.dontShow = checked
    }
    standardButtons: Kirigami.Dialog.Ok

    Controls.Label {
        text: i18n("This could cause visual artifact near the end of the pdf.")
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }
}
