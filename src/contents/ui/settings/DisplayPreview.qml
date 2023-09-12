// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import "qrc:/contents/ui/textEditor"

import org.kde.Klever 1.0


FormCard.FormCard {
    FormCard.FormComboBoxDelegate {
        text: i18nc("@label:combobox", "Style:")

        readonly property var styles: KleverUtility.getCssStylesList()
        readonly property var stylesNames: Object.keys(styles)
        readonly property var styleName: KleverUtility.getName(Config.stylePath).replace(".css", "")

        model: stylesNames
        currentIndex: stylesNames.indexOf(styleName)

        onCurrentValueChanged: {
            const path = styles[currentValue]

            if (Config.stylePath !== path) Config.stylePath = path;
        }
    }

    TextDisplay {
        id: displayer
        Layout.fillWidth: true
        Layout.preferredHeight: Kirigami.Units.gridUnit * 20

        visible: true

        path: "qrc:"
        text: DocumentHandler.readFile(":/demo_note.md")
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
            id: linkButton

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

        FormCard.FormTextFieldDelegate {
            id: fontDisplay

            label: i18nc("@label:textbox", "Font:")

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            text: (Config.viewFont !== "None") ? Config.viewFont : Kirigami.Theme.defaultFont.family
            font.family: text

            Layout.margins: 0
            Layout.preferredWidth: displayer.width / 2


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

