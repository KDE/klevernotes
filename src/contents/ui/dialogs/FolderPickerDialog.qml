// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick
import Qt.labs.platform

FolderDialog {
    id: folderDialog

    property QtObject parent
    property string toChangeProperty

    folder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
    options: FolderDialog.ShowDirsOnly

    onAccepted: {
        parent.folder = currentFolder
    }
}
