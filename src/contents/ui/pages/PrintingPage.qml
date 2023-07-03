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
                    let colors;
                    if (currentIndex === 0) colors = "default"
                    else {
                        const textDisplay = applicationWindow().pageStack.get(0).editorView.display
                        colors = ColorSchemer.getUsefullColors(currentIndex)
                    }
                    requestPdf(colors)
                }
            }
        },
        Kirigami.Action {
            text: i18n("background")
            icon.name: "backgroundtool"
            checkable: true
            onTriggered: {
                requestPdf()
                if (!Config.pdfWarningHidden) backgroundWarning.open()
            }
        },
        Kirigami.Action {
            separator: true
            enabled: false
        },
        Kirigami.Action {
            id: saveAction
            property string path

            text: i18n("Save")
            icon.name: "document-save-symbolic"
            onTriggered: pdfSaver.open()
            onPathChanged: {
                webEnginePreview.printToPdf(path.replace("file://",""))
            }
        }
    ]

    onBackRequested: (event) => {
        event.accepted = true;
        closePage()
    }

    WarningDialog {
        id: backgroundWarning

        onClosed: Config.pdfWarningHidden = checkbox.checked
    }

    FileSaverDialog {
        id: pdfSaver

        caller: saveAction
        noteName: applicationWindow().pageStack.get(0).title
    }

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
