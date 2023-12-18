/*
 *  SPDX-FileCopyrightText: 2020 Marco Martin <notmart@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0 as QQC2

import org.kde.kirigami 2.14
import org.kde.kirigamiaddons.treeview 1.0 as TreeView

/**
 * An item delegate for the TreeListView and TreeTableView components.
 *
 * It's intended to make all tree views look coherent.
 * It has a default icon and a label
 *
 * @since org.kde.kirigamiaddons.treeview 1.0
 */
TreeView.AbstractTreeItem {
    id: listItem

    /**
     * This property holds the single text label the list item will contain.
     *
     * @since org.kde.kirigamiaddons.treeview 1.0
     */
    property alias label: listItem.text

    /**
     * This property controls whether the text (in both primary text and subtitle)
     * should be rendered as bold.
     *
     * @since org.kde.kirigamiaddons.treeview 1.0
     */
    property bool bold: false

    /**
     * This property holds the preferred size for the icon.
     *
     * @since org.kde.kirigamiaddons.treeview 1.0
     */
    property alias iconSize: iconItem.size

    /**
     * This property holds the color the icon should be colorized to.
     * If the icon shouldn't be colorized in any way, set it to "transparent"
     *
     * By default it will be the text color.
     *
     * @since org.kde.kirigamiaddons.treeview 1.0
     */
    property alias iconColor: iconItem.color

    /**
     * @brief This property holds whether when there is no icon the space will
     * still be reserved for it.
     *
     * It's useful in layouts where only some entries have an icon, having the
     * text all horizontally aligned.
     */
    property alias reserveSpaceForIcon: iconItem.visible

    /**
     * @brief This property holds whether the label will try to be as wide as
     * possible.
     *
     * It's useful in layouts containing entries without text. By default, true.
     */
    property alias reserveSpaceForLabel: labelItem.visible

    default property alias _basicDefault: layout.data

    contentItem: Row {
        id: layout

        height: iconItem.size
        spacing: Units.smallSpacing
        Icon {
            id: iconItem

            property int size: Units.iconSizes.smallMedium

            width: size
            height: size
            opacity: 1
            visible: source != undefined
            selected: (listItem.highlighted || listItem.checked || (listItem.pressed && listItem.supportsMouseEvents))
            source: {
                if (!listItem.icon) {
                    return undefined
                }
                if (listItem.icon.hasOwnProperty) {
                    if (listItem.icon.hasOwnProperty("name") && listItem.icon.name !== "")
                        return listItem.icon.name;
                    if (listItem.icon.hasOwnProperty("source"))
                        return listItem.icon.source;
                }
                return listItem.icon;
            }
        }
        QQC2.Label {
            id: labelItem

            width: layout.width - iconItem.width

            text: listItem.text
            color: (listItem.highlighted || listItem.checked || (listItem.pressed && listItem.supportsMouseEvents)) ? listItem.activeTextColor : listItem.textColor
            elide: Text.ElideRight
            opacity: 1
            font.weight: listItem.bold ? Font.Bold : Font.Normal
        }
    }

    MouseArea {
        anchors.fill: layout

        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: function (mouse) {
            listItem.clicked()
            if (mouse.button === Qt.RightButton) {
                contextMenu.canDelete = !listItem.path.endsWith("/.BaseCategory")
                contextMenu.popup()
            }
        }
    }
}
