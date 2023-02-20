// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami

Kirigami.Dialog {
    title: i18n("Color Picker")
    padding: 0
    preferredWidth: Kirigami.Units.gridUnit * 16
    preferredHeight : Kirigami.Units.gridUnit * 16

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    property alias selectedColor: picker.selectedColor

    ColorPicker{
        id:picker
        anchors.fill:parent
    }
}

