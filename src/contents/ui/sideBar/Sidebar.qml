/*
 * SPDX-FileCopyrightText: (C) 2017 Atul Sharma <atulsharma406@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.5 as Kirigami
import org.kde.Klever 1.0



Kirigami.OverlayDrawer {
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
            model: View.hierarchy(Config.storagePath,-1)
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
                onClicked: applicationWindow().switchToPage('About')
        }

    }
}


