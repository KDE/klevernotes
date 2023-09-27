// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0

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
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 0

            text: root.text
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            color: root.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            Accessible.ignored: true // base class sets this text on root already
        }

        FormArrow {
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            // 0 = Qt::NoArrow ; 5 = Qt::RightArrow
            direction: (0 < root.arrowDirection && root.arrowDirection < 5) ? root.arrowDirection : Qt.NoArrow
        }
    }

    Accessible.onPressAction: action ? action.trigger() : root.clicked()
}

