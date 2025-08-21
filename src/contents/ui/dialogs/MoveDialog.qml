// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.klevernotes

FormCard.FormCardDialog {
    id: textPromptDialog

    required property var treeView
    required property bool isNote
    required property string itemName

    readonly property var clickedIndex: searchBar.clickedIndex
    readonly property bool moveToRoot: moveToRootSwitch.checked

    title: xi18nc("@title:dialog, to move a folder/note", "Move <filename>%1</filename>?", itemName)

    standardButtons: Kirigami.Dialog.Apply | Kirigami.Dialog.Cancel

    width: Kirigami.Units.gridUnit * 20

    onClosed: {
        searchBar.text = ""
        searchBar.selectedText = ""
    }

    contentItem: ColumnLayout {
        spacing: 0 
        Label {
            id: subtitle
            text: isNote 
                ? i18nc("@subtitle:dialog", "Where do you want to move this note ?") 
                : i18nc("@subtitle:dialog", "Where do you want to move this folder ?")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
            Layout.bottomMargin: Kirigami.Units.gridUnit
        }

        FormCard.FormSwitchDelegate {
            id: moveToRootSwitch
            text: i18nc("@label:checkbox, move this folder/note to the root of the storage", "Move to storage root")
            Layout.fillWidth: true
        }

        FormCard.AbstractFormDelegate {
            Layout.fillWidth: true
            onClicked: searchBar.forceActiveFocus()

            padding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing

            contentItem: ColumnLayout {
                spacing: Kirigami.Units.largeSpacing
                Label {
                    text: i18nc("@label:dialog", "Search for a folder:")
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                SearchBar {
                    id: searchBar

                    enabled: !moveToRootSwitch.checked

                    treeView: textPromptDialog.treeView
                    inSideBar: false

                    Layout.fillWidth: true

                    onClickedIndexChanged: if (clickedIndex) {
                        searchBar.popup.close()
                        return;
                    }
                }
            }
        }
    }
}
