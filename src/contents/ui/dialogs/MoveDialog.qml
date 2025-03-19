// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

import "qrc:/contents/ui/sideBar"

FormCard.FormCardDialog {
    id: textPromptDialog

    required property var treeView
    required property bool isNote

    readonly property var clickedIndex: searchBar.clickedIndex
    readonly property bool moveToRoot: moveToRootSwitch.checked

    title: i18nc("@title:dialog, to move a folder/note", "Move")

    standardButtons: Kirigami.Dialog.Apply | Kirigami.Dialog.Cancel

    width: Kirigami.Units.gridUnit * 20

    onOpened: {
        searchBar.forceActiveFocus()
    }
    onClosed: {
        searchBar.text = ""
    }

    padding: Kirigami.Units.largeSpacing * 2

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.largeSpacing * 2
        Label {
            id: subtitle
            text: isNote 
                ? i18nc("@subtitle:dialog", "Where do you want to move this note ?") 
                : i18nc("@subtitle:dialog", "Where do you want to move this folder ?")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        FormCard.FormDelegateSeparator { above: subtitle; below: moveToRootSwitch }

        FormCard.FormSwitchDelegate {
            id: moveToRootSwitch
            text: i18nc("@label:checkbox, move this folder/note to the root of the storage", "Move to storage root")
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
