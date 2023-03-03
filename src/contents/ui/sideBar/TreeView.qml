// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3

import org.kde.Klever 1.0

Controls.ScrollView {
    id: tree

    width: parent.width

    property var model
    property QtObject currentlySelected: subEntryColumn.children[0]
    property var hierarchyAsker: []

    readonly property QtObject subEntryColumn: subEntryColumn


    onCurrentlySelectedChanged: {
        const mainWindow = applicationWindow()
        if(mainWindow.isMainPage()) {
            const mainPage = mainWindow.pageStack.currentItem
            mainPage.currentlySelected = currentlySelected
        }
    }

    onModelChanged: {
        for (var childIdx in subEntryColumn.children) {
            subEntryColumn.children[childIdx].destroy();
        }
        subEntryColumn.addRows(model)
    }

    SubEntryColumn {
        id: subEntryColumn

        delimiter : 1
    }

    Connections {
        target: StorageHandler

        function onStorageUpdated() {
            const holder = currentlySelected.parent
            const childrenList = holder.visibleChildren

            let nextSelected
            for (var childIdx = 0; childIdx < childrenList.length; childIdx++) {
                if (currentlySelected == childrenList[childIdx]){
                    if (childIdx-1 >= 0) nextSelected = childrenList[childIdx-1]
                    else if (childIdx+1 != childrenList.length) nextSelected = childrenList[childIdx+1]
                    else nextSelected = holder.parent
                    break
                }
            }
            currentlySelected.destroy()
            currentlySelected = nextSelected
        }
    }

    Connections {
        target: View

        function onHierarchySent(hierarchy) {
            if (tree.hierarchyAsker.length != 0) {

                const askerInfo = tree.hierarchyAsker.shift()

                const caller = askerInfo[0]
                if (caller == tree) {
                    tree.subEntryColumn.addRows(hierarchy)
                    return
                }
                console.log(askerInfo)
                const additionnalInfo = askerInfo[1]

                caller.addRow(hierarchy,...additionnalInfo)
            }
        }
    }

    onVisibleChanged: {
        const caller = tree
        tree.hierarchyAsker.push([caller])
        View.hierarchySupplier(Config.storagePath,-1)
    }
}
