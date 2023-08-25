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

    property string pdfPath: ""
    property var colors
    readonly property QtObject textDisplay: applicationWindow().pageStack.get(0).editorView.display

    actions.contextualActions: [
        Kirigami.Action {
            displayComponent: ComboBox {
                id: colorTheme
                displayText: i18n("Color theme")
                textRole: "display"
                valueRole: "display"
                model: ColorSchemer.model

                onCurrentValueChanged: {
                    if (currentIndex === 0) printPreview.colors = "default"
                    else {
                        printPreview.colors = ColorSchemer.getUsefullColors(currentIndex)
                    }
                    requestPdf(toogleBackground.checked)
                }
            }
        },
        Kirigami.Action {
            id: toogleBackground

            text: i18n("background")
            icon.name: "backgroundtool"
            checkable: true
            onTriggered: {
                requestPdf(checked)
                if (!Config.pdfWarningHidden && checked) backgroundWarning.open()
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
        visible: false
        onLoadProgressChanged: if (loadProgress === 100) visible = true
        settings.pluginsEnabled: true
        settings.pdfViewerEnabled: true
        settings.javascriptEnabled: false
        onContextMenuRequested: request.accepted = true // disable context menu
        onPdfPrintingFinished: printPreview.closePage()
    }

    BusyIndicator {
        anchors.centerIn: parent

        width: Kirigami.Units.gridUnit * 5
        height: width
        visible: !webEnginePreview.visible
    }

    Timer {
        id: applyingCssTimer

        interval: Kirigami.Units.longDuration
        repeat: false

        onTriggered: {
            webEnginePreview.visible = false
            textDisplay.makePdf()
        }
    }

    function requestPdf(changeBackground) {
        textDisplay.printBackground = changeBackground
        textDisplay.changeStyle(colors)
        applyingCssTimer.start()
    }

    function closePage() {
        textDisplay.printBackground = true
        textDisplay.changeStyle("default")
        applicationWindow().pageStack.pop()
    }

    Component.onCompleted: requestPdf(false)
}
