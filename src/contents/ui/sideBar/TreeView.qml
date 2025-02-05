// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls as Controls

import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigami as Kirigami
import org.kde.kitemmodels

import org.kde.Klever

Controls.ScrollView {
    id: scrollView

    readonly property color backgroundColor: Kirigami.Theme.backgroundColor

    property alias model: descendantsModel.model
    property alias currentItem: treeView.currentItem
    property alias currentIndex: treeView.currentIndex
    property alias descendantsModel: descendantsModel

    property var currentModelIndex // Using var instead of QModelIndex
    property TreeItem currentClickedItem
    
    ListView {
        id: treeView

        clip: true
        model: KDescendantsProxyModel {
            id: descendantsModel
            expandsByDefault: false
        }

        property bool _hasBeenClicked: false

        delegate: TreeItem {
            id: treeItem

            // Without this the first item in the TreeView doesn't have the right color
            Kirigami.Theme.backgroundColor: scrollView.backgroundColor

            onItemRightClicked: {
                setClickedItemInfo(treeItem)
            }
            onWantFocusChanged: if (wantFocus) {
                clicked()
            }
            onWantExpandChanged: if (wantExpand) {
                descendantsModel.expandChildren(index)
            }
            onClicked: {
                treeView._hasBeenClicked = true
                descendantsModel.toggleChildren(index)
                forceActiveFocus()
                const mainWindow = applicationWindow()
                if (mainWindow.isMainPage()) {
                    const mainPage = mainWindow.pageStack.currentItem
                    mainPage.currentlySelected = treeItem
                    treeView.currentIndex = index
                }
            }
        }
    }

    onCurrentItemChanged: setClickedItemInfo(currentItem) 

    function setClickedItemInfo(item: TreeItem): void {
        currentClickedItem = item
        currentModelIndex = getModelIndex(item.index) 
    }

    function useCurrentItem(): void {
        setClickedItemInfo(treeView.currentItem)
    }

    function getModelIndex(rowIndex: int): var {
        return descendantsModel.mapToSource(descendantsModel.index(rowIndex, 0))
    }
}
