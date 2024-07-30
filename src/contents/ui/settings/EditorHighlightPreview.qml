// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

import "qrc:/contents/ui/sharedComponents"

ColumnLayout {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    FormCard.FormSwitchDelegate {
        id: adaptSizeCheck

        text: i18nc("@label:checkbox, a Markdown tag, e.g: the `#` for a header `# this is header`, similar concept as an html tag `<h1>`", "Enable adaptive tag size")
        description: i18nc("@description:checkbox, a Markdown tag, e.g: the `#` for a header `# this is header`, similar concept as an html tag `<h1>`", "The adaptive size will scale the tag size based on its surrounding.")
            + "\n" + i18nc("@description:checkbox, a Markdown tag, e.g: the `#` for a header `# this is header`, similar concept as an html tag `<h1>`", "Example: a tag inside a header will be as large as the rest of the header text.")
        checked: Config.adaptiveTagSizeEnabled

        Layout.fillWidth: true

        onCheckedChanged: if (checked != Config.adaptiveTagSizeEnabled) {
            Config.adaptiveTagSizeEnabled = checked
        }
    }

    FormCard.FormSpinBoxDelegate {
        id: scaleSpin

        label: i18nc("@label:spinbox, a Markdown tag, e.g: the `#` for a header `# this is header`, similar concept as an html tag `<h1>`", "Tag scale")

        from: 1
        to: 100
        value: Config.tagSizeScale

        Layout.fillWidth: true

        onValueChanged: if (Config.tagSizeScale != value) Config.tagSizeScale = value
    }
}
