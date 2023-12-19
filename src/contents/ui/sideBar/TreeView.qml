// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls 2 as Controls

import org.kde.kirigamiaddons.delegates 1 as Delegates
import org.kde.kirigami 2.5 as Kirigami
import org.kde.kitemmodels 1

import org.kde.Klever 1.0

Controls.ScrollView {
    id: scrollView

    property alias model: descendantsModel.model
    property alias currentItem: treeView.currentItem
    property alias currentIndex: treeView.currentIndex
    property alias descendantsModel: descendantsModel
    ListView {
        id: treeView

        model: KDescendantsProxyModel {
            id: descendantsModel
        } 

        delegate: TreeItem {
            id: treeItem

            onWantFocusChanged: if (wantFocus) {
                clicked()
            }
            onWantExpandChanged: if (wantExpand) {
                if (!kDescendantExpanded) {
                    descendantsModel.toggleChildren(index)
                }
                drawer.timer.start()
            }
            onClicked: function (mouse) {
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
}
