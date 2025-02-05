// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls as Controls

import org.kde.Klever

import "qrc:/contents/ui/dialogs"

Controls.Menu {
    id: contextMenu

    required property ActionBar actionBar
    required property QtObject treeView

    Controls.MenuItem {
        icon.name: "folder-new-symbolic"
        text: i18nc("@item:menu", "New folder")

        onTriggered: {
            actionBar.createFolderAction.triggered()
        }
    }

    Controls.MenuItem {
        icon.name: "document-new-symbolic"
        text: i18nc("@item:menu", "New note")

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

        isNote: treeView.currentClickedItem.isNote
        name: treeView.currentClickedItem ? treeView.currentClickedItem.text : ""

        onAccepted: {
            treeView.model.removeFromTree(treeView.currentModelIndex, permanent)
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

        treeView: contextMenu.treeView
        useCase: "note" // Will change

        onApplied: if (clickedIndex && treeView.currentModelIndex) {
            applicationWindow().saveState()
            contextMenu.treeView.model.moveRow(treeView.currentModelIndex, clickedIndex)
            close()
        }
        onRejected: {
            close()
        }
    }
}
