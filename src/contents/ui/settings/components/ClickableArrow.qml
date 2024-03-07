// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Templates 2.15 as T

import org.kde.kirigamiaddons.formcard 1.0

T.ItemDelegate {
    id: root

    // see https://doc.qt.io/qt-6/qt.html#ArrowType-enum
    property int arrowDirection: Qt.NoArrow

    hoverEnabled: true
    horizontalPadding: 0
    background: FormDelegateBackground { control: root }

    FormArrow {
        // 0 = Qt::NoArrow ; 5 = Qt::RightArrow
        direction: (0 < root.arrowDirection && root.arrowDirection < 5) ? root.arrowDirection : Qt.NoArrow

        x: Math.round((root.width - width) / 2)
        y: Math.round((root.height - height) / 2)
    }

    Accessible.onPressAction: action ? action.trigger() : root.clicked()
}
