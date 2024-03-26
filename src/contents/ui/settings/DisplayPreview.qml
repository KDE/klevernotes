// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import "qrc:/contents/ui/textEditor"

import org.kde.Klever 1.0

FormCard.FormCard {
    FormCard.FormComboBoxDelegate {
        readonly property var styles: KleverUtility.getCssStylesList()
        readonly property var stylesNames: Object.keys(styles)
        readonly property var styleName: KleverUtility.getName(Config.stylePath).replace(".css", "")

        text: i18nc("@label:combobox", "Style:")

        model: stylesNames
        currentIndex: stylesNames.indexOf(styleName)

        onCurrentValueChanged: {
            const path = styles[currentValue]

            if (Config.stylePath !== path) Config.stylePath = path;
        }
    }

    TextDisplay {
        id: displayer

        path: "qrc:"
        text: DocumentHandler.readFile(":/demo_note.md")

        visible: true

        Layout.fillWidth: true
        Layout.preferredHeight: Kirigami.Units.gridUnit * 20
    }

    RowLayout {
        SettingsColorButton {
            readonly property string configColor: Config.viewTextColor

            name: "text"
            title: i18nc("@label:button", "Text color:")

            Layout.preferredWidth: displayer.width / 2

            onConfigColorChanged: {
                color = configColor !== "None" ? configColor : Kirigami.Theme.textColor
            }
        }

        SettingsColorButton {
            readonly property string configColor: Config.viewTitleColor

            name: "title"
            title: i18nc("@label:button", "Title color:")

            Layout.preferredWidth: displayer.width / 2

            onConfigColorChanged: {
                color = configColor !== "None" ? configColor : Kirigami.Theme.disabledTextColor
            }
        }
    }
    RowLayout {
        SettingsColorButton {
            readonly property string configColor: Config.viewLinkColor

            name: "link"
            title: i18nc("@label:button", "Link color:")

            Layout.preferredWidth: displayer.width / 2
 
            onConfigColorChanged: {
                color = configColor !== "None" ? configColor : Kirigami.Theme.linkColor
            }
        }

        SettingsColorButton {
            readonly property string configColor: Config.viewVisitedLinkColor

            name: "visitedLink"
            title: i18nc("@label:button", "Visited Link color:")

            Layout.preferredWidth: displayer.width / 2

            onConfigColorChanged: {
                color = configColor !== "None" ? configColor : Kirigami.Theme.visitedLinkColor
            }
        }
    }

    RowLayout {
        SettingsColorButton {
            readonly property string configColor: Config.viewCodeColor

            name: "code"
            title: i18nc("@label:button", "Code color:")

            // Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: displayer.width / 2

            onConfigColorChanged: {
                color = configColor !== "None" ? configColor : Kirigami.Theme.alternateBackgroundColor
            }
        }

       SettingsColorButton {
            readonly property string configColor: Config.viewHighlightColor

            name: "highlight"
            title: i18nc("@label:button", "Highlight color:")

            // Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: displayer.width / 2

            onConfigColorChanged: {
                color = configColor !== "None" ? configColor : Kirigami.Theme.highlightColor
            }
        } 
    }

    FontPicker {
        label: i18nc("@label:textbox", "General font:")
        configFont: Config.viewFont

        Layout.fillWidth: true

        onNewFontChanged: if (text !== newFont) {
            Config.viewFont = newFont 
        }
    }

    FontPicker {
        label: i18nc("@label:textbox", "Code block font:")
        configFont: Config.codeFont

        Layout.fillWidth: true

        onNewFontChanged: if (text !== newFont) {
            Config.codeFont = newFont 
        }
    }
}
