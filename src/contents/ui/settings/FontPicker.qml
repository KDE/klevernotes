// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.Klever 1.0

FormCard.FormTextFieldDelegate {
    required property font configFont

    property font newFont
    property var fontInfo

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    Layout.margins: 0

    onConfigFontChanged: {
        setInfo()
    }
    Component.onCompleted: {
        setInfo()
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            fontDialog.caller = parent
            fontDialog.open()
        }
    }

    function setInfo() {
        fontInfo = KleverUtility.fontInfo(configFont)

        font.family = fontInfo.family
        text = fontInfo.family + " " + fontInfo.pointSize + "pt"
    }
}
