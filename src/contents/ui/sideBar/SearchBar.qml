// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kitemmodels
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.labs.components as KirigamiComponents

KirigamiComponents.SearchPopupField {
    id: root

    required property var listModel
    required property bool inSideBar
    required property string currentUseCase

    property var clickedIndex
    property bool textSelected: false
    property string selectedText: ""

    spaceAvailableLeft: false
    spaceAvailableRight: false

    popupContentItem: ListView {
        id: searchListView

        KSortFilterProxyModel {
            id: searchFilterProxyModel

            sourceModel: KSortFilterProxyModel {
                id: noteFilterProxyModel
                sourceModel: KDescendantsProxyModel {
                    id: descendants
                    model: root.listModel
                }
                filterRoleName: "isNote" 
                filterString: inSideBar ? "true" : "false" // Seems silly but we need a string
                filterCaseSensitivity: Qt.CaseInsensitive
            }
            filterRoleName: "name"
            filterCaseSensitivity: Qt.CaseInsensitive
        }

        clip: true
        model: (root.text === "") ? null : searchFilterProxyModel

        delegate: Controls.ItemDelegate {
            id: searchDelegate

            width: ListView.view.width

            highlighted: activeFocus

            leftInset: 1
            rightInset: 1

            contentItem: ColumnLayout {
                Controls.Label {
                    text: i18n("From") + ": " + model.parentPath

                    font: Kirigami.Theme.smallFont
                    elide: Text.ElideMiddle
                    Layout.fillWidth: true
                }
                Controls.Label {
                    id: nameLabel

                    text: model.name
                    wrapMode: Text.WordWrap
                    font.bold: true
                    Layout.fillWidth: true
                }
            }

            Keys.onReturnPressed: {
                enterSelected()
            }
            onClicked: {
                enterSelected()
            }

            function enterSelected() {
                const searchModelIndex = searchFilterProxyModel.mapToSource(searchFilterProxyModel.index(index,0))
                const noteFilderModelIndex = noteFilterProxyModel.mapToSource(noteFilterProxyModel.index(searchModelIndex.row, 0))
                const descendantsModelIndex = descendants.mapToSource(descendants.index(noteFilderModelIndex.row, 0))
                root.clickedIndex = descendantsModelIndex
                
                if (!inSideBar) {
                    root.selectedText = nameLabel.text
                }
            }
        }

        Kirigami.PlaceholderMessage {
            width: parent.width - Kirigami.Units.gridUnit * 4
            anchors.centerIn: parent

            text: i18n("No search results")
            visible: searchListView.count === 0
            icon.name: "system-search-symbolic"
        }
    }

    onFieldFocusChanged: if (fieldFocus && selectedText.length !== 0) {
        //root.text = selectedText
        searchFilterProxyModel.setFilterFixedString(text)
    }
    // Doesn't triggered when changing text to selected item
    // see: https://doc.qt.io/qt-6/qml-qtquick-textinput.html#textEdited-signal
    searchField.onTextEdited: {
        searchFilterProxyModel.setFilterFixedString(text)
        clickedIndex = null
    }
    // true only when we clear the search field
    onTextChanged: if (text.length === 0) {
        clickedIndex = null
    }
}
