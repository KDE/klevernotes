// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigamiaddons.formcard 1.0 as FormCard

ColumnLayout {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    FormCard.FormHeader {
        Layout.fillWidth: true
        title: i18nc("@title, where you can view the renderer note", "Note preview")
    }

    DisplayPreview {
        id: displayPreview
    }
}
