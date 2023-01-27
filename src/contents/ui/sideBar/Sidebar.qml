// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

/*
 * LOOSELY BASED ON :
 * https://invent.kde.org/graphics/koko/-/blob/master/src/qml/Sidebar.qml
 * 2017 Atul Sharma <atulsharma406@gmail.com>
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

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0


    property alias treeModel : treeview.model
    // Place
    contentItem: ColumnLayout {
        id: column
        // FIXME: Dirty workaround for 385992
        implicitWidth: Kirigami.Units.gridUnit * 14


        Kirigami.AbstractApplicationHeader {
            topPadding: Kirigami.Units.smallSpacing;
            bottomPadding: Kirigami.Units.smallSpacing;
            leftPadding: Kirigami.Units.largeSpacing
            rightPadding: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            Kirigami.Heading {
                level: 1
                text: i18n("Notes")
            }
            Layout.alignment:Qt.AlignTop
        }

        ActionBar{
            id: action
            Layout.fillWidth: true
            Layout.alignment:Qt.AlignTop
            treeView: treeview
        }

        TreeView{
            id: treeview
            Layout.fillHeight: true
            Layout.alignment:Qt.AlignTop
        }

        Controls.ToolSeparator {
            Layout.topMargin: -1;
            Layout.fillWidth: true
            orientation: Qt.Horizontal
            Layout.alignment:Qt.AlignBottom
        }

        Kirigami.BasicListItem {
            text: i18n("Settings")
            // onClicked: applicationWindow().switchToPage('settings')
            icon: "settings-configure"
            Layout.alignment:Qt.AlignBottom
        }

        Kirigami.BasicListItem {
                text: i18n("About Klever")
                icon: "help-about"
                onClicked: {drawerOpen = false,applicationWindow().switchToPage('About')}
        }

    }


    function checkForStorage(subtitle){
        var actualPath = Config.storagePath

        if (actualPath === "None"){
            let component = Qt.createComponent("qrc:/contents/ui/dialogs/StorageDialog.qml")

            if (component.status == Component.Ready) {
                var dialog = component.createObject(drawer);

                if (subtitle) dialog.subtitle = subtitle

                dialog.drawer = drawer
                dialog.open()
                return
            }
        }
        treeview.model = View.hierarchy(Config.storagePath,-1)
    }

     Component.onCompleted: {
        checkForStorage()
     }
}


