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

import qtMdEditor 1.0 as QtMdEditor
import org.kde.Klever 1.0

RowLayout {
    id: frame

    spacing:0

    property string text
    property var defaultCSS: {
            '--body-color':`${Kirigami.Theme.backgroundColor}`,
            '--font': `${Kirigami.Theme.defaultFont}`,
            '--textColor': `${Kirigami.Theme.textColor}`,
            '--titleColor': `${Kirigami.Theme.disabledTextColor}`,
            '--linkColor': `${Kirigami.Theme.linkColor}`,
            '--visitedLinkColor': `${Kirigami.Theme.visitedLinkColor}`,
            '--codeColor': `${Kirigami.Theme.alternateBackgroundColor}`,
        };
    onDefaultCSSChanged: if(web_view.loadProgress === 100) changeStyle()

    onTextChanged: if(web_view.loadProgress === 100) editorLink.text = text

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    Kirigami.Card{
        id:background

        Layout.fillWidth: true
        Layout.fillHeight: true

        WebEngineView{
            id:web_view

            settings.showScrollBars:false

            width: background.width - 4
            height:background.height - 4
            x: 2
            y: 2
            url: "qrc:/index.html"
            focus: true
            webChannel:WebChannel{
                registeredObjects:[editorLink,cssLink]
            }

            QtMdEditor.QmlLinker{
                id:editorLink
                WebChannel.id:"linkToEditor"
            }

            QtMdEditor.QmlLinker{
                id:cssLink
                css: {"key":"value"}
                WebChannel.id:"linkToCss"
            }

            onLoadProgressChanged: if (loadProgress === 100) {
                changeStyle()
                // Dirty workaround
                editorLink.text = text
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
