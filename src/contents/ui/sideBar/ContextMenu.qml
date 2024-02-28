// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"

Controls.Menu {
    id: contextMenu

    required property ActionBar actionBar
    required property TreeView treeView

    Controls.MenuItem {
        text: i18nc("as in 'A note category'", "New category")
        icon.name: "journal-new"

        onTriggered: {
            actionBar.createCategoryAction.triggered()
        }
    }

    Controls.MenuItem {
        icon.name: "folder-new"
        text: i18nc("as in 'A note group'", "New group")

        onTriggered: {
            actionBar.createGroupAction.triggered()
        }
    }

    Controls.MenuItem {
        icon.name: "document-new"
        text: i18nc("as in 'A note'", "New note")

        onTriggered: {
            actionBar.createNoteAction.triggered()
        }
    }

    Controls.MenuItem {
        icon.name: "edit-rename"
        text: i18n("Rename")

        onTriggered: {
            actionBar.renameAction.triggered()
        }
    }

    Controls.MenuItem {
        icon.name: "user-trash-symbolic"
        text: i18n("Delete")

        onTriggered: {
            deleteConfirmationDialog.open()
        }
    }

    DeleteConfirmationDialog {
        id: deleteConfirmationDialog

        useCase: actionBar.currentClickedItem ? actionBar.currentClickedItem.useCase : ""

        onAccepted: {
            treeView.model.removeFromTree(actionBar.currentModelIndex)
        }
        onClosed: {
            actionBar.useCurrentItem()
        }
    }
}
