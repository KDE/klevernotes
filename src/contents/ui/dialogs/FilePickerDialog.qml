// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Dialogs @QTQUICKDIALOG_VERSION@

import org.kde.kirigami 2.19 as Kirigami

FileDialog {
    id: fileDialog

    property QtObject caller

    title: i18nc("@title:dialog", "Image picker")

    nameFilters: [ "Image files (*.jpeg *.jpg *.png)" ]

    onAccepted: {
        caller.path = fileDialog.fileUrl
    }
    Component.onCompleted: if (@QT_MAJOR_VERSION@ < 6) {
        selectMultiple = false
        selectExisting = true
    }
}
