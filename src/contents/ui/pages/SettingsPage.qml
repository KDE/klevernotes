// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

import "qrc:/contents/ui/settings"
import "qrc:/contents/ui/dialogs"

FormCard.FormCardPage {
    id: settingsPage

    title: i18nc("@title:window", "Settings")

    data: [
        StorageDialog {
            id: storageDialog

            subtitle: i18n("Please choose a location for your future KleverNotes storage or select an existing one.\n")
            firstSetup: false

            onClosed: {
                generalLoader.item.displayedStoragePath = Config.storagePath
            }
        },
        NamingDialog {
            id: namingDialog

            isNote: false
            parentPath: ""
            newItem: false

            onRejected: {
                close()
            }
        },
        FontPickerDialog {
            id: fontDialog

            onAccepted: {
                caller.newFont = fontDialog.selectedFont
                fontDialog.close()
            }
        }
    ]

    header: TabBar {
        id: tabBar

        Layout.fillWidth: true
    }

    Loader {
        id: generalLoader

        Layout.fillWidth: true
        Layout.fillHeight: true
        
        sourceComponent: GeneralTab {}

        active: tabBar.currentTab === "general" 
        visible: active
    }

    Loader {
        id: notePreviewLoader

        Layout.fillWidth: true
        Layout.fillHeight: true
 
        sourceComponent: NotePreviewTab {}

        active: tabBar.currentTab === "preview" 
        visible: active
    }

    Loader {
        id: editorLoader

        Layout.fillWidth: true
        Layout.fillHeight: true
 
        sourceComponent: EditorTab {}

        active: tabBar.currentTab === "editor" 
        visible: active
    }

    Loader {
        id: pluginsLoader

        Layout.fillWidth: true
        Layout.fillHeight: true
 
        sourceComponent: PluginsTab {}

        active: tabBar.currentTab === "plugins" 
        visible: active
    }

    onBackRequested: (event) => {
        event.accepted = true;
        applicationWindow().switchToPage("Main")
    }

    function updateName(isNote: bool, shownName: string, callBackFunc: var): void {
        namingDialog.isNote = isNote
        namingDialog.shownName = shownName
        namingDialog.textFieldText = shownName
        namingDialog.callBackFunc = callBackFunc
        namingDialog.open()
        namingDialog.nameField.selectAll()
    }

    function updateColor(button: QtObject, selectedColor: string): void {
        switch(button.name) {
            case "background":
                Config.viewBodyColor = selectedColor
                break;
            case "text":
                Config.viewTextColor = selectedColor
                break;
            case "title":
                Config.viewTitleColor = selectedColor
                break;
            case "link":
                Config.viewLinkColor = selectedColor
                break;
            case "visitedLink":
                Config.viewVisitedLinkColor = selectedColor
                break;
            case "code":
                Config.viewCodeColor = selectedColor
                break;
            case "highlight":
                Config.viewHighlightColor = selectedColor
                break;
        }
    }
}
