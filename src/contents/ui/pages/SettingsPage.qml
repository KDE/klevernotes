// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

import "qrc:/contents/ui/settings"
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

        useCase: ""
        parentPath: ""
        newItem: false
    }

    ColorDialog {
        id: colorPicker

        property QtObject caller

        selectedColor: caller.color

        onApplied: {
            if (selectedColor != caller.color) updateColor(caller, selectedColor)
            colorPicker.close()
        }
    }

    FontPickerDialog{
        id: fontDialog

        onApplied: {
            Config.viewFont = checkedFamily
            fontDialog.close()
        }
    }
    ColumnLayout {
        anchors.fill: parent
        Kirigami.FormLayout {
            id: formLayout

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
                        newNoteField.isActive = true
                        updateName(newNoteField.text, newNoteField)
                    }
                }
            }

            Kirigami.Separator {
                Kirigami.FormData.label: i18n("Note display")
                Kirigami.FormData.isSection: true
            }
        }

        DisplayPreview {
            id: displayPreview

            Layout.maximumWidth: Kirigami.Units.gridUnit * 40
            Layout.margins: formLayout.wideMode ? Kirigami.Units.largeSpacing * 5 : 0
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.alignment: Qt.AlignCenter
        }
    }
}


