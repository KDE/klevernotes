// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>

// LOOSELY BASED ON : https://invent.kde.org/network/angelfish/-/blob/master/lib/contents/ui/PrintPreview.qml
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Michael Lang <criticaltemp@protonmail.com>

import QtQuick
import QtWebEngine
import QtQuick.Controls as Controls
import QtQuick.Pdf
import QtQuick.Layouts
import Qt.labs.platform

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

import org.kde.Klever

import "qrc:/contents/ui/dialogs"

Kirigami.Page {
    id: printPreview

    readonly property QtObject textDisplay: applicationWindow().pageStack.get(0).editorView.display
    readonly property string pdfPath: StandardPaths.writableLocation(StandardPaths.TempLocation)+"/pdf-preview.pdf"
    readonly property string emptyPdf: StandardPaths.writableLocation(StandardPaths.TempLocation)+"/empty.pdf"

    property var colors

    title: i18nc("@title:page", "Print")

    actions: [
        Kirigami.Action {
            displayComponent: Controls.ComboBox {
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
            icon.name: "backgroundtool-symbolic"

            onTriggered: {
                if (!checked) {
                    requestPdf(false)
                } else {
                    backgroundWarning.openDialog()
                }
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
                printingUtility.copy(pdfPath.substring(7), path.substring(7))
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

    Components.MessageDialog {
        id: backgroundWarning

        dialogType: Components.MessageDialog.Warning
        standardButtons: Controls.Dialog.Ok
        dontShowAgainName: "pdfWarning"

        onAccepted: {
            close();
            requestPdf(true)
        }

        Controls.Label {
            text: i18n("This could cause visual artifacts near the end of the PDF.")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }

    FileSaverDialog {
        id: pdfSaver

        caller: saveAction

        title: i18nc("@title:dialog, choose the location of where the file will be saved", "Save note as PDF")
        currentFile: applicationWindow().pageStack.get(0).title
        nameFilters: [ "PDF file (*.pdf)"]
    }

    PdfMultiPageView {
        id: viewer
        width: Kirigami.Units.gridUnit * 15
        anchors.fill: parent
        clip: true
        document: PdfDocument { 
            id: pdfDoc

            onStatusChanged: function (status) {
                if (status === PdfDocument.Ready && source.toString().endsWith("preview.pdf")) {
                    busyIndicator.visible = false
                }
            }
        }
    }

    Controls.BusyIndicator {
        id: busyIndicator
        
        width: Kirigami.Units.gridUnit * 5
        height: width
        anchors.centerIn: parent
    }

    Timer {
        id: applyingCssTimer

        repeat: false
        interval: Kirigami.Units.longDuration

        onTriggered: {
            textDisplay.makePdf()
        }
    }

    Timer {
        id: changingDocTimer

        repeat: false
        interval: Kirigami.Units.longDuration

        onTriggered: {
            pdfDoc.source = pdfPath
        }
    }

    PrintingUtility {
        id: printingUtility

        onPdfCopyDone: function (succes, errorMessage) {
            if (!succes) {
                showPassiveNotification(errorMessage)
            } else {
                closePage()
            } 
        }
    }

    function displayPdf() {
        busyIndicator.visible = true
        printingUtility.writePdf(emptyPdf.substring(7)) 
        pdfDoc.source = emptyPdf
        changingDocTimer.start()
    }

    function requestPdf(changeBackground) {
        StyleHandler.printBackground = changeBackground
        StyleHandler.changeStyles(colors)
        applyingCssTimer.start()
    }

    function closePage() {
        StyleHandler.printBackground = true
        StyleHandler.changeStyles()
        applicationWindow().switchToPage("Main")
    }
}
