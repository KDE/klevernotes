// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

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
        text: i18nc("@item:menu", "Rename")

        onTriggered: {
            actionBar.renameAction.triggered()
        }
    }

    Controls.MenuItem {
        icon.name: "edit-move-symbolic"
        text: i18nc("@item:menu", "Move")

        onTriggered: if (visible) { // A bit useless right now, but might be handy if we had a shortcut
            moveDialog.open()
        }
    }

    Controls.MenuItem {
        icon.name: "user-trash-symbolic"
        text: i18nc("@item:menu", "Delete")

        onTriggered: {
            deleteConfirmationDialog.open()
        }
    }

    Controls.MenuItem {
        icon.name: "document-properties-symbolic"
        text: i18nc("@item:menu", "Properties")

        onTriggered: {
            itemPropertiesDialog.itemTitle = treeView.currentClickedItem.name
            itemPropertiesDialog.color = treeView.currentClickedItem.color 
                ? treeView.currentClickedItem.color 
                : treeView.currentClickedItem.defaultColor
            itemPropertiesDialog.iconName = treeView.currentClickedItem.icon.name
            itemPropertiesDialog.open()
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
            treeView.useCurrentItem()
        }
    }

    MoveDialog {
        id: moveDialog

        treeView: contextMenu.treeView
        isNote: treeView.currentClickedItem.isNote

        onApplied: if ((clickedIndex || moveDialog.moveToRoot) && treeView.currentModelIndex) {
            applicationWindow().saveState()
            if (moveDialog.moveToRoot) {
                treeView.model.moveRow(treeView.currentModelIndex)
            } else {
                treeView.model.moveRow(treeView.currentModelIndex, clickedIndex)
            } 
            close()
        }
        onRejected: {
            close()
        }
        onClosed: {
            treeView.useCurrentItem()
        }
    }

    ItemPropertiesDialog {
        id: itemPropertiesDialog

        onResetProperty: function (val) {
            switch (val) {
                case 0:
                    itemPropertiesDialog.iconName = treeView.currentClickedItem.defaultIcon
                    break;
                case 1:
                    itemPropertiesDialog.color = treeView.currentClickedItem.defaultColor
                    break;
            }
        }

        onApplied: {
            // No point in saving the default values
            const c = itemPropertiesDialog.color === treeView.currentClickedItem.defaultColor ? "" : itemPropertiesDialog.color
            const i = itemPropertiesDialog.iconName === treeView.currentClickedItem.defaultIcon ? "" : itemPropertiesDialog.iconName
            treeView.model.setProperties(treeView.currentModelIndex, c, i)
            close()
        }
    }
}
