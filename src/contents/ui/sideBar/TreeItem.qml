// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>

/*
 * BASED ON:
 *  SPDX-FileCopyrightText: 2020 Marco Martin <notmart@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

Delegates.RoundedTreeDelegate {
    id: treeItem

    required property string path
    required property string dir
    required property bool isNote
    required property string name
    required property string iconName
    required property bool wantExpand
    required property bool wantFocus

    signal itemRightClicked 

    text: name
    icon.name: iconName
    highlighted: treeView._hasBeenClicked && treeView.currentItem ? treeView.currentItem.path === path : false

    MouseArea {
        anchors.fill: parent
        enabled: !Kirigami.Settings.isMobile 

        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: function (mouse) {
            if (mouse.button === Qt.RightButton) {
                contextMenu.popup()
                itemRightClicked()
                return
            }
            treeItem.clicked()
        }
    }
}
