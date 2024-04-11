// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import Qt.labs.platform 1.1

import org.kde.kirigami 2.19 as Kirigami

FileDialog {
    id: fileDialog

    property QtObject caller

    folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
    fileMode: FileDialog.SaveFile

    onAccepted: {
        caller.path = fileDialog.currentFile
    }
}
