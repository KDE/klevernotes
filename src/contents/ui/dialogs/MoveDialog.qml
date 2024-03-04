// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.Klever 1.0

import "qrc:/contents/ui/sideBar"

Kirigami.PromptDialog {
    id: textPromptDialog

    required property var treeView
    required property string useCase
    readonly property var useCaseTrad: {
        "category": i18nc("Name, as in 'A note category'", "category"),
        "group": i18nc("Name, as in 'A note group'", "group"),
        "note": i18nc("Name, as in 'A note'", "note")
    }
    readonly property var clickedIndex: searchBar.clickedIndex

    title: i18nc("@title:dialog, to move a group/note", "Move")

    standardButtons: Kirigami.Dialog.Apply | Kirigami.Dialog.Cancel

    onOpened: {
        searchBar.forceActiveFocus()
    }
    onClosed: {
        searchBar.text = ""
    }

    ColumnLayout {
        Kirigami.Heading {
            text: i18nc("@subtitle:dialog, %1 can be 'group' (a note group) or 'note' (a note)",
                "Where do you want to move this %1 ?", useCase ? useCaseTrad[useCase.toLowerCase()] : "")
            level: 4

            wrapMode: Text.WordWrap

            Layout.fillWidth: true
        }

        SearchBar {
            id: searchBar

            listModel: textPromptDialog.treeView
            inSideBar: false
            currentUseCase: textPromptDialog.useCase

            Layout.fillWidth: true

            onClickedIndexChanged: if (clickedIndex) {
                searchBar.popup.close()
                return;
            }
        }
    }
}
