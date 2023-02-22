// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami

Kirigami.Dialog {
    id: scrollableDialog
    title: i18n("Font selector")

    property string currentFamily
    property string checkedFamily

    standardButtons: Kirigami.Dialog.Apply | Kirigami.Dialog.Cancel

    ListView {
        id: listView
        implicitWidth: Kirigami.Units.gridUnit * 16
        implicitHeight: Kirigami.Units.gridUnit * 16

        model: Qt.fontFamilies()

        delegate: Controls.RadioDelegate {
            topPadding: Kirigami.Units.smallSpacing * 2
            bottomPadding: Kirigami.Units.smallSpacing * 2
            implicitWidth: listView.width
            text: modelData
            font.family: modelData
            checked: currentFamily == modelData
            onCheckedChanged: if (checked) scrollableDialog.checkedFamily = modelData
        }
    }
}
