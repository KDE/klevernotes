// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FontDialog {
    property var caller

    title: i18nc("@title:dialog", "Font selector")
    
    onCallerChanged: if (caller) {
        selectedFont = caller.fontInfo
    }
}
