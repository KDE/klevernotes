// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

// LOOSELY BASED ON : https://invent.kde.org/network/angelfish/-/blob/master/lib/contents/ui/PrintPreview.qml
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Michael Lang <criticaltemp@protonmail.com>

import QtQuick 2.15
import QtWebEngine 1.10

import org.kde.kirigami 2.19 as Kirigami

Kirigami.Page {
    id: printPreview

    title: i18n("Print")

    property string pdfPath

    WebEngineView {
        id: webEnginePreview

        anchors.fill: parent

        url: pdfPath
        settings.pluginsEnabled: true
        settings.pdfViewerEnabled: true
        settings.javascriptEnabled: false
        onContextMenuRequested: request.accepted = true // disable context menu
        onPdfPrintingFinished: printPreview.close()
    }
}
