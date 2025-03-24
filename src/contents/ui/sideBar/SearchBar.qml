// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kitemmodels
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.labs.components as KirigamiComponents

import org.kde.Klever

KirigamiComponents.SearchPopupField {
    id: root

    required property var treeView
    required property bool inSideBar

    readonly property var listModel: treeView.model

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
            filterRowCallback: function (sourceRow, sourceParent) {
                const index = sourceModel.index(sourceRow, 0, sourceParent)
                const rowName = sourceModel.data(index, NoteTreeModel.NameRole)
                const rowPath = searchFilterProxyModel.sourceModel.data(index, NoteTreeModel.PathRole)

                return RegExp(root.text, "i").test(rowName) && (
                    inSideBar || (
                        rowPath !== treeView.currentClickedItem.path
                        && !rowPath.includes(treeView.currentClickedItem.path + "/") // Can't move to children
                        && rowPath !== treeView.currentClickedItem.parentPath // No point in moving to parent
                    )    
                )
            }
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
                    text: i18n("From") + ": " + niceParentPath()

                    font: Kirigami.Theme.smallFont
                    elide: Text.ElideMiddle
                    Layout.fillWidth: true

                    function niceParentPath(): string {
                        const parentPath = model.parentPath
                        let trimmedParentPath = parentPath.substring(Config.storagePath.length)

                        return trimmedParentPath.length === 0 ? '/' : trimmedParentPath
                    }
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

            function enterSelected(): void {
                const searchModelIndex = searchFilterProxyModel.mapToSource(searchFilterProxyModel.index(index,0))
                const noteFilderModelIndex = noteFilterProxyModel.mapToSource(noteFilterProxyModel.index(searchModelIndex.row, 0))
                const descendantsModelIndex = descendants.mapToSource(descendants.index(noteFilderModelIndex.row, 0))
                root.clickedIndex = descendantsModelIndex
                
                if (!inSideBar) {
                    root.selectedText = nameLabel.text
                    root.text = selectedText
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
        root.text = selectedText
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
