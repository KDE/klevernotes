// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.klevernotes

FormCard.FormTextFieldDelegate {
    required property font configFont

    property font newFont
    property var fontInfo

    readonly property string fontFamily: fontInfo.family
    readonly property int fontPointSize: fontInfo.pointSize

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

        text = fontInfo.family + " " + fontInfo.pointSize + "pt"
    }
}
