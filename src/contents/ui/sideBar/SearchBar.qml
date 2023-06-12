// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15

import org.kde.kitemmodels 1.0
import org.kde.kirigamiaddons.labs.components 1.0 as KirigamiComponents

KirigamiComponents.SearchPopupField {
    id: root

    required property var listModel
    property var clickedIndex

    onTextChanged: searchFilterProxyModel.setFilterFixedString(text)

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
            filterRole: "noteName"
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
                    Layout.fillWidth: true
                    text: i18n("From : ")+model.branchName
                    elide: Text.ElideRight
                    font: Kirigami.Theme.smallFont
                }
                Controls.Label {
                    Layout.fillWidth: true
                    text: model.displayName
                    font.bold: true
                    wrapMode: Text.WordWrap
                }
            }

            onClicked: {
                const searchModelIndex = searchFilterProxyModel.mapToSource(searchFilterProxyModel.index(index,0))

                const descendantsModelIndex = descendants.mapToSource(descendants.index(searchModelIndex.row, 0))
                root.clickedIndex = descendantsModelIndex
            }
        }

        Kirigami.PlaceholderMessage {
            text: i18n("No search results")
            visible: searchListView.count === 0
            icon.name: "system-search"
            anchors.centerIn: parent
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
