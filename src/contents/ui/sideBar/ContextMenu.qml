// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls as Controls

import org.kde.Klever

import "qrc:/contents/ui/dialogs"

Controls.Menu {
    id: contextMenu

    required property ActionBar actionBar
    required property QtObject treeView

    Controls.MenuItem {
        text: i18nc("as in 'A note category'", "New category")
        icon.name: "journal-new-symbolic"

        onTriggered: {
            actionBar.createCategoryAction.triggered()
        }
    }

    Controls.MenuItem {
        icon.name: "folder-new-symbolic"
        text: i18nc("as in 'A note group'", "New group")

        onTriggered: {
            actionBar.createGroupAction.triggered()
        }
    }

    Controls.MenuItem {
        icon.name: "document-new-symbolic"
        text: i18nc("as in 'A note'", "New note")

        onTriggered: {
            actionBar.createNoteAction.triggered()
        }
    }

    Controls.MenuItem {
        icon.name: "edit-rename-symbolic"
        text: i18n("Rename")

        onTriggered: {
            actionBar.renameAction.triggered()
        }
    }

    Controls.MenuItem {
        icon.name: "edit-move-symbolic"
        text: i18n("Move")
        visible: actionBar.currentClickedItem.useCase !== "Category"

        onTriggered: if (visible) { // A bit useless right now, but might be handy if we had a shortcut
            moveDialog.open()
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
        name: actionBar.currentClickedItem ? actionBar.currentClickedItem.text : ""

        onAccepted: {
            treeView.model.removeFromTree(actionBar.currentModelIndex)
            close()
        }
        onRejected: {
            close()
        }
        onClosed: {
            actionBar.useCurrentItem()
        }
    }

    MoveDialog {
        id: moveDialog

        treeView: contextMenu.treeView.model
        useCase: actionBar.currentClickedItem ? actionBar.currentClickedItem.useCase : ""

        onApplied: if (clickedIndex && actionBar.currentModelIndex) {
            applicationWindow().saveState()
            contextMenu.treeView.model.moveRow(actionBar.currentModelIndex, clickedIndex)
            close()
        }
        onRejected: {
            close()
        }
    }
}
