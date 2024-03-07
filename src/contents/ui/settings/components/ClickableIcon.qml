// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Templates 2.15 as T

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0

T.ItemDelegate {
    id: root

    hoverEnabled: true
    horizontalPadding: 0
    background: FormDelegateBackground { control: root }

   Kirigami.Icon {
        color: root.icon.color
        source: root.icon.name
        visible: root.icon.name !== ""

        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing * 2
    } 

    Accessible.onPressAction: action ? action.trigger() : root.clicked()
}
