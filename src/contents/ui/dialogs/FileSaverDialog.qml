// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.3 as Controls
import Qt.labs.platform 1.1
import org.kde.kirigami 2.19 as Kirigami

FileDialog {
    id: fileDialog

    title: "Save note"

    folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
    currentFile: folder+"/"+noteName+".pdf"

    fileMode: FileDialog.SaveFile

    property QtObject caller
    property string noteName

    onAccepted: {
        caller.path = fileDialog.currentFile
    }
}

