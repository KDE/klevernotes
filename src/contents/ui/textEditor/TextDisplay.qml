// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

// This is based on https://github.com/CrazyCxl/markdown-editor
// SPDX-License-Identifier: GPL-3.0


import QtQuick 2.2
import QtQuick.Controls 2.2
import QtWebChannel 1.0
import QtWebEngine 1.6
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import Qt.labs.platform 1.1

import qtMdEditor 1.0 as QtMdEditor
import org.kde.Klever 1.0

RowLayout {
    id: root

    required property string path
    required property string text

    property var defaultCSS: {
        '--bodyColor': Config.viewBodyColor !== "None" ? Config.viewBodyColor : Kirigami.Theme.backgroundColor,
        '--font': Config.viewFont !== "None" ? Config.viewFont : Kirigami.Theme.defaultFont,
        '--textColor': Config.viewTextColor !== "None" ? Config.viewTextColor : Kirigami.Theme.textColor,
        '--titleColor': Config.viewTitleColor !== "None" ? Config.viewTitleColor : Kirigami.Theme.disabledTextColor,
        '--linkColor': Config.viewLinkColor !== "None" ? Config.viewLinkColor : Kirigami.Theme.linkColor,
        '--visitedLinkColor': Config.viewVisitedLinkColor !== "None" ? Config.viewVisitedLinkColor : Kirigami.Theme.visitedLinkColor,
        '--codeColor': Config.viewCodeColor !== "None" ? Config.viewCodeColor : Kirigami.Theme.alternateBackgroundColor,
    }

    spacing:0

    onDefaultCSSChanged: if(web_view.loadProgress === 100) changeStyle()

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    Kirigami.Card{
        id:background

        Layout.fillWidth: true
        Layout.fillHeight: true

        WebEngineView{
            id:web_view

            onJavaScriptConsoleMessage: console.error('WEB:', message, lineNumber, sourceID)

            settings.showScrollBars:false

            width: background.width - 4
            height:background.height - 4
            x: 2
            y: 2
            url: "qrc:/index.html"
            focus: true

            webChannel: WebChannel{
                registeredObjects: [editorLink, cssLink, homePathPasser, notePathPasser]
            }

            onLoadProgressChanged: if (loadProgress === 100) {
                changeStyle()
            }

            QtMdEditor.QmlLinker{
                id: editorLink
                text: root.text
                WebChannel.id: "linkToEditor"
            }

            QtMdEditor.QmlLinker{
                id: cssLink
                css: {"key":"value"}
                WebChannel.id: "linkToCss"
            }

            QtMdEditor.QmlLinker{
                id: homePathPasser
                homePath: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0].substring("file://".length)
                WebChannel.id: "homePathPasser"
            }

            QtMdEditor.QmlLinker{
                id: notePathPasser
                notePath: root.path
                WebChannel.id: "notePathPasser"
            }
        }

        MouseArea{
            anchors.fill:web_view
            enabled:true

            onWheel:{
                (wheel.angleDelta.y > 0)
                    ? vbar.decrease()
                    : vbar.increase()
            }

            // Use to make some test

             onClicked: web_view.runJavaScript("document.body.innerHTML",function(result) { console.log(result); });

        }
    }

    ScrollBar {
        id: vbar

        hoverEnabled: true
        active: hovered || pressed
        orientation: Qt.Vertical
        size: background.height / web_view.contentsSize.height
        stepSize: 0.03
        snapMode: ScrollBar.SnapOnRelease
        onPositionChanged:  {
            let scrollY = web_view.contentsSize.height*vbar.position + 5
            web_view.runJavaScript("window.scrollTo(0,"+scrollY+")")
        }

        Layout.row:0
        Layout.column:1
        Layout.fillHeight: true
    }

    function changeStyle() {
        cssLink.css = defaultCSS
    }
}
