// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

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
            }
    }

    StorageDialog {
        id: storageDialog

        subtitle: i18n("Please choose a location for your future KleverNotes storage or select an existing one.\n")
        firstSetup: false

        onClosed: storageField.text = Config.storagePath
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
        spacing: 0

        FormCard.FormHeader {
            Layout.fillWidth: true
            title: i18n("General")
            visible: Qt.platform.os !== "android"
        }

        FormCard.FormCard {
            Layout.fillWidth: true

            ColumnLayout {
                RowLayout {
                    id: storageDelegate

                    Layout.margins: 0
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    Layout.fillWidth: true
                    FormCard.FormTextFieldDelegate {
                        id: storageField

                        text: Config.storagePath
                        label: i18n("Storage path:")

                        Layout.margins: 0
                        Layout.fillWidth: true
                        // workaround to make it readOnly
                        onTextChanged: text = Config.storagePath

                        MouseArea {
                            anchors.fill: parent
                            onClicked: storageDialog.open()
                        }
                    }
                }

                FormCard.FormDelegateSeparator { above: storageDelegate; below: categoryDelegate }

                RowLayout {
                    id: categoryDelegate

                    Layout.margins: 0
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    Layout.fillWidth: true
                    FormCard.FormTextFieldDelegate {
                        id: newCategoryField

                        text: Config.defaultCategoryName
                        label: i18n("New Category name:")

                        Layout.margins: 0
                        Layout.fillWidth: true

                        property bool isActive: false
                        property string name

                        onNameChanged: {
                            if (isActive) {
                                text = name
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
                }

                FormCard.FormDelegateSeparator { above: categoryDelegate; below: groupDelegate }

                RowLayout {
                    id: groupDelegate

                    Layout.margins: 0
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    Layout.fillWidth: true
                    FormCard.FormTextFieldDelegate {
                        id: newGroupField

                        text: Config.defaultGroupName
                        label: i18n("New Group name:")

                        Layout.margins: 0
                        Layout.fillWidth: true

                        property bool isActive: false
                        property string name

                        onNameChanged: {
                            if (isActive) {
                                text = name
                                Config.defaultGroupName = name
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
                }

                FormCard.FormDelegateSeparator { above: groupDelegate; below: noteDelegate }

                RowLayout {
                    id: noteDelegate

                    Layout.margins: 0
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    Layout.fillWidth: true
                    FormCard.FormTextFieldDelegate {
                        id: newNoteField

                        text: Config.defaultNoteName
                        label: i18n("New Note name:")

                        Layout.margins: 0
                        Layout.fillWidth: true

                        property bool isActive: false
                        property string name

                        onNameChanged: {
                            if (isActive) {
                                text = name
                                Config.defaultNoteName = name
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
                }
            }
        }

        Item {height: Kirigami.Units.largeSpacing * 2}

        FormCard.FormHeader {
            Layout.fillWidth: true
            title: i18n("Display")
            visible: Qt.platform.os !== "android"
        }

        DisplayPreview {
            id: displayPreview
        }
    }
}


