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

    readonly property string stylePath: Config.stylePath
    readonly property string previewLocation: StandardPaths.writableLocation(StandardPaths.TempLocation)+"/pdf-preview.pdf"

    property string parsedHtml
    property string cssStyle
    property string completCss
    property var defaultCSS: {
        '--bodyColor': Config.viewBodyColor !== "None" ? Config.viewBodyColor : Kirigami.Theme.backgroundColor,
        '--font': Config.viewFont !== "None" ? Config.viewFont : Kirigami.Theme.defaultFont,
        '--textColor': Config.viewTextColor !== "None" ? Config.viewTextColor : Kirigami.Theme.textColor,
        '--titleColor': Config.viewTitleColor !== "None" ? Config.viewTitleColor : Kirigami.Theme.disabledTextColor,
        '--linkColor': Config.viewLinkColor !== "None" ? Config.viewLinkColor : Kirigami.Theme.linkColor,
        '--visitedLinkColor': Config.viewVisitedLinkColor !== "None" ? Config.viewVisitedLinkColor : Kirigami.Theme.visitedLinkColor,
        '--codeColor': Config.viewCodeColor !== "None" ? Config.viewCodeColor : Kirigami.Theme.alternateBackgroundColor,
    }
    property bool printBackground: true

    spacing: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    onPathChanged:  MDParser.notePath = path;
    onTextChanged: {
        text = text.length > 0 ? text : "\n"
        parsedHtml = MDParser.parse(text)
        updateHtml()
    }
    onDefaultCSSChanged: if (web_view.loadProgress === 100) changeStyle({})
    onStylePathChanged: if (web_view.loadProgress === 100) loadStyle()

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

            onLoadProgressChanged: if (loadProgress === 100) loadStyle()

            onScrollPositionChanged: if (!vbar.active) {
                vbar.position = scrollPosition.y / contentsSize.height
            }

            onNavigationRequested: function(request) {
                if (request.url.toString().startsWith("http")) {
                    Qt.openUrlExternally(request.url)
                    request.action = WebEngineNavigationRequest.IgnoreRequest
                }
            }

            Component.onCompleted: {
                let defaultHtml = DocumentHandler.readFile(":/index.html")
                web_view.loadHtml(defaultHtml, "file:/")
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

        Layout.row:0
        Layout.column:1
        Layout.fillHeight: true
    }

    function updateHtml() {
        let defaultHtml = DocumentHandler.readNote(":/index.html")

        let customHtml = '<style>\n'
            + root.completCss
            + '</style>\n'
            + parsedHtml

        const finishedHtml = defaultHtml.replace("INSERT HTML HERE", customHtml)

        web_view.loadHtml(finishedHtml, "file:/")
    }

    function changeStyle(styleDict: Object) {
        const emptyDict = Object.keys(styleDict).length === 0;
        const styleDict = emptyDict ? defaultCSS : style

        let varStartIndex
        let varEndIndex
        let newCssVar = ""
        let noBg
        let style = root.cssStyle
        for (const [cssVar, value] of Object.entries(defaultCSS)) {
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
        root.cssStyle = DocumentHandler.getCssStyle(stylePath)
        changeStyle({})
    }

    function makePdf() {
        web_view.printToPdf(root.previewLocation.replace("file://",""))
    }
}
