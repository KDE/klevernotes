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
    id:drawer

    edge: Qt.application.layoutDirection == Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    handleClosedIcon.source: null
    handleOpenIcon.source: null
    handleVisible: applicationWindow().isMainPage() && modal && pageStack.layers.depth < 2

    // Autohiding behavior
    modal: !applicationWindow().isMainPage() || !root.wideScreen
    onEnabledChanged: drawerOpen = enabled && !modal
    onModalChanged: drawerOpen = !modal && pageStack.layers.depth < 2
    // Prevent it to being close while in wideScreen
    closePolicy: Controls.Popup.CloseOnReleaseOutside

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    property bool storageExist: Config.storagePath !== "None"

    contentItem: ColumnLayout {
        id: column
        // FIXME: Dirty workaround for 385992
        implicitWidth: Kirigami.Units.gridUnit * 14

        ActionBar{
            id: action

            treeView: treeview

            Layout.fillWidth: true
            Layout.preferredHeight: pageStack.globalToolBar.preferredHeight
        }

        TreeView{
            id: treeview

            visible: storageExist

            Layout.fillHeight: true
            Layout.alignment:Qt.AlignTop
        }

        Item{
            id: dummyPlaceHolder

            Layout.fillHeight: !storageExist
            Layout.alignment:Qt.AlignTop
        }


        Controls.ToolSeparator {
            orientation: Qt.Horizontal

            Layout.topMargin: -1;
            Layout.fillWidth: true
            Layout.alignment:Qt.AlignBottom
        }

        Kirigami.BasicListItem {
            text: i18n("Settings")
            icon: "settings-configure"

            Layout.alignment:Qt.AlignBottom

            onClicked: applicationWindow().switchToPage('Settings')
        }

        Kirigami.BasicListItem {
            text: i18n("About Klever")
            icon: "help-about"

            onClicked: {drawerOpen = false,applicationWindow().switchToPage('About')}
        }

    }

     Component.onCompleted: {
        if (!storageExist){
            let component = Qt.createComponent("qrc:/contents/ui/dialogs/StorageDialog.qml")

            if (component.status == Component.Ready) {
                var dialog = component.createObject(applicationWindow());
                dialog.open()
            }
        }
     }
}


