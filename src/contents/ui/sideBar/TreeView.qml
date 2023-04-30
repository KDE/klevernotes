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

    readonly property QtObject deleteConfirmationDialog: deleteConfirmationDialog
    property QtObject actionBar : undefined

    onCurrentItemChanged: {
        const mainWindow = applicationWindow()
        if(mainWindow.isMainPage()) {
            const mainPage = mainWindow.pageStack.currentItem
            mainPage.currentlySelected = currentItem
        }
    }

    delegate: TreeView.BasicTreeItem {
        id: treeItem

        property int level: model.level
        property string path: model.path
        property string useCase: model.useCase
        property int index: model.index

        label: model.displayedName
        icon.name: model.iconName

        MouseArea {
            // For whatever reason fillWidth/Height doesn't work
            Layout.preferredWidth: contentItem.width
            Layout.preferredHeight: contentItem.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onClicked: {
                treeItem.clicked()
                if (mouse.button === Qt.RightButton) {
                    contextMenu.canDelete = !treeItem.path.endsWith("/.BaseCategory")
                    contextMenu.popup()
                }
            }
        }
    }
}
