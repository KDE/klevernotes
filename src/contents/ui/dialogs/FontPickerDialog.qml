// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>

// TEMPORARILY REMOVED DUE TO UI ISSUE WITH FLATPAK
// import QtQuick.Dialogs
//
// FontDialog {
//     property var caller
//
//     title: i18nc("@title:dialog", "Font selector")
//     
//     onCallerChanged: if (caller) {
//         selectedFont = caller.fontInfo
//     }
// }

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardDialog {
    id: scrollableDialog

    property string checkedFamily: "Note Sans"
    property int checkedSize: 10
    readonly property font selectedFont: ({
        family: checkedFamily,
        pointSize: checkedSize
    })
    property var caller

    title: i18nc("@title:dialog", "Font selector")

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    
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
    onRejected: {
        close()
    }

    contentItem: ColumnLayout {
        Layout.bottomMargin: Kirigami.Units.largeSpacing

        FormCard.FormComboBoxDelegate {
            id: fontBox

            displayMode: FormCard.FormComboBoxDelegate.Dialog
            model: Qt.fontFamilies() 

            Layout.fillWidth: true

            onCurrentValueChanged: {
                scrollableDialog.checkedFamily = currentValue;
            }
        }

        FormCard.FormComboBoxDelegate {
            id: sizeBox

            displayMode: FormCard.FormComboBoxDelegate.Dialog
            model: [6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72]

            Layout.fillWidth: true

            onCurrentValueChanged: {
                scrollableDialog.checkedSize = currentValue
            }
        }

        FormCard.FormSectionText {
            id: displayText

            text: fontBox.currentValue ? fontBox.currentValue : "Note Sans"
            font.family: text
            font.pointSize: sizeBox.currentValue ? sizeBox.currentValue : 10
            wrapMode: Text.NoWrap
            elide: Text.ElideRight

            Layout.fillWidth: true
        }
    }
}
