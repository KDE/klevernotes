// SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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

    required property string path
    required property string text
    
    // TODO: move all of those to C++ connect
    // NoteMapper
    readonly property bool noteMapEnabled: Config.noteMapEnabled // give us acces to a "Changed" signal
    readonly property NoteMapper noteMapper: applicationWindow().noteMapper
    // Emoji
    readonly property bool emojiEnabled: Config.quickEmojiEnabled
    readonly property string emojiTone: Config.emojiTone
    // ======================================

    readonly property string previewLocation: StandardPaths.writableLocation(StandardPaths.TempLocation)+"/pdf-preview.pdf"
    readonly property string emptyPreview: (StandardPaths.writableLocation(StandardPaths.TempLocation)+"/empty.pdf").substring(7)

    property string defaultHtml
    property string css

    spacing: 0

    onPathChanged: {
        EditorHandler.notePath = path
    }
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
                if (url.startsWith("http")) {
                    Qt.openUrlExternally(request.url)
                    request.action = WebEngineNavigationRequest.IgnoreRequest
                    return
                }
                if (url.startsWith("file:///")) {
                    let notePath = url.substring(7)
                    const delimiterIndex = notePath.lastIndexOf("@HEADER@")
                    const header = notePath.substring(delimiterIndex + 8)
                    
                    notePath = notePath.substring(0, delimiterIndex)

                    const headerInfo = applicationWindow().noteMapper.getCleanedHeaderAndLevel(header)
                    const sidebar = applicationWindow().globalDrawer
                    const noteModelIndex = sidebar.treeModel.getNoteModelIndex(notePath)

                    if (noteModelIndex.row !== -1) {
                        if (header[1] !== 0) EditorHandler.headerInfo = headerInfo

                        sidebar.askForFocus(noteModelIndex)
                    } 
                    else {
                        notePath = notePath.replace(".BaseCategory", Config.categoryDisplayName).replace(".BaseGroup/", "")
                        showPassiveNotification(i18nc("@notification, error message %1 is a path", "%1 doesn't exists", notePath))
                    }
                    request.action = WebEngineNavigationRequest.IgnoreRequest
                    return
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

        function onParsingFinished(content) {
            if (!web_view.loading) {
                contentLink.text = content
            } else {
                root.parsedHtml = content
            }
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
        if (EditorHandler.headerLevel !== "0") {
            web_view.runJavaScript("document.getElementById('noteMapperScrollTo')",function(result) { 
                if (result) { // Seems redundant but it's mandatory due to the way the wayview handle loadProgress
                    web_view.runJavaScript("document.getElementById('noteMapperScrollTo').scrollIntoView()")
                    // parser.headerInfo = ["", "0"]
                }
            })
        }
    }
}
