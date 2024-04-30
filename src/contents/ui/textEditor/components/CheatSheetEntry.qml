// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

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
