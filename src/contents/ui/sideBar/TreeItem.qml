// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

Delegates.RoundedTreeDelegate {
    id: root

    required property string path
    required property string parentPath
    required property string dir
    required property bool isNote
    required property string name
    required property string iconName
    required property string color
    required property bool wantExpand
    required property bool wantFocus

    readonly property string defaultIcon: isNote ? "document-edit-sign-symbolic" : "folder-symbolic"
    readonly property string defaultColor: Kirigami.Theme.highlightColor

    signal itemRightClicked 

    text: name
    icon.name: iconName ? iconName : defaultIcon
    highlighted: treeView._hasBeenClicked && treeView.currentItem ? treeView.currentItem.path === path : false
    width: listView.view.width / 3 < leftInset ? Math.round(listView.view.width * 0.75) + leftInset : listView.view.width

    background: Rectangle {
        readonly property string baseColor: root.color ? root.color : root.defaultColor 
        radius: Kirigami.Units.cornerRadius

        color: if (root.highlighted || root.checked || (root.down && !root.checked) || root.visualFocus) {
            const highlight = Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, baseColor, 0.3);
            if (root.hovered || root.dropAreaHovered) {
                Kirigami.ColorUtils.tintWithAlpha(highlight, Kirigami.Theme.textColor, 0.10)
            } else {
                highlight
            }
        } else if (root.hovered || root.dropAreaHovered) {
            Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.10)
        } else {
           Kirigami.Theme.backgroundColor
        }

        border {
            color: baseColor
            width: root.visualFocus || root.activeFocus ? 1 : 0
        }

        Behavior on color {
            ColorAnimation {
                duration: Kirigami.Units.shortDuration
            }
        }
    }

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
