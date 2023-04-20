// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as Controls

Kirigami.PromptDialog {
    title: i18n("KleverNotes painting")

    // Dirty workaround to prevent weird height
    height: header.height + footer.height + topPadding + bottomPadding + mainItem.height

    showCloseButton: false
    closePolicy: Controls.Popup.NoAutoClose
    standardButtons: Kirigami.Dialog.Save | Kirigami.Dialog.Discard

    subtitle: i18n("You're about to leave without saving your drawing!\nDo you want to save it before leaving ?\n")
}

