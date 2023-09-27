// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0

AbstractFormDelegate {
    id: root

    focusPolicy: Qt.StrongFocus

    ToolTip.text: text
    ToolTip.delay: Kirigami.Units.toolTipDelay
    ToolTip.visible: hovered

    contentItem: GridLayout {
        rows: root.display === AbstractButton.TextUnderIcon ? 2 : 1
        columns: root.display === AbstractButton.TextUnderIcon ? 1 : 2
        columnSpacing: 0

        Kirigami.Icon {
            Layout.row: 0
            Layout.column: 0
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.fillHeight: true
            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing
            Layout.topMargin: root.display === AbstractButton.TextUnderIcon ? Kirigami.Units.largeSpacing : Kirigami.Units.smallSpacing
            Layout.bottomMargin: Kirigami.Units.smallSpacing

            color: root.icon.color
            source: root.icon.name
            visible: root.icon.name !== ""
            implicitWidth: (root.icon.width) ? root.icon.width : 0
            implicitHeight: (root.icon.height) ? root.icon.height : 0
        }

        Label {
            Layout.row: root.display === AbstractButton.TextUnderIcon ? 1 : 0
            Layout.column: root.display === AbstractButton.TextUnderIcon ? 0 : 1
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: root.display === AbstractButton.TextUnderIcon ? Kirigami.Units.largeSpacing : 0
            Layout.rightMargin: Kirigami.Units.largeSpacing
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.bottomMargin: root.display === AbstractButton.TextUnderIcon ? Kirigami.Units.largeSpacing : Kirigami.Units.smallSpacing

            text: root.text
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            color: root.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            Accessible.ignored: true // base class sets this text on root already
        }
    }

    Accessible.onPressAction: action ? action.trigger() : root.clicked()
}

