// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

FormCard.AbstractFormDelegate {
    id: root

    required property string element
    required property string syntax

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        Controls.Label {
            text: root.element
            textFormat: Text.RichText
            Layout.fillWidth: true
        }

        Kirigami.Separator {
            Layout.preferredWidth: root.width - Kirigami.Units.gridUnit * 4
        }

        Controls.Label {
            text: root.syntax
            elide: Text.ElideMiddle
            Layout.fillWidth: true
        }
    }
}
