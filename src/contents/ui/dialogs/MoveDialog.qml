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
    required property string useCase
    readonly property var useCaseTrad: {
        "group": i18nc("@subtitle:dialog, as in 'A note group'", "Where do you want to move this group ?"),
        "note": i18nc("@subtitle:dialog", "Where do you want to move this note ?") 
    }
    readonly property var clickedIndex: searchBar.clickedIndex

    title: i18nc("@title:dialog, to move a group/note", "Move")

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
            text: useCase && useCase !== "Category" ? useCaseTrad[useCase.toLowerCase()] : ""
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        SearchBar {
            id: searchBar

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
