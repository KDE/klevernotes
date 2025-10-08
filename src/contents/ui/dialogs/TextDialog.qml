// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.klevernotes

FormCard.FormCardDialog {
    id: promptDialog

    property alias text: textField.text

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onOpened: textField.forceActiveFocus()

    TextAreaDelegate {
        id: textField

        label: ""
        wrapMode: TextEdit.WrapAnywhere

        Layout.preferredHeight: Kirigami.Units.gridUnit * 8
    }

    function clear() {
        textField.clear()
    }
}
