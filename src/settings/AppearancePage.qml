// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.Klever

ColumnLayout {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    DisplayPreview {
        id: displayPreview

        Layout.topMargin: Kirigami.Units.gridUnit
    }
}
