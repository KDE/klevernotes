// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.3 as Controls
import QtQuick.Dialogs 1.3
import org.kde.kirigami 2.19 as Kirigami

FileDialog {
    id: fileDialog

    title: i18n("Image picker")
    selectExisting: true
    selectMultiple: false
    nameFilters: [ "Image files (*.jpeg *.jpg *.png)" ]

    property QtObject caller

    onAccepted: {
        caller.path = fileDialog.fileUrl
    }
}
