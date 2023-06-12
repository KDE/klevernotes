// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3

import org.kde.Klever 1.0

import org.kde.kirigamiaddons.treeview 1.0 as TreeView

TreeView.TreeListView {
    id: treeView

    onCurrentItemChanged: {
        const mainWindow = applicationWindow()
        if (mainWindow.isMainPage()) {
            const mainPage = mainWindow.pageStack.currentItem
            mainPage.currentlySelected = currentItem
        }
    }

    delegate: TreeItem {
        id: treeItem

        readonly property string path: model.path
        readonly property string useCase: model.useCase

        readonly property bool wantFocus: model.wantFocus
        property bool wantExpand: model.wantExpand

        onWantFocusChanged: if (wantFocus) {
            const sourceIndex = decoration.model.mapToSource(decoration.model.index(index, 0));
            const newIndex = decoration.model.mapFromSource(sourceIndex);

            treeView.currentIndex = newIndex.row
        }
        onWantExpandChanged: if (wantExpand) {
            if (!kDescendantExpanded) {
                decoration.model.toggleChildren(index)
            } else {
                actionBar.timer.interval = 0
            }
            actionBar.timer.start()
        }


        label: model.displayName
        icon.name: model.iconName
    }
}
