// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import Qt.labs.platform

import org.kde.kirigami as Kirigami

FileDialog {
    id: fileDialog

    property QtObject caller

    folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
    fileMode: FileDialog.SaveFile

    onAccepted: {
        caller.path = fileDialog.currentFile
    }
}
