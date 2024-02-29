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

import org.kde.kirigamiaddons.delegates 1.0 as Delegates
import org.kde.kirigami 2.5 as Kirigami

import org.kde.Klever 1.0

Kirigami.OverlayDrawer {
    id: drawer

    readonly property NoteMapper noteMapper: applicationWindow().noteMapper
    readonly property NoteTreeModel treeModel: treeView.model
    readonly property string storagePath: Config.storagePath
    readonly property int narrowWidth: Kirigami.Units.gridUnit * 3
    readonly property int largeWidth: Kirigami.Units.gridUnit * 15

    property bool isWide: true
    property bool changeWidth: true

    width: isWide ? largeWidth : narrowWidth

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    edge: Qt.application.layoutDirection == Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    handleClosedIcon.source: handleOpenIcon.source
    handleOpenIcon.source: null
    handleVisible: applicationWindow().isMainPage() && modal

    modal: Kirigami.Settings.isMobile 
    // Prevent it to being close while in wideScreen
    closePolicy: modal ? Controls.Popup.CloseOnReleaseOutside : Controls.Popup.NoAutoClose

    contentItem: Item {
        id: column

        ActionBar {
            id: actionBar

            height: pageStack.globalToolBar.preferredHeight
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }    

            treeView: treeView
            sideBarWide: drawer.isWide
        }
        
        Loader {
            active: !drawer.isWide
            anchors {
                top: actionBar.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            sourceComponent: ColumnLayout {
                anchors.fill: parent

                Controls.ToolButton {
                    icon.name: "file-catalog-symbolic"

                    Layout.fillWidth: true
                    Layout.preferredHeight: drawer.narrowWidth

                    Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                    Controls.ToolTip.visible: hovered
                    Controls.ToolTip.text: i18n("Show notes")

                    onClicked: {
                        drawer.close()
                    }
                }

                Item {
                    id: spaceTaker

                    Layout.fillHeight: true
                }

                Controls.ToolButton {
                    icon.name: "text-markdown"

                    Layout.fillWidth: true
                    Layout.preferredHeight: drawer.narrowWidth

                    Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                    Controls.ToolTip.visible: hovered
                    Controls.ToolTip.text: i18n("Cheat sheet")

                    onClicked: {
                        applicationWindow().showCheatSheet()
                        if (drawer.modal) drawer.close()
                    }
                }

                Controls.ToolButton {
                    icon.name: "settings-configure-symbolic"

                    Layout.fillWidth: true
                    Layout.preferredHeight: drawer.narrowWidth

                    Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                    Controls.ToolTip.visible: hovered
                    Controls.ToolTip.text: i18n("Settings")

                    onClicked: {
                        applicationWindow().switchToPage('Settings')
                    }
                }

                Controls.ToolButton {
                    icon.name: "help-about-symbolic"

                    Layout.fillWidth: true
                    Layout.preferredHeight: drawer.narrowWidth

                    Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                    Controls.ToolTip.visible: hovered
                    Controls.ToolTip.text: i18n("About KleverNotes")

                    onClicked: {
                        applicationWindow().switchToPage('About')
                    }
                }
            }
        }

        Loader {
            visible: drawer.isWide
            anchors {
                top: actionBar.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            ColumnLayout {
                anchors.fill: parent
                TreeView {
                    id: treeView

                    model: NoteTreeModel {
                        id: noteTreeModel

                        noteMapEnabled: Config.noteMapEnabled

                        onNewGlobalPathFound: function (path) {
                            drawer.noteMapper.addGlobalPath(path)
                        }
                        onGlobalPathUpdated: function (oldPath, newPath) {
                            drawer.noteMapper.updateGlobalPath(oldPath, newPath)
                        }
                        onGlobalPathRemoved: function (path) {
                            drawer.noteMapper.removeGlobalPath(path)
                        }
                        onInitialGlobalPathsSent: function (initialGlobalPaths) {
                            drawer.noteMapper.addInitialGlobalPaths(initialGlobalPaths)
                        }
                        onErrorOccurred: function (errorMessage) {
                            applicationWindow().showPassiveNotification(errorMessage)
                        }
                    }

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    onItemRightClicked: function (clickedItem) {
                        const tempModelIndex = treeView.getModelIndex(clickedItem.index)
                        actionBar.setClickedItemInfo(clickedItem, tempModelIndex)
                    }
                    onCurrentItemChanged: {
                        const currentModelIndex = treeView.getModelIndex(treeView.currentIndex)
                        actionBar.setClickedItemInfo(treeView.currentItem, currentModelIndex)
                    }
                }

                Controls.ToolSeparator {
                    orientation: Qt.Horizontal

                    Layout.fillWidth: true
                }

                Delegates.RoundedItemDelegate {
                    text: i18n("Cheat sheet")
                    icon.name: "text-markdown"

                    Layout.fillWidth: true

                    onClicked: {
                        applicationWindow().showCheatSheet()
                        if (drawer.modal) drawer.close()
                    }
                }

                Delegates.RoundedItemDelegate {
                    text: i18n("Settings")
                    icon.name: "settings-configure-symbolic"

                    Layout.fillWidth: true

                    onClicked: {
                        applicationWindow().switchToPage('Settings')
                    }
                }

                Delegates.RoundedItemDelegate {
                    text: i18n("About KleverNotes")
                    icon.name: "help-about-symbolic"

                    Layout.fillWidth: true

                    onClicked: {
                        applicationWindow().switchToPage('About')
                    }
                }
            }
        }
    }

    onClosed: if (changeWidth && !modal) {
        isWide = !isWide
        open()
    }
    onStoragePathChanged: if (storagePath !== "None") {
        noteTreeModel.initModel()
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

    ContextMenu {
        id: contextMenu

        treeView: treeView
        actionBar: actionBar
    }

    Timer {
        id: focusTimer

        property var focusModelIndex

        repeat: false
        interval: Kirigami.Units.shortDuration

        onTriggered: if (focusModelIndex) {
            focusModelIndex.model.askForFocus(focusModelIndex)
            focusModelIndex = undefined
        }
    }

    Timer {
        id: timer

        property var modelIndex

        repeat: false
        interval: Kirigami.Units.shortDuration

        onTriggered: if (modelIndex) {
            modelIndex.model.askForExpand(modelIndex)
            modelIndex = undefined
            interval = Kirigami.Units.shortDuration
            focusTimer.start()
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
