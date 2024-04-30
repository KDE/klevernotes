// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami

FileDialog {
    id: fileDialog

    property QtObject caller

    title: i18nc("@title:dialog", "Image picker")

    nameFilters: [ "Image files (*.jpeg *.jpg *.png)" ]

    onAccepted: {
        caller.path = fileDialog.selectedFile
    }
}
