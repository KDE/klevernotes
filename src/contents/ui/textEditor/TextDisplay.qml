// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

// ORIGINALLY BASED ON : https://github.com/CrazyCxl/markdown-editor
// SPDX-FileCopyrightText: 2019 CrazyCxl <chenxiaolong0001@gmail.com>

import QtQuick 2.2
import QtQuick.Controls 2.2
import QtWebChannel
import QtWebEngine
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0
import qtMdEditor 1.0 as QtMdEditor

RowLayout {
    id: root

    readonly property var view: web_view

    required property string path
    required property string text
    
    // Syntax highlight
    readonly property bool highlightEnabled: Config.codeSynthaxHighlightEnabled // give us acces to a "Changed" signal
    readonly property string highlighterStyle: Config.codeSynthaxHighlighterStyle // This will also be triggered when the highlighter itself is changed
    // NoteMapper
    readonly property bool noteMapEnabled: Config.noteMapEnabled // give us acces to a "Changed" signal
    readonly property NoteMapper noteMapper: applicationWindow().noteMapper
    // Emoji
    readonly property bool emojiEnabled: Config.quickEmojiEnabled
    readonly property string emojiTone: Config.emojiTone
    // PlantUML
    readonly property bool pumlEnabled: Config.pumlEnabled
    readonly property bool pumlDark: Config.pumlDark

    readonly property Parser parser: parser
    readonly property string stylePath: Config.stylePath
    readonly property var codeFontInfo: KleverUtility.fontInfo(Config.codeFont)
    readonly property var viewFontInfo: KleverUtility.fontInfo(Config.viewFont)
    readonly property string previewLocation: StandardPaths.writableLocation(StandardPaths.TempLocation)+"/pdf-preview.pdf"
    readonly property string emptyPreview: (StandardPaths.writableLocation(StandardPaths.TempLocation)+"/empty.pdf").substring(7)

    readonly property var defaultCSS: {
        '--bodyColor': Config.viewBodyColor !== "None" ? Config.viewBodyColor : Kirigami.Theme.backgroundColor,
        '--font': viewFontInfo.family,
        '--fontSize': viewFontInfo.pointSize + "px",
        '--textColor': Config.viewTextColor !== "None" ? Config.viewTextColor : Kirigami.Theme.textColor,
        '--titleColor': Config.viewTitleColor !== "None" ? Config.viewTitleColor : Kirigami.Theme.disabledTextColor,
        '--linkColor': Config.viewLinkColor !== "None" ? Config.viewLinkColor : Kirigami.Theme.linkColor,
        '--visitedLinkColor': Config.viewVisitedLinkColor !== "None" ? Config.viewVisitedLinkColor : Kirigami.Theme.visitedLinkColor,
        '--codeColor': Config.viewCodeColor !== "None" ? Config.viewCodeColor : Kirigami.Theme.alternateBackgroundColor,
        '--highlightColor': Config.viewHighlightColor !== "None" ? Config.viewHighlightColor : Kirigami.Theme.highlightColor,
        '--codeFont': codeFontInfo.family,
        '--codeFontSize': codeFontInfo.pointSize + "px",
    }

    property string defaultHtml
    property string parsedHtml
    property string cssStyle
    property string completCss
    property bool printBackground: true

    spacing: 0

    onPathChanged: {
        parser.notePath = path
    }
    onTextChanged: {
        root.parseText()
    }
    onHighlightEnabledChanged: {
        root.parseText()
    }
    onHighlighterStyleChanged: {
        parser.newHighlightStyle()
        root.parseText()
    }
    onNoteMapEnabledChanged: {
        root.parseText()
    }
    onEmojiEnabledChanged: {
        root.parseText()
    }
    onEmojiToneChanged: {
        root.parseText()
    }
    onPumlEnabledChanged: {
        root.parseText()
    }
    onPumlDarkChanged: {
        parser.pumlDarkChanged()
        root.parseText()
    }
    onDefaultCSSChanged: if (web_view.loadProgress === 100) {
        changeStyle({})
    }
    onStylePathChanged: if (web_view.loadProgress === 100) {
        loadStyle()
    }
    Component.onCompleted: {
        loadBaseHtml()
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
            onLoadProgressChanged: if (loadProgress === 100) {
                loadStyle()
                parseText()
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

    Parser { 
        id: parser

        onNewLinkedNotesInfos: function(linkedNotesInfos) {
            noteMapper.addLinkedNotesInfos(linkedNotesInfos)
        }
        onNoteHeadersSent: function(notePath, noteHeaders) {
            noteMapper.updatePathInfo(notePath, noteHeaders)
        }
    }

    function loadBaseHtml() {
        if (!root.defaultHtml) root.defaultHtml = DocumentHandler.readFile(":/index.html")

        web_view.loadHtml(root.defaultHtml, "file:/")
    }

    function parseText() {
        if (web_view.loadProgress === 100) {
            parsedHtml = parser.parse(text)
            contentLink.text = parsedHtml
        }
    }

    function changeStyle(styleDict) {
        if (!styleDict) return
        const emptyDict = Object.keys(styleDict).length === 0;
        styleDict = emptyDict ? defaultCSS : styleDict

        let varStartIndex
        let varEndIndex
        let newCssVar = ""
        let noBg
        let style = root.cssStyle

        for (const key in defaultCSS) {
            if (!styleDict[key]) {
                styleDict[key] = defaultCSS[key]
            }
        }

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

        cssLink.text = style
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
            })
        }
    }
}
