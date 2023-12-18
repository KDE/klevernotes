// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import org.kde.kirigamiaddons.treeview 1.0 as TreeView

import org.kde.Klever 1.0

TreeView.TreeListView {
    id: treeView

    delegate: TreeItem {
        id: treeItem

        readonly property string path: model.path
        readonly property string useCase: model.useCase
        readonly property bool wantFocus: model.wantFocus

        property bool wantExpand: model.wantExpand

        label: model.displayName
        icon.name: model.iconName

        onWantFocusChanged: if (wantFocus) {
            const sourceIndex = decoration.model.mapToSource(decoration.model.index(index, 0));
            const newIndex = decoration.model.mapFromSource(sourceIndex);

            treeView.currentIndex = newIndex.row
        }
        onWantExpandChanged: if (wantExpand) {
            if (!kDescendantExpanded) {
                decoration.model.toggleChildren(index)
            } else {
                drawer.timer.interval = 0
            }
            drawer.timer.start()
        }
    }
    onCurrentItemChanged: {
        const mainWindow = applicationWindow()
        if (mainWindow.isMainPage()) {
            const mainPage = mainWindow.pageStack.currentItem
            mainPage.currentlySelected = currentItem
        }
    }
}
