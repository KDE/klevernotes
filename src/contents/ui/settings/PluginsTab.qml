// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.Klever 1.0

ColumnLayout {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    FormCard.FormHeader {
        title: i18nc("@title", "KleverNotes plugins")
        Layout.fillWidth: true
    }

    FormCard.FormCard {
        id: pluginsCard

        Layout.fillWidth: true

        FormCard.FormCheckDelegate {
            id: noteMapperCheck

            text: i18nc("@label:checkbox", "Enable note linking")
            description: i18nc("@description:checkbox", "Note linking allows you to create a link from one note to another.") 
                + "\n" + i18nc("@description:checkbox", "Advice: restart the app once activated.")
            checked: Config.noteMapEnabled

            onCheckedChanged: if (checked != Config.noteMapEnabled) {
                Config.noteMapEnabled = checked
            }
        }

        FormCard.FormDelegateSeparator { above: noteMapperCheck; below: highlitingCheck }

        ExpendingFormCheckBox {
            id: highlitingCheck

            text: i18nc("@label:checkbox", "Enable code syntax highlighting")
            description: "<a href='https://invent.kde.org/office/klevernotes#syntax-highlighting'>" + i18nc("@description:checkbox", "List of supported highlighters") + "</a>"
            checked: Config.codeSynthaxHighlightEnabled

            highlighterCombobox.model: HighlightHelper.highlighters 
            styleCombobox.model: HighlightHelper.getHighlighterStyle(highlighterCombobox.currentValue) 

            onCheckedChanged: if (checked != Config.codeSynthaxHighlightEnabled) {
                Config.codeSynthaxHighlightEnabled = checked
            }
            highlighterCombobox.onCurrentValueChanged: {
                const highlighter = highlighterCombobox.currentValue
                if (highlighter != Config.codeSynthaxHighlighter) Config.codeSynthaxHighlighter = highlighter
            }
            styleCombobox.onCurrentValueChanged: {
                const style = styleCombobox.currentValue
                if (style != Config.codeSynthaxHighlighterStyle) Config.codeSynthaxHighlighterStyle = style 
            }
        }
    }
}
