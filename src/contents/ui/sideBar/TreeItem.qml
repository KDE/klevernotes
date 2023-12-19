/*
 *  SPDX-FileCopyrightText: 2020 Marco Martin <notmart@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.delegates 1 as Delegates

Delegates.RoundedTreeDelegate {
    id: treeItem

    required property string path
    required property string useCase
    required property string displayName
    required property string iconName
    required property bool wantExpand
    required property bool wantFocus

    signal itemRightClicked 

    text: displayName
    icon.name: iconName
    highlighted: treeview.currentItem ? treeView.currentItem.path === path : false

    background: Rectangle { // Forced to do it, without that, the first element is not using the correct Theme
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false

        radius: Kirigami.Units.smallSpacing

        color: if (treeItem.highlighted || treeItem.checked || (treeItem.down && !treeItem.checked) || treeItem.visualFocus) {
            const highlight = Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.highlightColor, 0.3);
            if (treeItem.hovered) {
                Kirigami.ColorUtils.tintWithAlpha(highlight, Kirigami.Theme.textColor, 0.10)
            } else {
                highlight
            }
        } else if (treeItem.hovered) {
            Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.10)
        } else {
           Kirigami.Theme.backgroundColor
        }

        border {
            color: Kirigami.Theme.highlightColor
            width: treeItem.visualFocus || treeItem.activeFocus ? 1 : 0
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
                contextMenu.canDelete = !treeItem.path.endsWith("/.BaseCategory")
                contextMenu.popup()
                itemRightClicked()
                return
            }
            treeItem.clicked()
        }
    }
}
