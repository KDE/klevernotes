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
            name: "text"
            title: i18nc("@label:button", "Text color:")

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: (Config.viewTextColor !== "None") ? Config.viewTextColor : Kirigami.Theme.textColor

            Layout.preferredWidth: displayer.width / 2
        }

        SettingsColorButton {
            name: "title"
            title: i18nc("@label:button", "Title color:")

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: (Config.viewTitleColor !== "None") ? Config.viewTitleColor : Kirigami.Theme.disabledTextColor

            Layout.preferredWidth: displayer.width / 2
        }
    }
    RowLayout {
        SettingsColorButton {
            name: "link"
            title: i18nc("@label:button", "Link color:")

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: (Config.viewLinkColor !== "None") ? Config.viewLinkColor : Kirigami.Theme.linkColor

            Layout.preferredWidth: displayer.width / 2
        }

        SettingsColorButton {
            name: "visitedLink"
            title: i18nc("@label:button", "Visited Link color:")

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: (Config.viewVisitedLinkColor !== "None") ? Config.viewVisitedLinkColor : Kirigami.Theme.visitedLinkColor

            Layout.preferredWidth: displayer.width / 2
        }
    }

    RowLayout {
        SettingsColorButton {
            name: "code"
            title: i18nc("@label:button", "Code color:")

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: (Config.viewCodeColor !== "None") ? Config.viewCodeColor : Kirigami.Theme.alternateBackgroundColor

            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: displayer.width / 2
        }

       SettingsColorButton {
            name: "highlight"
            title: i18nc("@label:button", "Highlight color:")

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: (Config.viewHighlightColor !== "None") ? Config.viewHighlightColor : Kirigami.Theme.highlightColor

            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: displayer.width / 2
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
