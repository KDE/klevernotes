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
                storageField.text = Config.storagePath
            }
        },
        NamingDialog {
            id: namingDialog

            useCase: ""
            parentPath: ""
            newItem: false

            onRejected: {
                close()
            }
        },
        FontPickerDialog {
            id: fontDialog

            onApplied: {
                caller.newFont = Qt.font({"family": checkedFamily, "pointSize": checkedSize})
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
        id: appearanceLoader

        Layout.fillWidth: true
        Layout.fillHeight: true
 
        sourceComponent: AppearanceTab {}

        active: tabBar.currentTab === "appearance" 
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

    onBackRequested: {
        applicationWindow().currentPageName = "Main"
    }

    function updateName(shownName, callingAction) {
        namingDialog.shownName = shownName
        namingDialog.textFieldText = shownName
        namingDialog.callingAction = callingAction
        namingDialog.open()
        // Can't use selectAll on FormTextFieldDelegate
        // namingDialog.nameField.selectAll()
        namingDialog.nameField.forceActiveFocus()
    }

    function updateColor(button, selectedColor) {
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
