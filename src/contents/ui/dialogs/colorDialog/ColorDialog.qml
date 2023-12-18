// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15

import org.kde.kirigami 2.19 as Kirigami

Kirigami.Dialog {
    property alias selectedColor: picker.selectedColor

    title: i18nc("@title:dialog", "Color Picker")

    padding: 0
    preferredWidth: Kirigami.Units.gridUnit * 16
    preferredHeight : Kirigami.Units.gridUnit * 16

    standardButtons: Kirigami.Dialog.Apply | Kirigami.Dialog.Cancel

    ColorPicker {
        id: picker
        anchors.fill: parent
    }
}
