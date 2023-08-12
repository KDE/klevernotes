// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

import "qrc:/contents/ui/textEditor"

import org.kde.Klever 1.0


RowLayout {
    spacing: 0

    TextDisplay {
        id: displayer
        Layout.fillHeight: true
        Layout.fillWidth: true

        visible: true

        path: "qrc:"
        text: DocumentHandler.readFile(":/demo_note.md")
    }

    Kirigami.FormLayout {
        Layout.maximumWidth: Kirigami.Units.gridUnit * 8

        Controls.ComboBox {
            Kirigami.FormData.label: i18n("Style:")

            readonly property var styles: KleverUtility.getCssStylesList()
            readonly property var stylesNames: Object.keys(styles)
            readonly property var styleName: KleverUtility.getName(Config.stylePath).replace(".css", "")

            model: stylesNames
            width: Kirigami.Units.gridUnit * 7
            currentIndex: stylesNames.indexOf(styleName)

            onCurrentValueChanged: {
                const path = styles[currentValue]

                if (Config.stylePath !== path) Config.stylePath = path;
            }
        }

        Row {
            Kirigami.FormData.label: i18n("Text color:")

            property string name: "text"

            Controls.Button {
                id: textButton

                width: Kirigami.Units.gridUnit * 7

                anchors.bottom: resetText.bottom
                anchors.top: resetText.top
                anchors.margins: Kirigami.Units.smallSpacing

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                property string color: (Config.viewTextColor !== "None") ? Config.viewTextColor : Kirigami.Theme.textColor

                background: Rectangle {
                    color: textButton.color
                    radius: Kirigami.Units.smallSpacing
                }

                onClicked: {
                    colorPicker.caller = textButton
                    colorPicker.open()
                }
            }

            Controls.Button {
                id: resetText
                icon.name: "edit-undo"

                onClicked: updateColor(resetText, "None")
            }
        }

        Row {
            Kirigami.FormData.label: i18n("Title color:")

            property string name: "title"

            Controls.Button {
                id: titleButton

                width: Kirigami.Units.gridUnit * 7

                anchors.bottom: resetTitle.bottom
                anchors.top: resetTitle.top
                anchors.margins: Kirigami.Units.smallSpacing

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                property string color: (Config.viewTitleColor !== "None") ? Config.viewTitleColor : Kirigami.Theme.disabledTextColor

                background: Rectangle {
                    color: titleButton.color
                    radius: Kirigami.Units.smallSpacing
                }

                onClicked: {
                    colorPicker.caller = titleButton
                    colorPicker.open()
                }
            }

            Controls.Button {
                id: resetTitle
                icon.name: "edit-undo"

                onClicked: updateColor(resetTitle, "None")
            }
        }

        Row {
            Kirigami.FormData.label: i18n("Link color:")

            property string name: "link"

            Controls.Button {
                id: linkButton

                width: Kirigami.Units.gridUnit * 7

                anchors.bottom: resetLink.bottom
                anchors.top: resetLink.top
                anchors.margins: Kirigami.Units.smallSpacing

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                property string color: (Config.viewLinkColor !== "None") ? Config.viewLinkColor : Kirigami.Theme.linkColor

                background: Rectangle {
                    color: linkButton.color
                    radius: Kirigami.Units.smallSpacing
                }

                onClicked: {
                    colorPicker.caller = linkButton
                    colorPicker.open()
                }
            }

            Controls.Button {
                id: resetLink
                icon.name: "edit-undo"

                onClicked: updateColor(resetLink, "None")
            }
        }

        Row {
            Kirigami.FormData.label: i18n("Visited Link color:")

            property string name: "visitedLink"

            Controls.Button {
                id: visitedLinkButton

                width: Kirigami.Units.gridUnit * 7

                anchors.bottom: resetVisitiedLink.bottom
                anchors.top: resetVisitiedLink.top
                anchors.margins: Kirigami.Units.smallSpacing

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                property string color: (Config.viewVisitedLinkColor !== "None") ? Config.viewVisitedLinkColor : Kirigami.Theme.visitedLinkColor

                background: Rectangle {
                    color: visitedLinkButton.color
                    radius: Kirigami.Units.smallSpacing
                }
                onClicked: {
                    colorPicker.caller = visitedLinkButton
                    colorPicker.open()
                }
            }

            Controls.Button {
                id: resetVisitiedLink
                icon.name: "edit-undo"

                onClicked: updateColor(resetVisitiedLink, "None")
            }
        }

        Row {
            Kirigami.FormData.label: i18n("Code color:")

            property string name: "code"

            Controls.Button {
                id: codeButton

                width: Kirigami.Units.gridUnit * 7

                anchors.bottom: resetCode.bottom
                anchors.top: resetCode.top
                anchors.margins: Kirigami.Units.smallSpacing

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                property string color: (Config.viewCodeColor !== "None") ? Config.viewCodeColor : Kirigami.Theme.alternateBackgroundColor

                background: Rectangle {
                    color: codeButton.color
                    radius: Kirigami.Units.smallSpacing
                }

                onClicked: {
                    colorPicker.caller = codeButton
                    colorPicker.open()
                }
            }

            Controls.Button {
                id: resetCode
                icon.name: "edit-undo"

                onClicked: updateColor(resetCode, "None")
            }
        }

        Controls.TextField {
            id: fontDisplay

            Kirigami.FormData.label: i18n("Font:")

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false

            readOnly: true
            Layout.preferredWidth: Kirigami.Units.gridUnit * 7
            text: (Config.viewFont !== "None") ? Config.viewFont : Kirigami.Theme.defaultFont.family
            font.family: text

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    fontDialog.currentFamily = parent.text
                    fontDialog.open()
                }
            }
        }
    }
}

