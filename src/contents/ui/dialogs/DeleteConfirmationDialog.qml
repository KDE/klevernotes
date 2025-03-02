// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components
import org.kde.kirigamiaddons.formcard as FormCard

Components.MessageDialog {
    required property bool isNote
    required property string name
    readonly property bool permanent: permanentDeleteCheck.checked

    width: Kirigami.Units.gridUnit * 20

    dialogType: Components.MessageDialog.Warning
    title: isNote 
        ? i18nc("@title:dialog, Confirmation for deletion of a note", "Delete Note Confirmation")
        : i18nc("@title:dialog, Confirmation for deletion of a folder", "Delete Folder Confirmation")

    standardButtons: Dialog.Ok | Dialog.Cancel

    Label {
        text: isNote
            ? xi18nc("@info", "Are you sure you want to delete this note <filename>%1</filename>?", name)
            : xi18nc("@info", "Are you sure you want to delete this folder <filename>%1</filename>?", name)

        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }

    FormCard.FormCheckDelegate {
        id: permanentDeleteCheck

        text: i18nc("@label:check", "Delete this permanently.")

        Layout.fillWidth: true
    }
}
