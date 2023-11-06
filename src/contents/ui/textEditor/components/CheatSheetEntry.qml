// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2822 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

FormCard.AbstractFormDelegate {
    id: root

    required property string element
    required property string syntax

    width: parent.width

    contentItem: GridLayout {
        flow: root.width > Kirigami.Units.gridUnit * 28
            ? GridLayout.LeftToRight
            : GridLayout.TopToBottom

        rowSpacing: root.width > Kirigami.Units.gridUnit * 28
            ? 0
            : Kirigami.Units.largeSpacing

        columnSpacing: root.width > Kirigami.Units.gridUnit * 28
            ? Kirigami.Units.largeSpacing
            : 0


        Controls.Label {
            text: root.element
            textFormat: Text.RichText
            Layout.preferredWidth: root.width / (root.width > Kirigami.Units.gridUnit * 28 ? 2 : 1) 
        }

        Kirigami.Separator {
            Layout.preferredWidth: root.width <= Kirigami.Units.gridUnit * 28 ? root.width - Kirigami.Units.gridUnit * 2 : 1 
            Layout.fillHeight: root.width > Kirigami.Units.gridUnit * 28
        }

        Controls.Label {
            text: root.syntax
            Layout.preferredWidth: root.width / (root.width > Kirigami.Units.gridUnit * 28 ? 2 : 1)  
        }
    }
}

