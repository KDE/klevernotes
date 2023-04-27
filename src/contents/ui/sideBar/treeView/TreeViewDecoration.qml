/*
 *  SPDX-FileCopyrightText: 2020 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import QtQuick.Layouts 1.4
import QtQuick.Controls 2.2 as QQC2
import QtQuick.Templates 2.2 as T2
import org.kde.kitemmodels 1.0
import org.kde.kirigami 2.14 as Kirigami

/**
 * The tree expander decorator for item views.
 *
 * It will have a "> v" expander button graphics, and will have indentation on the left
 * depending on the level of the tree the item is in
 */
RowLayout {
    id: decorationLayout
    /**
     * This property holds the delegate there this decoration will live in.
     * It needs to be assigned explicitly by the developer.
     */
    property T2.ItemDelegate parentDelegate

    /**
     * This property holds the KDescendantsProxyModel the view is showing.
     * It needs to be assigned explicitly by the developer.
     */
    property KDescendantsProxyModel model

    /**
     * This property holds the color of the decoration highlight.
     */
    property color decorationHighlightColor

    Layout.topMargin: -parentDelegate.topPadding
    Layout.bottomMargin: -parentDelegate.bottomPadding
    Repeater {
        model: kDescendantLevel - 1
        delegate: Item {
            Layout.preferredWidth: controlRoot.width
            Layout.fillHeight: true
        }
    }
    T2.Button {
        id: controlRoot
        Layout.preferredWidth: Kirigami.Units.gridUnit
        Layout.fillHeight: true
        enabled: kDescendantExpandable
        onClicked: model.toggleChildren(index)
        contentItem: Item {
            id: styleitem
            implicitWidth: Kirigami.Units.gridUnit

            Kirigami.Icon {
                id: expander
                anchors.centerIn: parent
                width: Kirigami.Units.iconSizes.small
                height: width
                source: kDescendantExpanded ? "go-down-symbolic" : (Qt.application.layoutDirection == Qt.RightToLeft ? "go-previous-symbolic" : "go-next-symbolic")
                isMask: true
                color: controlRoot.hovered ? decorationLayout.decorationHighlightColor ? decorationLayout.decorationHighlightColor : Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                Behavior on color { ColorAnimation { duration: Kirigami.Units.shortDuration; easing.type: Easing.InOutQuad } }
                visible: kDescendantExpandable
            }
        }
        background: Item {}
    }
}
