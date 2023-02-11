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
    readonly property QtObject subEntryColumn: subEntryColumn

    SubEntryColumn {
        id: subEntryColumn

        delimiter : 1
    }

    onCurrentlySelectedChanged: {
        const mainWindow = applicationWindow()
        if(mainWindow.isMainPage()) {
            const mainPage = mainWindow.pageStack.currentItem
            mainPage.currentlySelected = currentlySelected
        }
    }

    onModelChanged: subEntryColumn.addRows(model)
}
