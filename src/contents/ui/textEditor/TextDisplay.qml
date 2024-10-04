// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>

// ORIGINALLY BASED ON : https://github.com/CrazyCxl/markdown-editor
// SPDX-FileCopyrightText: 2019 CrazyCxl <chenxiaolong0001@gmail.com>

import QtQuick
import QtQuick.Controls
import QtWebChannel
import QtWebEngine
import QtQuick.Layouts
import Qt.labs.platform

import org.kde.kirigami as Kirigami

import org.kde.Klever
import qtMdEditor as QtMdEditor

RowLayout {
    id: root

    readonly property var view: web_view

    readonly property string previewLocation: StandardPaths.writableLocation(StandardPaths.TempLocation)+"/pdf-preview.pdf"
    readonly property string emptyPreview: (StandardPaths.writableLocation(StandardPaths.TempLocation)+"/empty.pdf").substring(7)

    property string defaultHtml
    property string css

    spacing: 0

    Component.onCompleted: {
        loadBaseHtml()
        StyleHandler.loadStyle()
    }

    // WARNING, HACK: 
    // Do not remove, this prevent the WebEngineView from taking 
    //to much space and escaping from the Card !
    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true

        WebEngineView {
            id: web_view

            x: 2
            y: 2

            settings {
                showScrollBars: false
                localContentCanAccessFileUrls: true
                localContentCanAccessRemoteUrls: true
            }
            focus: true
            backgroundColor: "transparent"
            webChannel: WebChannel{
                registeredObjects: [contentLink, cssLink]
            }

            anchors.fill: parent
            clip: true

            onJavaScriptConsoleMessage: function (level, message, lineNumber, sourceID) {
                console.error('WEB:', message, lineNumber, sourceID)
            }
            onPdfPrintingFinished: {
                const printingPage = applicationWindow().pageStack.currentItem

                printingPage.displayPdf()
            }
            onLoadingChanged: if (!loading) {
                loadCss()
                scrollToHeader()
            }
            onScrollPositionChanged: if (!vbar.active) {
                vbar.position = scrollPosition.y / contentsSize.height
            }
            onNavigationRequested: function(request) {
                const url = request.url.toString()
                if (url.startsWith("copy:")) {
                    KleverUtility.copyToClipboard(url.substring("copy:".length))
                    showPassiveNotification(i18n("Copied !"))
                    request.reject()
                } else if (url.startsWith("http") || url.startsWith("file://")) { // Seems silly but prevent errors when loading pages
                    let notePath = url.substring(7)
                    const delimiterIndex = notePath.lastIndexOf("@HEADER@")
                    if (delimiterIndex != -1 && Config.noteMapEnabled) {
                        // const header = notePath.substring(delimiterIndex + 8)
                        
                        notePath = notePath.substring(0, delimiterIndex)

                        const headerInfo = NoteMapper.getCleanedHeaderAndLevel(header)
                        const sidebar = applicationWindow().globalDrawer
                        const noteModelIndex = sidebar.treeModel.getNoteModelIndex(notePath)

                        if (noteModelIndex.row !== -1) {
                            // if (header[1] !== 0) parser.headerInfo = headerInfo

                            sidebar.askForFocus(noteModelIndex)
                        } 
                        else {
                            notePath = notePath.replace(".BaseCategory", Config.categoryDisplayName).replace(".BaseGroup/", "")
                            showPassiveNotification(i18nc("@notification, error message %1 is a path", "%1 doesn't exists", notePath))
                        }
                    } else {
                        Qt.openUrlExternally(request.url)
                    }
                    request.reject()
                }
            }
            QtMdEditor.QmlLinker{
                id: contentLink
                WebChannel.id: "contentLink"
            }
            QtMdEditor.QmlLinker{
                id: cssLink
                WebChannel.id: "cssLink"
            }
        }
    }

    ScrollBar {
        id: vbar

        size: background.height / web_view.contentsSize.height
        active: hovered || pressed
        snapMode: ScrollBar.SnapAlways
        orientation: Qt.Vertical
        hoverEnabled: true

        Layout.row :0
        Layout.column: 1
        Layout.fillHeight: true

        onPositionChanged: {
            if (active) {
                let scrollY = web_view.contentsSize.height * vbar.position
                web_view.runJavaScript("window.scrollTo(0," + scrollY + ")")
            }
        }
    }

    Connections {
        id: editorHandlerConnections
        target: EditorHandler

        function onRenderingFinished(content) {
            contentLink.text = content
        }
    }

    Connections {
        id: styleHandlerConnections
        target: StyleHandler 

        function onNewCss(css) {
            root.css = css
            loadCss()
        }
    }

    function loadBaseHtml() {
        if (!root.defaultHtml) root.defaultHtml = DocumentHandler.readFile(":/index.html")

        web_view.loadHtml(root.defaultHtml, "file:/")
    }

    function loadCss() {
        cssLink.text = root.css
    }

    function makePdf() {
        web_view.printToPdf(root.previewLocation.replace("file://",""))
    }

    function scrollToHeader() {
        return
    }
}
