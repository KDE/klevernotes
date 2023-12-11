// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

/*
 * BASED ON :
 * https://invent.kde.org/graphics/koko/-/blob/master/src/qml/Sidebar.qml
 * 2017 Atul Sharma <atulsharma406@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.5 as Kirigami

import org.kde.Klever 1.0

Kirigami.OverlayDrawer {
    id: drawer

    readonly property NoteMapper noteMapper: applicationWindow().noteMapper
    readonly property NoteTreeModel treeModel: treeview.model

    edge: Qt.application.layoutDirection == Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    handleClosedIcon.source: null
    handleOpenIcon.source: null
    handleVisible: applicationWindow().isMainPage() && modal

    // Autohiding behavior
    modal: !applicationWindow().isMainPage() || !root.wideScreen
    onEnabledChanged: drawerOpen = enabled && !modal
    onModalChanged: drawerOpen = !modal
    // Prevent it to being close while in wideScreen
    closePolicy: modal ? Controls.Popup.CloseOnReleaseOutside : Controls.Popup.NoAutoClose

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    readonly property string storagePath: Config.storagePath
    onStoragePathChanged: if (storagePath !== "None") noteTreeModel.initModel()

    width: Kirigami.Units.gridUnit * 15

    Timer {
        id: focusTimer

        property var focusModelIndex

        interval: Kirigami.Units.longDuration
        repeat: false

        onTriggered: if (focusModelIndex) {
            focusModelIndex.model.askForFocus(focusModelIndex)
            focusModelIndex = undefined
        }
    }

    Timer {
        id: timer

        property var modelIndex

        interval: Kirigami.Units.longDuration
        repeat: false

        onTriggered: if (modelIndex) {
            modelIndex.model.askForExpand(modelIndex)
            modelIndex = undefined
            interval = Kirigami.Units.longDuration
            focusTimer.start()
        }
    }

    contentItem: ColumnLayout {
        id: column

        implicitWidth: Kirigami.Units.gridUnit * 15
        width: Kirigami.Units.gridUnit * 15

        ActionBar{
            id: actionBar

            treeView: treeview

            Layout.fillWidth: true
            Layout.preferredHeight: pageStack.globalToolBar.preferredHeight
        }

        TreeView {
            id: treeview

            sourceModel: NoteTreeModel {
                id: noteTreeModel

                noteMapEnabled: Config.noteMapEnabled

                onNewGlobalPathFound: drawer.noteMapper.addGlobalPath(path)
                onGlobalPathUpdated: drawer.noteMapper.updateGlobalPath(oldPath, newPath)
                onGlobalPathRemoved: drawer.noteMapper.removeGlobalPath(path)
                onInitialGlobalPathsSent: drawer.noteMapper.addInitialGlobalPaths(initialGlobalPaths)
                onErrorOccurred: applicationWindow().showPassiveNotification(errorMessage)
            }

            Layout.fillWidth: true
            Layout.fillHeight: true

            onCurrentItemChanged: {
                actionBar.currentModelIndex = treeview.descendantsModel.mapToSource(treeview.descendantsModel.index(treeview.currentIndex, 0))
            }
        }

        Controls.ToolSeparator {
            orientation: Qt.Horizontal

            Layout.topMargin: -1;
            Layout.fillWidth: true
            Layout.alignment:Qt.AlignBottom
        }

        Kirigami.BasicListItem {
            text: i18n("Cheat sheet")
            @KIRIGAMI_LISTITEM_ICON@: "text-markdown"

            Layout.alignment:Qt.AlignBottom

            onClicked: {
                applicationWindow().showCheatSheet()
                if (drawer.modal) drawer.close()
            }
        }

        Kirigami.BasicListItem {
            text: i18n("Settings")
            @KIRIGAMI_LISTITEM_ICON@: "settings-configure"

            Layout.alignment:Qt.AlignBottom

            onClicked: applicationWindow().switchToPage('Settings')
        }

        Kirigami.BasicListItem {
            text: i18n("About KleverNotes")
            @KIRIGAMI_LISTITEM_ICON@: "help-about"

            onClicked: applicationWindow().switchToPage('About')
        }
    }

    ContextMenu {
        id: contextMenu

        actionBar: actionBar
        treeView: treeview
    }

    Component.onCompleted: {
        if (storagePath === "None"){
            let component = Qt.createComponent("qrc:/contents/ui/dialogs/StorageDialog.qml")

            if (component.status == Component.Ready) {
                var dialog = component.createObject(applicationWindow());
                dialog.open()
            }
        }
    }

    function askForFocus(modelIndex) {
        let parentRowsList = []
        let currentModelIndex = modelIndex
        while (currentModelIndex.parent.row !== -1) {
            const nextModelIndex = currentModelIndex.parent
            parentRowsList.push(nextModelIndex)
            currentModelIndex = nextModelIndex
        }

        const firstModelIndex = parentRowsList[parentRowsList.length - 1]

        firstModelIndex.model.askForExpand(firstModelIndex)
        // This might be the exact same as "firstModelIndex" but is still needed for Category notes
        timer.modelIndex = parentRowsList[0]
        timer.start()

        focusTimer.focusModelIndex = modelIndex
    }
}


