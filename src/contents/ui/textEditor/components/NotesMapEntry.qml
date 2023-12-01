// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

FormCard.AbstractFormDelegate {
    id: root

    readonly property string title: model.title
    readonly property string notePath: model.notePath
    readonly property string displayedPath: model.displayedPath
    readonly property string exists: model.exists
    readonly property string header: model.header
    readonly property bool headerExists: model.headerExists
    readonly property int headerLevel: model.headerLevel
    
    readonly property int elemWidth: Math.round((root.width - arrow.width - root.horizontalPadding * 2 - Kirigami.Units.smallSpacing * 3) / 5) 

    width: parent.width //prevent binding loop on implicitWidth
    
    contentItem: RowLayout {

        spacing: Kirigami.Units.smallSpacing 

        Controls.Label {
            text: root.title
            elide: Text.ElideRight
            Layout.preferredWidth: root.elemWidth * 2
        }

        Controls.Label {
            text: root.displayedPath
            elide: Text.ElideRight
            color: root.exists === "Yes" ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.negativeTextColor
            Layout.preferredWidth: root.elemWidth * 2
        }

        Controls.Label {
            text: root.header
            elide: Text.ElideRight
            color: root.headerExists ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.negativeTextColor
            Layout.preferredWidth: root.elemWidth
        }

        FormCard.FormArrow {
            id: arrow
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.preferredWidth: Kirigami.Units.gridUnit
            direction: Qt.RightArrow
        }
    }
}

