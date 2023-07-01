// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

// LOOSELY BASED ON : https://invent.kde.org/network/angelfish/-/blob/master/lib/contents/ui/PrintPreview.qml
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Michael Lang <criticaltemp@protonmail.com>

import QtQuick 2.15
import QtWebEngine 1.10
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.2
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"

Kirigami.Page {
    id: printPreview

    title: i18n("Print")

    property string pdfPath
    readonly property QtObject textDisplay: applicationWindow().pageStack.get(0).editorView.display

    actions.contextualActions: [
        Kirigami.Action {
            displayComponent: ComboBox {
                id: colorTheme
                displayText: i18n("Color theme")
                textRole: "display"
                valueRole: "display"
                model: ColorSchemer.model
                Component.onCompleted: currentIndex = ColorSchemer.indexForScheme(Config.colorScheme);
                visible: Qt.platform.os !== "android"
                onCurrentValueChanged: {
                    if (currentIndex === 0) return;
                    const colors = ColorSchemer.getUsefullColors(currentIndex)
                    console.log(colors.bodyColor)
                    // ColorSchemer.apply(currentIndex);
                    // Config.colorScheme = ColorSchemer.nameForIndex(currentIndex);
                    // Config.save();
                }
            }
            // onTriggered:
        },
        Kirigami.Action {
            text: i18n("Save")
            icon.name: "document-save-symbolic"
            // onTriggered:
        },
        Kirigami.Action {
            text: i18n("Cancel")
            icon.name: "edit-clear"
            // onTriggered:
        }
    ]


    WebEngineView {
        id: webEnginePreview

        anchors.fill: parent
        url: pdfPath
        settings.pluginsEnabled: true
        settings.pdfViewerEnabled: true
        settings.javascriptEnabled: false
        onContextMenuRequested: request.accepted = true // disable context menu
        onPdfPrintingFinished: printPreview.closePage()
    }

    Timer {
        id: applyingCssTimer

        interval: Kirigami.Units.longDuration
        repeat: false

        onTriggered: textDisplay.makePdf()
    }

    function requestPdf(style) {
        const textDisplay = applicationWindow().pageStack.get(0).editorView.display
        if (style) textDisplay.changeStyle(style)
        else textDisplay.printBackground = !textDisplay.printBackground
        applyingCssTimer.start()
    }

    function closePage() {
        const textDisplay = applicationWindow().pageStack.get(0).editorView.display
        textDisplay.changeStyle("default")
        applicationWindow().pageStack.pop()
    }
}
