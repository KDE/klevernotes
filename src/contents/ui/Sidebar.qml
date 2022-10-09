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

import '/test.js' as Ouep

Kirigami.OverlayDrawer {
    edge: Qt.application.layoutDirection == Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    handleClosedIcon.source: null
    handleOpenIcon.source: null
    handleVisible: modal && pageStack.layers.depth < 2

    // Autohiding behavior
    modal: !root.wideScreen
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
        }

        Controls.Button{
            text:"click"
            onClicked: {
                const storageContent = View.hierarchy(KleverUtility.getPath(Config.path))["content"]

                Ouep.sortCategory(machin["content"])
                p1model.append({score: "truc"})
            }
        }
        Controls.ScrollView {
            id: scrollView
            Controls.ScrollBar.vertical.policy: Controls.ScrollBar.AsNeeded
            Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AsNeeded
            property var currentlySelectedAction
            property var previouslySelectedAction

            Layout.topMargin: -Kirigami.Units.smallSpacing - 1;
            Layout.bottomMargin: -Kirigami.Units.smallSpacing;
            Layout.fillHeight: true
            Layout.fillWidth: true

            Accessible.role: Accessible.MenuBar
            contentWidth: availableWidth

            clip: true

            Component {
                id: delegate
                Item {
                    width: 200; height: 28
                    Controls.Label {
                        text: score
                    }
                }
            }

            ListView {
                id: p1scores
                model: p1model
                // TODO adapt delegate based on if isDir or not => https://stackoverflow.com/a/36085569
                delegate: delegate
                // anchors.top: p1name.bottom
                // anchors.topMargin: units.gu(1)
                width:50
                height:200
                // boundsBehavior: Flickable.StopAtBounds
                // Controls.ScrollBar.vertical: Controls.ScrollBar {
                //     policy: ScrollBar.AlwaysOn
                //     active: ScrollBar.AlwaysOn
                // }
                Controls.ScrollBar.vertical: Controls.ScrollBar {
                    active: true
                }
            }
            // Controls.ScrollBar {
            //         flickableItem: p1scrores
            //         // align: Qt.AlignTrailing
            // }

            ListModel {
                id: p1model
                ListElement { score: "0" }
            }

            //List{}
/*
            component PlaceHeading : Kirigami.Heading {
                topPadding: Kirigami.Units.largeSpacing
                leftPadding: Kirigami.Units.largeSpacing
                Layout.fillWidth: true
                level: 6
                opacity: 0.7
            }

            component PlaceItem : Kirigami.AbstractListItem {
                id: item
                property string icon
                property string filter
                property string query
                checkable: true
                separatorVisible: false
                Layout.fillWidth: true
                Keys.onDownPressed: nextItemInFocusChain().forceActiveFocus(Qt.TabFocusReason)
                Keys.onUpPressed: nextItemInFocusChain(false).forceActiveFocus(Qt.TabFocusReason)
                Accessible.role: Accessible.MenuItem
                leftPadding: Kirigami.Units.largeSpacing
                contentItem: Row {
                    Kirigami.Icon {
                        source: item.icon
                        width: height
                        height: Kirigami.Units.iconSizes.small
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Controls.Label {
                        leftPadding: Kirigami.Units.smallSpacing
                        text: item.text
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
                onClicked: {
                    scrollView.currentlySelectedAction = item;

                    if (scrollView.previouslySelectedAction) {
                        scrollView.previouslySelectedAction.checked = false;
                    }

                    scrollView.currentlySelectedAction.checked = true;

                    applicationWindow().filterBy(filter, query);

                    scrollView.previouslySelectedAction = item;
                }
            }*/
/*
            ColumnLayout {
                spacing: 1
                width: scrollView.width
                PlaceHeading {
                    text: i18n("Places")
                }
                PlaceItem {
                    id: picturesAction
                    icon: "folder-pictures"
                    text: i18n("Pictures")
                    filter: "Pictures"
                }
                PlaceItem {
                    text: i18n("Favorites")
                    icon: "starred-symbolic"
                    filter: "Favorites"
                }

                PlaceItem {
                    icon: "user-trash-symbolic"
                    text: i18n("Trash")
                    filter: "Trash"
                    query: "trash:/"
                }
                PlaceHeading {
                    text: i18nc("Remote network locations", "Remote")
                }
                PlaceItem {
                    icon: "folder-cloud"
                    text: i18n("Network")
                    filter: "Remote"
                    query: "remote:/"
                }
                PlaceHeading {
                    text: i18n("Locations")
                }
                PlaceItem {
                    text: i18n("Countries")
                    icon: "tag-places"
                    filter: "Countries"
                }
                PlaceItem {
                    text: i18n("States")
                    icon: "tag-places"
                    filter: "States"
                }
                PlaceItem {
                    text: i18n("Cities")
                    icon: "tag-places"
                    filter: "Cities"
                }
                PlaceHeading {
                    text: i18n("Time")
                }
                PlaceItem {
                    text: i18n("Years")
                    icon: "view-calendar"
                    filter: "Years"
                }
                PlaceItem {
                    text: i18n("Months")
                    icon: "view-calendar"
                    filter: "Months"
                }
                PlaceItem {
                    text: i18n("Weeks")
                    icon: "view-calendar"
                    filter: "Weeks"
                }
                PlaceItem {
                    text: i18n("Days")
                    icon: "view-calendar"
                    filter: "Days"
                }
            }*/
        }
        Controls.ToolSeparator {
            Layout.topMargin: -1;
            Layout.fillWidth: true
            orientation: Qt.Horizontal
            visible: scrollView.contentHeight > scrollView.height
        }

        Kirigami.BasicListItem {
            text: i18n("Settings")
            onClicked: applicationWindow().openSettingsPage()
            icon: "settings-configure"
        }
    }

}


