// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

Kirigami.Dialog {
    id: scrollableDialog

    property string checkedFamily
    property int checkedSize
    property var caller

    title: i18nc("@title:dialog", "Font selector")

    standardButtons: Kirigami.Dialog.Apply | Kirigami.Dialog.Cancel
    
    onCallerChanged: if (caller) {
        const familyDefaultIndex = 0
        const sizeDefaultIndex = 4
        const fontInfo = caller.fontInfo
        const familyModelIndex = fontBox.model.indexOf(fontInfo.family)
        const sizeModelIndex = sizeBox.model.indexOf(fontInfo.pointSize)

        fontBox.currentIndex = familyModelIndex > -1 ? familyModelIndex : familyDefaultIndex 
        sizeBox.currentIndex = sizeModelIndex > -1 ? sizeModelIndex : sizeDefaultIndex 
    }
    onClosed: {
        caller = undefined
    }

    GridLayout {
        rows: 1
        columns: 2
        FormCard.FormComboBoxDelegate {
            id: fontBox

            model: Qt.fontFamilies() 

            Layout.row: 0
            Layout.column: 0

            onCurrentValueChanged: {
                scrollableDialog.checkedFamily = currentValue;
            }
        }

        FormCard.FormComboBoxDelegate {
            id: sizeBox

            model: [6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72]

            Layout.row: 0
            Layout.column: 1

            onCurrentValueChanged: {
                scrollableDialog.checkedSize = currentValue
            }
        }

        FormCard.FormSectionText {
            id: displayText

            text: fontBox.currentValue ? fontBox.currentValue : "Note Sans"
            font.family: text
            font.pointSize: sizeBox.currentValue ? sizeBox.currentValue : 10

            Layout.row: 1
            Layout.column: 0
            Layout.rowSpan: 2
        }
    }
}
