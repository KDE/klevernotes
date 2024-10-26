// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

import "qrc:/contents/ui/sharedComponents"

ColumnLayout {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    Component.onCompleted: setPreviewEditorFontSize()

    FormCard.FormSwitchDelegate {
        id: adaptSizeCheck

        text: i18nc("@label:checkbox, a Markdown tag, e.g: the `#` for a header `# this is header`, similar concept as an html tag `<h1>`", "Enable adaptive tag size")
        description: i18nc("@description:checkbox, a Markdown tag, e.g: the `#` for a header `# this is header`, similar concept as an html tag `<h1>`", "The adaptive size will scale the tag size based on its surrounding.")
            + "\n" + i18nc("@description:checkbox, a Markdown tag, e.g: the `#` for a header `# this is header`, similar concept as an html tag `<h1>`", "Example: a tag inside a header will be as large as the rest of the header text.")
        checked: Config.adaptiveTagSizeEnabled

        Layout.fillWidth: true

        onCheckedChanged: if (checked != Config.adaptiveTagSizeEnabled) {
            Config.adaptiveTagSizeEnabled = checked
            setPreviewEditorFontSize()
        }
    }

    FormCard.FormSpinBoxDelegate {
        id: scaleSpin

        label: i18nc("@label:spinbox, a Markdown tag, e.g: the `#` for a header `# this is header`, similar concept as an html tag `<h1>`", "Tag scale")

        from: 1
        to: 100
        value: Config.tagSizeScale

        Layout.fillWidth: true

        onValueChanged: if (Config.tagSizeScale != value) {
            Config.tagSizeScale = value
            setPreviewEditorFontSize()
        }
    }

    ColumnLayout {
        id: labelLayout

        property real editorTextSize: 12
        property real editorDelimSize: 12
        readonly property int delimBottomMargin: Math.round(editorTextSize - editorDelimSize) / 2
        readonly property color delimColor: Kirigami.ColorUtils.brightnessForColor(Kirigami.Theme.backgroundColor) == Kirigami.ColorUtils.Light
            ? Qt.lighter(Kirigami.Theme.textColor, 2)
            : Qt.darker(Kirigami.Theme.textColor, 3)

        spacing: 0
        Layout.fillWidth: true
        RowLayout {
            spacing: 0
            Layout.fillWidth: true

            Label {
                text: "# "

                color: labelLayout.delimColor
                font.pointSize: labelLayout.editorDelimSize
                verticalAlignment: Text.AlignBottom
                Layout.alignment: Qt.AlignBottom
                Layout.bottomMargin: labelLayout.delimBottomMargin
            }

            Label {
                text: i18nc("@exemple, the rest of the sentence is 'italic text and its tags', the second part is provided by another string",
                        "A title with a preview of") + " "

                font.pointSize: labelLayout.editorTextSize
                verticalAlignment: Text.AlignBottom
                Layout.alignment: Qt.AlignBottom
            }
        }

        RowLayout {
            spacing: 0
            Layout.fillWidth: true

            Label {
                text: "_"

                color: labelLayout.delimColor
                font.pointSize: labelLayout.editorDelimSize
                verticalAlignment: Text.AlignBottom
                Layout.alignment: Qt.AlignBottom
                Layout.bottomMargin: labelLayout.delimBottomMargin
            }

            Label {
                text: i18nc("@exemple, the first part of the sentence is 'A title with a preview of ', the first part is provided by another string",
                        "italic text and its tags")

                font.pointSize: labelLayout.editorTextSize
                verticalAlignment: Text.AlignBottom
                Layout.alignment: Qt.AlignBottom
            }

            Label {
                text: "_"

                color: labelLayout.delimColor
                font.pointSize: labelLayout.editorDelimSize
                verticalAlignment: Text.AlignBottom
                Layout.alignment: Qt.AlignBottom
                Layout.bottomMargin: labelLayout.delimBottomMargin
            }
        }
    }

    function setPreviewEditorFontSize() {
        const editorFontInfo = KleverUtility.fontInfo(Config.editorFont)

        const baseSize = editorFontInfo.pointSize

        // Based on KleverStyle.css
        const titleSize = baseSize + 12

        let delimSize = Config.adaptiveTagSizeEnabled ? titleSize : baseSize
        delimSize = delimSize * Config.tagSizeScale / 100
        if (delimSize < 1) {
            delimSize = 1
        }

        labelLayout.editorTextSize = titleSize
        labelLayout.editorDelimSize = delimSize
    }
}
