// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

// LOOSELY BASED ON : https://invent.kde.org/network/angelfish/-/blob/master/lib/contents/ui/PrintPreview.qml
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Michael Lang <criticaltemp@protonmail.com>

import QtQuick 2.15
import QtWebEngine 1.10
import QtQuick.Controls 2.2

import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"

Kirigami.Page {
    id: printPreview

    readonly property QtObject textDisplay: applicationWindow().pageStack.get(0).editorView.display

    property string pdfPath: ""
    property var colors

    title: i18nc("@title:page", "Print")

    actions.contextualActions: [
        Kirigami.Action {
            displayComponent: ComboBox {
                id: colorTheme

                model: ColorSchemer.model
                textRole: "display"
                valueRole: "display"
                displayText: i18nc("@label:ComboBox", "Color theme")

                onCurrentValueChanged: {
                    if (currentIndex === 0) printPreview.colors = {}
                    else {
                        printPreview.colors = ColorSchemer.getUsefullColors(currentIndex)
                    }
                    requestPdf(toogleBackground.checked)
                }
            }
        },
        Kirigami.Action {
            id: toogleBackground

            text: i18nc("@label:button, as in 'the background of an image'", "Background")
            checkable: true
            icon.name: "backgroundtool"

            onTriggered: {
                requestPdf(checked)
                if (!Config.pdfWarningHidden && checked) backgroundWarning.open()
            }
        },
        Kirigami.Action {
            enabled: false
            separator: true
        },
        Kirigami.Action {
            id: saveAction

            property string path

            text: i18nc("@label:button", "Save")
            icon.name: "document-save-symbolic"

            onTriggered: {
                pdfSaver.open()
            }
            onPathChanged: {
                webEnginePreview.printToPdf(path.replace("file://",""))
            }
        }
    ]

    onBackRequested: (event) => {
        event.accepted = true;
        closePage()
    }
    Component.onCompleted: {
        requestPdf(false)
    }

    WarningDialog {
        id: backgroundWarning

        onClosed: {
            Config.pdfWarningHidden = dontShow
        }
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

        settings.pluginsEnabled: true
        settings.pdfViewerEnabled: true
        settings.javascriptEnabled: false

        onLoadProgressChanged: if (loadProgress === 100) {
            visible = true
        }
        onContextMenuRequested: {
            request.accepted = true // disable context menu
        }
        onPdfPrintingFinished: {
            printPreview.closePage()
        }
    }

    BusyIndicator {
        width: Kirigami.Units.gridUnit * 5
        height: width
        anchors.centerIn: parent

        visible: !webEnginePreview.visible
    }

    Timer {
        id: applyingCssTimer

        repeat: false
        interval: Kirigami.Units.longDuration

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
}
