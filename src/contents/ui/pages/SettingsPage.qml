// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"
import "qrc:/contents/ui/dialogs/colorDialog"

Kirigami.ScrollablePage {
    id:settingsPage

    title: i18nc("@title:window", "Settings")

    function updateName(shownName,callingAction){
        namingDialog.shownName = shownName
        namingDialog.callingAction = callingAction
        namingDialog.open()
        namingDialog.nameField.selectAll()
        namingDialog.nameField.forceActiveFocus()
    }

    function updateColor(button, selectedColor) {
         switch(button.parent.name) {
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
            }
    }

    StorageDialog {
        id: storageDialog

        subtitle: i18n("Please choose a location for your future KleverNotes storage or select an existing one.\n")
        firstSetup: false
    }

    NamingDialog {
        id: namingDialog

        sideBarAction: false
        useCase: ""
        parentPath: ""
        realName: shownName
        newItem: false
    }

    ColorDialog {
        id: colorPicker

        property QtObject caller

        selectedColor: caller.color

        onAccepted: if (selectedColor != caller.color) updateColor(caller, selectedColor)
    }

    FontDialog{
        id: fontDialog

        onAccepted: Config.viewFont = checkedFamily
    }

    Kirigami.FormLayout {
        Kirigami.Separator {
            Kirigami.FormData.label: i18n("General")
            Kirigami.FormData.isSection: true
        }

        Row {
            Kirigami.FormData.label: i18n("Storage path:")

            Controls.TextField {
                text: Config.storagePath
                readOnly: true
            }

            Controls.Button{
                text: i18n("Change storage path")
                onClicked: storageDialog.open()
            }
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        Controls.TextField {
            id: newCategoryField

            Kirigami.FormData.label: i18n("New Category name:")

            readOnly: true
            text: Config.defaultCategoryName

            property bool isActive: false
            property string name

            onNameChanged: {
                if (isActive) {
                    Config.defaultCategoryName = name
                    isActive = false
                    name = ""
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    newCategoryField.isActive = true
                    updateName(newCategoryField.text, newCategoryField)
                }
            }
        }

        Controls.TextField {
            id: newGroupField

            Kirigami.FormData.label: i18n("New Group name:")

            readOnly: true
            text: Config.defaultGroupName

            property bool isActive: false
            property string name

            onNameChanged: {
                if (isActive) {
                    Config.defaultCategoryName = name
                    isActive = false
                    name = ""
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    newGroupField.isActive = true
                    updateName(newGroupField.text, newGroupField)
                }
            }
        }

        Controls.TextField {
            id: newNoteField

            Kirigami.FormData.label: i18n("New Note name:")

            readOnly: true
            text: Config.defaultNoteName

            property bool isActive: false
            property string name

            onNameChanged: {
                if (isActive) {
                    Config.defaultCategoryName = name
                    isActive = false
                    name = ""
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    newNoteField.isActive = newNoteField.true
                    updateName(newNoteField.text, newNoteField)
                }
            }
        }

        Kirigami.Separator {
            Kirigami.FormData.label: i18n("Note display")
            Kirigami.FormData.isSection: true
        }

        // We need to put the button in a raw to prevent a weird display bug and align the label and button correctly
        Row {
            Kirigami.FormData.label: i18n("Background color:")

            property string name: "background"

            Controls.Button {
                id: backgroundButton

                width: Kirigami.Units.largeSpacing * 20

                anchors.bottom: resetBackground.bottom
                anchors.top: resetBackground.top
                anchors.margins: Kirigami.Units.smallSpacing


                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                property string color: (Config.viewBodyColor !== "None") ? Config.viewBodyColor : Kirigami.Theme.backgroundColor

                background: Rectangle {
                    color: backgroundButton.color
                    radius: Kirigami.Units.smallSpacing
                }

                onClicked: {
                    colorPicker.caller = backgroundButton
                    colorPicker.open()
                }
            }

            Controls.Button {
                id: resetBackground
                icon.name: "edit-undo"

                onClicked: updateColor(resetBackground, "None")
            }
        }

        Row {
            Kirigami.FormData.label: i18n("Text color:")

            property string name: "text"

            Controls.Button {
                id: textButton

                width: Kirigami.Units.largeSpacing * 20

                anchors.bottom: resetText.bottom
                anchors.top: resetText.top
                anchors.margins: Kirigami.Units.smallSpacing

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                property string color: (Config.viewTextColor !== "None") ? Config.viewTextColor : Kirigami.Theme.textColor

                background: Rectangle {
                    color: textButton.color
                    radius: Kirigami.Units.smallSpacing
                }

                onClicked: {
                    colorPicker.caller = textButton
                    colorPicker.open()
                }
            }

            Controls.Button {
                id: resetText
                icon.name: "edit-undo"

                onClicked: updateColor(resetText, "None")
            }
        }

        Row {
            Kirigami.FormData.label: i18n("Title color:")

            property string name: "title"

            Controls.Button {
                id: titleButton

                width: Kirigami.Units.largeSpacing * 20

                anchors.bottom: resetTitle.bottom
                anchors.top: resetTitle.top
                anchors.margins: Kirigami.Units.smallSpacing

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                property string color: (Config.viewTitleColor !== "None") ? Config.viewTitleColor : Kirigami.Theme.disabledTextColor

                background: Rectangle {
                    color: titleButton.color
                    radius: Kirigami.Units.smallSpacing
                }

                onClicked: {
                    colorPicker.caller = titleButton
                    colorPicker.open()
                }
            }

            Controls.Button {
                id: resetTitle
                icon.name: "edit-undo"

                onClicked: updateColor(resetTitle, "None")
            }
        }

        Row {
            Kirigami.FormData.label: i18n("Link color:")

            property string name: "link"

            Controls.Button {
                id: linkButton

                width: Kirigami.Units.largeSpacing * 20

                anchors.bottom: resetLink.bottom
                anchors.top: resetLink.top
                anchors.margins: Kirigami.Units.smallSpacing

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                property string color: (Config.viewLinkColor !== "None") ? Config.viewLinkColor : Kirigami.Theme.linkColor

                background: Rectangle {
                    color: linkButton.color
                    radius: Kirigami.Units.smallSpacing
                }

                onClicked: {
                    colorPicker.caller = linkButton
                    colorPicker.open()
                }
            }

            Controls.Button {
                id: resetLink
                icon.name: "edit-undo"

                onClicked: updateColor(resetLink, "None")
            }
        }

        Row {
            Kirigami.FormData.label: i18n("Visited Link color:")

            property string name: "visitedLink"

            Controls.Button {
                id: visitedLinkButton

                width: Kirigami.Units.largeSpacing * 20

                anchors.bottom: resetVisitiedLink.bottom
                anchors.top: resetVisitiedLink.top
                anchors.margins: Kirigami.Units.smallSpacing

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                property string color: (Config.viewVisitedLinkColor !== "None") ? Config.viewVisitedLinkColor : Kirigami.Theme.visitedLinkColor

                background: Rectangle {
                    color: visitedLinkButton.color
                    radius: Kirigami.Units.smallSpacing
                }
                onClicked: {
                    colorPicker.caller = visitedLinkButton
                    colorPicker.open()
                }
            }

            Controls.Button {
                id: resetVisitiedLink
                icon.name: "edit-undo"

                onClicked: updateColor(resetVisitiedLink, "None")
            }
        }

        Row {
            Kirigami.FormData.label: i18n("Code color:")

            property string name: "code"

            Controls.Button {
                id: codeButton

                width: Kirigami.Units.largeSpacing * 20

                anchors.bottom: resetCode.bottom
                anchors.top: resetCode.top
                anchors.margins: Kirigami.Units.smallSpacing

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                property string color: (Config.viewCodeColor !== "None") ? Config.viewCodeColor : Kirigami.Theme.alternateBackgroundColor

                background: Rectangle {
                    color: codeButton.color
                    radius: Kirigami.Units.smallSpacing
                }

                onClicked: {
                    colorPicker.caller = codeButton
                    colorPicker.open()
                }
            }

            Controls.Button {
                id: resetCode
                icon.name: "edit-undo"

                onClicked: updateColor(resetCode, "None")
            }
        }

        Controls.TextField {
            id: fontDisplay

            Kirigami.FormData.label: i18n("Font:")

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false

            readOnly: true
            text: (Config.viewFont !== "None") ? Config.viewFont : Kirigami.Theme.defaultFont.family
            font.family: text

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    fontDialog.currentFamily = parent.text
                    fontDialog.open()
                }
            }
        }
    }
}


