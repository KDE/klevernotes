// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami


Kirigami.PromptDialog {
    id: textPromptDialog
    title: "Klever Notes Storage"

    property string useCase
    property QtObject nameField
    subtitle:i18n("This "+useCase+" already exist.\nPlease choose another name for it.")

    onRejected: nameField.selectAll()
}
