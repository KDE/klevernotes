// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard

AbstractFormDelegate {
    id: root

    // see https://doc.qt.io/qt-6/qt.html#ArrowType-enum
    property int arrowDirection: Qt.NoArrow

    focusPolicy: Qt.StrongFocus

    ToolTip.text: text
    ToolTip.delay: Kirigami.Units.toolTipDelay
    ToolTip.visible: hovered

    contentItem: RowLayout {
        spacing: 0

        Label {
            text: root.text
            color: root.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            maximumLineCount: 2

            Layout.margins: 0
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.fillHeight: true

            Accessible.ignored: true // base class sets this text on root already
        }

        FormArrow {
            // 0 = Qt::NoArrow ; 5 = Qt::RightArrow
            direction: (0 < root.arrowDirection && root.arrowDirection < 5) ? root.arrowDirection : Qt.NoArrow

            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.leftMargin: Kirigami.Units.smallSpacing
        }
    }

    Accessible.onPressAction: action ? action.trigger() : root.clicked()
}
