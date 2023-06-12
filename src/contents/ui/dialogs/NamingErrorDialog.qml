// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami

Kirigami.PromptDialog {
    title: "Klever Notes Storage"

    showCloseButton: false
    closePolicy: Controls.Popup.NoAutoClose
    standardButtons: Kirigami.Dialog.Ok

    property string error
    property string useCase
    property QtObject nameField

    readonly property var useCaseTrad: {
        "Category": i18n("category"),
        "Group": i18n("group"),
        "Note": i18n("note")
    }

    subtitle: setSubtitle()

    onAccepted: nameField.selectAll()

    function setSubtitle() {
        if (error === "dot") {
            return i18n("Your "+useCaseTrad[useCase]+" starts with a dot, "+
                        "this may cause problem and is therefore not allowed.\n"+
                        "Please remove the dot or choose another name.")
        }
        else if (error === "exist") {
            return i18n("This "+useCaseTrad[useCase]+" already exist.\nPlease choose another name for it.\n")
        }
    }
}
