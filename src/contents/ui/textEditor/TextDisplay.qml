// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
// SPDX-FileCopyrightText: 2019 CrazyCxl <chenxiaolong0001@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.2
import QtQuick.Controls 2.2
import QtWebChannel 1.0
import QtWebEngine 1.10
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import Qt.labs.platform 1.1

import org.kde.Klever 1.0

RowLayout {
    id: root

    required property string path
    required property string text
    
    // Syntax highlight
    readonly property bool highlightEnabled: Config.codeSynthaxHighlightEnabled // give us acces to a "Changed" signal
    readonly property string highlighterStyle: Config.codeSynthaxHighlighterStyle // This will also be triggered when the highlighter itself is changed
    // NoteMapper
    readonly property bool noteMapEnabled: Config.noteMapEnabled // give us acces to a "Changed" signal

    readonly property Parser parser: parser
    readonly property NoteMapper noteMapper: applicationWindow().noteMapper
    readonly property string stylePath: Config.stylePath
    readonly property string previewLocation: StandardPaths.writableLocation(StandardPaths.TempLocation)+"/pdf-preview.pdf"

    property string defaultHtml
    property string parsedHtml
    property string cssStyle
    property string completCss
    readonly property var viewFontInfo: KleverUtility.fontInfo(Config.viewFont)
    readonly property var codeFontInfo: KleverUtility.fontInfo(Config.codeFont)
    property var defaultCSS: {
        '--bodyColor': Config.viewBodyColor !== "None" ? Config.viewBodyColor : Kirigami.Theme.backgroundColor,
        '--font': viewFontInfo.family,
        '--fontSize': viewFontInfo.pointSize + "px",
        '--textColor': Config.viewTextColor !== "None" ? Config.viewTextColor : Kirigami.Theme.textColor,
        '--titleColor': Config.viewTitleColor !== "None" ? Config.viewTitleColor : Kirigami.Theme.disabledTextColor,
        '--linkColor': Config.viewLinkColor !== "None" ? Config.viewLinkColor : Kirigami.Theme.linkColor,
        '--visitedLinkColor': Config.viewVisitedLinkColor !== "None" ? Config.viewVisitedLinkColor : Kirigami.Theme.visitedLinkColor,
        '--codeColor': Config.viewCodeColor !== "None" ? Config.viewCodeColor : Kirigami.Theme.alternateBackgroundColor,
        '--codeFont': codeFontInfo.family,
        '--codeFontSize': codeFontInfo.pointSize + "px",
    }
    property bool printBackground: true
    property bool isInit: false

    spacing: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    onPathChanged: parser.notePath = path
    onTextChanged: root.parseText()

    onHighlightEnabledChanged: {
        parser.highlightEnabled = highlightEnabled 
        root.parseText()
    }

    onHighlighterStyleChanged: {
        parser.newHighlightStyle()
        root.parseText()
    }

    onNoteMapEnabledChanged: {
        parser.noteMapEnabled = noteMapEnabled
        root.parseText()
    }

    onDefaultCSSChanged: if (web_view.loadProgress === 100) changeStyle({})
    onStylePathChanged: if (web_view.loadProgress === 100) loadStyle()

    Parser { 
        id: parser

        onNewLinkedNotesInfos: noteMapper.addLinkedNotesInfos(linkedNotesInfos)
        onNoteHeadersSent: noteMapper.updatePathInfo(notePath, noteHeaders)
    }

    Kirigami.Card{
        id: background

        Layout.fillWidth: true
        Layout.fillHeight: true

        WebEngineView {
            id: web_view

            onJavaScriptConsoleMessage: function(level, message, lineNumber, sourceID) {console.error('WEB:', message, lineNumber, sourceID)}

            settings {
                showScrollBars: false
                localContentCanAccessFileUrls: true
                localContentCanAccessRemoteUrls: true
            }

            width: background.width - 4
            height: background.height - 4
            x: 2
            y: 2
            focus: true
            backgroundColor: "transparent"

            onPdfPrintingFinished: {
                const printingPage = applicationWindow().pageStack.currentItem
                printingPage.pdfPath = ""
                printingPage.pdfPath = root.previewLocation
            }

            onLoadProgressChanged: if (loadProgress === 100) {
                if (!root.isInit) {
                    loadStyle()
                    root.isInit = true
                    updateHtml()
                }

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
                        if (header[1] !== 0) parser.headerInfo = headerInfo

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
        }
    }

    ScrollBar {
        id: vbar

        hoverEnabled: true
        active: hovered || pressed
        orientation: Qt.Vertical
        size: background.height / web_view.contentsSize.height
        snapMode: ScrollBar.SnapAlways
        onPositionChanged: {
            if (active) {
                let scrollY = web_view.contentsSize.height * vbar.position
                web_view.runJavaScript("window.scrollTo(0," + scrollY + ")")
            }
        }

        Layout.row :0
        Layout.column: 1
        Layout.fillHeight: true
    }

    function updateHtml() {
        if (!root.defaultHtml) root.defaultHtml = DocumentHandler.readFile(":/index.html")

        let customHtml = '<style>\n'
            + root.completCss
            + '</style>\n'
            + parsedHtml
        
        let defaultHtml = root.defaultHtml
        const finishedHtml = defaultHtml.replace("INSERT HTML HERE", customHtml)

        web_view.loadHtml(finishedHtml, "file:/")
    }

    function parseText() {
        parsedHtml = parser.parse(text)
        updateHtml()
    }

    function changeStyle(styleDict: Object) {
        const emptyDict = Object.keys(styleDict).length === 0;
        styleDict = emptyDict ? defaultCSS : styleDict

        let varStartIndex
        let varEndIndex
        let newCssVar = ""
        let noBg
        let style = root.cssStyle

        for (const [cssVar, value] of Object.entries(styleDict)) {
            noBg = cssVar === "--bodyColor" && !root.printBackground

            varStartIndex = style.indexOf(cssVar)
            if (-1 < varStartIndex) {
                varEndIndex = style.indexOf(";", varStartIndex)

                newCssVar = noBg
                    ? cssVar + ": " + "undefined" + ";\n"
                    : cssVar + ": " + value + ";\n"

                style = style.substring(0, varStartIndex) + newCssVar + style.substring(varEndIndex)
            }
        }

        root.completCss = style
        updateHtml()
    }

    function loadStyle() {
        root.cssStyle = DocumentHandler.getCssStyle(root.stylePath)
        changeStyle({})
    }

    function makePdf() {
        web_view.printToPdf(root.previewLocation.replace("file://",""))
    }

    function scrollToHeader() {
        if (parser.headerLevel !== "0") {
            web_view.runJavaScript("document.getElementById('noteMapperScrollTo')",function(result) { 
                if (result) { // Seems redundant but it's mandatory due to the way the wayview handle loadProgress
                    web_view.runJavaScript("document.getElementById('noteMapperScrollTo').scrollIntoView()")
                    parser.headerInfo = ["", "0"]
                }
            ; })
        }
    }
}
