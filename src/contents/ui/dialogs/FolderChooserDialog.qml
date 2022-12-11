// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import Qt.labs.platform 1.1

FolderDialog{
    id:folderDialog
    folder:StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
    options: FolderDialog.ShowDirsOnly

    property QtObject parent;
    property string toChangeProperty;
    onAccepted: {
        parent.folder = currentFolder
    }
}
