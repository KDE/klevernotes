// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"

Controls.Menu {
	id: contextMenu

	required property QtObject actionBar
	required property QtObject treeView

	property bool canDelete: false

	Controls.MenuItem {
		icon.name: "journal-new"
		text: i18n("New category")

		onTriggered: actionBar.createCategoryAction.triggered()
	}

	Controls.MenuItem {
		icon.name: "folder-new"
		text: i18n("New group")

		onTriggered: actionBar.createGroupAction.triggered()
	}

	Controls.MenuItem {
		icon.name: "document-new"
		text: i18n("New note")

		onTriggered: actionBar.createNoteAction.triggered()
	}

	Controls.MenuItem {
		icon.name: "edit-rename"
		text: i18n("Rename")

		onTriggered: actionBar.renameAction.triggered()
	}

	Controls.MenuItem {
		icon.name: "user-trash-symbolic"
		text: i18n("Delete")

		visible: contextMenu.canDelete
		onTriggered: deleteConfirmationDialog.open()
	}

	DeleteConfirmationDialog {
        id: deleteConfirmationDialog

        useCase: treeView.currentItem.useCase

        onAccepted: {
            StorageHandler.remove(treeView.currentItem.path, actionBar.currentModelIndex)
        }
    }

    Connections {
        target: StorageHandler

        function onStorageUpdated(modelIndex) {
			treeView.model.removeFromTree(modelIndex)
        }
    }
}

