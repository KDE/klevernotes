// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kitemmodels 1.0
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.labs.components 1.0 as KirigamiComponents

KirigamiComponents.SearchPopupField {
    id: root

    required property var listModel

    property var clickedIndex

    spaceAvailableLeft: false
    spaceAvailableRight: false

    popupContentItem: ListView {
        id: searchListView

        KSortFilterProxyModel {
            id: searchFilterProxyModel

            sourceModel: KDescendantsProxyModel {
                id: descendants
                model: root.listModel
            }
            @FILTER_NAMING@: "noteName"
            filterCaseSensitivity: Qt.CaseInsensitive
        }

        model: (root.text === "") ? null : searchFilterProxyModel

        delegate: Controls.ItemDelegate {
            id: searchDelegate

            width: ListView.view.width

            highlighted: activeFocus

            leftInset: 1
            rightInset: 1

            contentItem: ColumnLayout {
                Controls.Label {
                    text: i18n("From : ")+model.branchName
                    font: Kirigami.Theme.smallFont
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                Controls.Label {
                    text: model.displayName
                    wrapMode: Text.WordWrap
                    font.bold: true
                    Layout.fillWidth: true
                }
            }

            onClicked: {
                const searchModelIndex = searchFilterProxyModel.mapToSource(searchFilterProxyModel.index(index,0))

                const descendantsModelIndex = descendants.mapToSource(descendants.index(searchModelIndex.row, 0))
                root.clickedIndex = descendantsModelIndex
            }
        }

        Kirigami.PlaceholderMessage {
            width: parent.width - Kirigami.Units.gridUnit * 4
            anchors.centerIn: parent

            text: i18n("No search results")
            visible: searchListView.count === 0
            icon.name: "system-search"
        }
    }

    onTextChanged: {
        searchFilterProxyModel.setFilterFixedString(text)
    }
}
