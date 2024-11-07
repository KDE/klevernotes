// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard

AbstractFormDelegate {
    id: root

    property bool reversed: false
    focusPolicy: Qt.StrongFocus

    icon {
        width: Kirigami.Units.iconSizes.small
        height: Kirigami.Units.iconSizes.small
    }

    contentItem: RowLayout {
        spacing: 0

        LayoutMirroring.enabled: root.reversed

        Item {
            Layout.fillWidth: true
        }

        RowLayout {
            Kirigami.Icon {
                visible: root.icon.name !== ""
                source: root.icon.name
                Layout.rightMargin: (root.icon.name !== "") ? Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing : 0
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                implicitWidth: (root.icon.name !== "") ? root.icon.width : 0
                implicitHeight: (root.icon.name !== "") ? root.icon.height : 0
            }

            Label {
                text: root.text
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                maximumLineCount: 2
                color: root.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                Accessible.ignored: true // base class sets this text on root already
            }

        }

        Item {
            Layout.fillWidth: true
        }

        FormArrow {
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            direction: root.reversed ? Qt.LeftArrow : Qt.RightArrow
            visible: root.background.visible
        }
    }

    Accessible.onPressAction: action ? action.trigger() : root.clicked()
}
