// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>

/*
 * BASED ON :
 * https://invent.kde.org/graphics/koko/-/blob/master/src/qml/Sidebar.qml
 * 2017 Atul Sharma <atulsharma406@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.15 // Removing version break onCurrentItemChanged
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigami as Kirigami

import "qrc:/contents/ui/dialogs"

import org.kde.Klever

Kirigami.OverlayDrawer {
    id: drawer

    required property color backgroundColor

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

    contentItem: Rectangle {
        id: column

        Kirigami.Theme.backgroundColor: drawer.backgroundColor

        color: Kirigami.Theme.backgroundColor

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
                    // used when moveRow throws an error
                    property var movingRowModelIndex
                    property var movingRowNewParentIndex

                    Kirigami.Theme.backgroundColor: drawer.backgroundColor

                    model: NoteTreeModel {
                        id: noteTreeModel

                        noteMapEnabled: Config.noteMapEnabled

                        onNewGlobalPathFound: function (path) {
                            NoteMapper.addGlobalPath(path)
                        }
                        onGlobalPathUpdated: function (oldPath, newPath) {
                            NoteMapper.updateGlobalPath(oldPath, newPath)
                        }
                        onGlobalPathRemoved: function (path) {
                            NoteMapper.removeGlobalPath(path)
                        }
                        onInitialGlobalPathsSent: function (initialGlobalPaths) {
                            NoteMapper.addInitialGlobalPaths(initialGlobalPaths)
                        }
                        onErrorOccurred: function (errorMessage) {
                            applicationWindow().showPassiveNotification(errorMessage)
                        }
                        onOldStorageStructure: function () {
                            oldStructureWarning.open()
                        }
                        onMoveError: function (rowModelIndex, newParentIndex, isNote, name, parentPath) {
                            treeView.movingRowModelIndex = rowModelIndex
                            treeView.movingRowNewParentIndex = newParentIndex
                            actionBar.getName(isNote, treeView.moveErrorRename, false, parentPath, name)
                            actionBar.forceError("exist")
                        }
                        onForceFocus: function (rowModelIndex) {
                            drawer.askForFocus(rowModelIndex)
                        }
                    }

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    function moveErrorRename(newName: string): void {
                        if (newName.length > 0) {
                            noteTreeModel.moveRow(treeView.movingRowModelIndex, treeView.movingRowNewParentIndex, newName)
                            treeView.movingRowModelIndex = null
                            treeView.movingRowNewParentIndex = null
                        }
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
                        StyleHandler.isMain = false
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

    StorageConversionDialog {
        id: oldStructureWarning

        onApplied: {
            noteTreeModel.initModel(true)
            close()
        }
        onRejected: {
            close()
        }
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
        id: expandTimer

        property var _modelIndexesList
        property int _counter

        repeat: false
        interval: Kirigami.Units.veryShortDuration

        onTriggered: if (_modelIndexesList) {
            if (0 <= _counter) {
                const currentModelIndex = _modelIndexesList[_counter]
                currentModelIndex.model.askForExpand(currentModelIndex)
                _counter--
                expandTimer.start()
            } else {
                _modelIndexesList = undefined
                focusTimer.start()
            }
        }

        function expand(modelIndexesList: var): void {
            _modelIndexesList = modelIndexesList
            _counter = modelIndexesList.length - 1
            expandTimer.start()
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

        focusTimer.focusModelIndex = modelIndex

        if (parentRowsList.length !== 0) {
            expandTimer.expand(parentRowsList)
        } else {
            focusTimer.start()
        }
    }
}
