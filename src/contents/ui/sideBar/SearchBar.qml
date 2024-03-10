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
    required property bool inSideBar
    required property string currentUseCase

    property var clickedIndex
    property bool textSelected: false
    property string selectedText: ""
    property string selectedUseCase: ""

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
                filterRoleName: inSideBar 
                    ? "useCase" 
                    : currentUseCase === "Note" 
                        ? "noteName"
                        : "useCase"
                filterString: inSideBar
                    ? "note"
                    : currentUseCase === "Note" 
                        ? ".Not a note"
                        : "category"
                filterCaseSensitivity: Qt.CaseInsensitive
            }
            filterRoleName: inSideBar ? "noteName" : "displayName"
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
                    id: useCaseLabel
                    readonly property var useCaseTrad: {
                        "Category": i18nc("Name, as in 'A note category'", "Category"),
                        "Group": i18nc("Name, as in 'A note group'", "Group"),
                    }
                    readonly property string branchNameStr: model.branchName
                    readonly property string branchNameContext: useCaseLabel.branchNameStr.length > 0 
                        ? " (" + i18n("From") + " : " + useCaseLabel.branchNameStr + ")"
                        : ""


                    text: root.inSideBar
                        ? i18n("From") + " : " + useCaseLabel.branchNameStr 
                        : useCaseTrad[model.useCase] + useCaseLabel.branchNameContext

                    font: Kirigami.Theme.smallFont
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
                Controls.Label {
                    id: nameLabel

                    text: model.displayName
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
                    root.selectedUseCase = useCaseLabel.text
                    root.text = selectedUseCase + ": " + selectedText
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
        selectedText = ""
        selectedUseCase = ""
        clickedIndex = null
    }
    // true only when we clear the search field
    onTextChanged: if (text.length === 0) {
        selectedText = ""
        selectedUseCase = ""
        clickedIndex = null
    }
}
