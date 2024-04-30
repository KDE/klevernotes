// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

import "qrc:/contents/ui/sharedComponents"
import "qrc:/contents/ui/textEditor"
import "qrc:/contents/ui/dialogs/tableMakerDialog"
import "qrc:/contents/ui/settings/components"

ColumnLayout {
    id: root

    Layout.fillWidth: true
    Layout.fillHeight: true

    Component.onCompleted: fillModel()

    FormCard.FormHeader {
        title: i18nc("@title, general storage settings, Storage as in 'the folder where all the notes will be stored'", "Storage")
        Layout.fillWidth: true
    }

    FormCard.FormCard {
        Layout.fillWidth: true

        FormCard.FormTextFieldDelegate {
            id: storageField

            text: Config.storagePath
            label: i18nc("@label:textbox, Storage as in 'the folder where all the notes will be stored'", "Storage path:")

            Layout.margins: 0
            Layout.fillWidth: true
            
            // workaround to make it readOnly
            onTextChanged: {
                text = Config.storagePath
            }

            MouseArea {
                anchors.fill: parent
                onClicked: storageDialog.open()
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title, general sidebar settings", "Sidebar")
        Layout.fillWidth: true
    }

    FormCard.FormCard {
        Layout.fillWidth: true

        FormCard.FormTextFieldDelegate {
            id: newCategoryField

            property string name
            property bool isActive: false

            text: Config.defaultCategoryName
            label: i18nc("@label:textbox, the default note category name", "New Category name:")

            Layout.margins: 0
            Layout.fillWidth: true

            onNameChanged: if (isActive) {
                text = name
                Config.defaultCategoryName = name
                isActive = false
                name = ""
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    newCategoryField.isActive = true
                    updateName(newCategoryField.text, newCategoryField)
                }
            }
        }

        FormCard.FormDelegateSeparator { above: newCategoryField; below: newGroupField }

        FormCard.FormTextFieldDelegate {
            id: newGroupField

            property string name
            property bool isActive: false

            text: Config.defaultGroupName
            label: i18nc("@label:textbox, the default note group name", "New Group name:")

            Layout.margins: 0
            Layout.fillWidth: true

            onNameChanged: if (isActive) {
                text = name
                Config.defaultGroupName = name
                isActive = false
                name = ""
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    newGroupField.isActive = true
                    updateName(newGroupField.text, newGroupField)
                }
            }
        }

        FormCard.FormDelegateSeparator { above: newGroupField; below: newNoteField }

        FormCard.FormTextFieldDelegate {
            id: newNoteField

            property string name
            property bool isActive: false

            text: Config.defaultNoteName
            label: i18nc("@label:textbox, the default note name", "New Note name:")

            Layout.margins: 0
            Layout.fillWidth: true

            onNameChanged: if (isActive) {
                text = name
                Config.defaultNoteName = name
                isActive = false
                name = ""
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

    FormCard.FormHeader {
        title: i18nc("@title, general text editor settings", "Editor")
        Layout.fillWidth: true
    }

    FormCard.FormCard {
        Layout.fillWidth: true

        ExpandingFormSwitch {
            id: spaceTabSwitch

            text: i18nc("@label:checkbox", "Use spaces for tab")
            checked: Config.useSpaceForTab

            onCheckedChanged: if (checked != Config.useSpaceForTab) {
                Config.useSpaceForTab = checked
            }

            FormCard.FormSpinBoxDelegate {
                id: spaceSpinBox

                property bool isInit: false

                label: i18nc("@label:combobox", "Number of spaces")

                from: 1
                to: 8

                Component.onCompleted: {
                    value = Config.spacesForTab
                    isInit = true
                }
                onValueChanged: {
                    if (value != Config.spacesForTab && isInit) Config.spacesForTab = value
                }
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title, general text toolbar settings", "Toolbar")
        Layout.fillWidth: true
    }

    FormCard.FormCard {
        Layout.fillWidth: true

        Kirigami.Heading {
            text: i18nc("@title, list of visible toolbar actions", "Visible")
            level: 4
            Layout.margins: Kirigami.Units.largeSpacing
            Layout.bottomMargin: visibleScrollView.viewCount > 0 ? Kirigami.Units.largeSpacing : 0
        }

        Controls.ScrollView { // Prevent the Page from stealing the scroll
            id: visibleScrollView
            readonly property int maxHeight: Kirigami.Units.gridUnit * 10
            readonly property int viewCount: visibleView.count
            Layout.fillWidth: true
            Layout.preferredHeight: viewCount > 3 
                ? maxHeight + Kirigami.Units.largeSpacing
                : viewCount > 0
                    ? ((Math.round(maxHeight) / 3) * viewCount) + Kirigami.Units.gridUnit
                    : 0
 
            Layout.topMargin: viewCount > 0 ? 0 : Kirigami.Units.gridUnit
            ListView {
                id: visibleView

                anchors.fill: parent
                topMargin: Kirigami.Units.largeSpacing
                bottomMargin: Kirigami.Units.largeSpacing
                leftMargin: Kirigami.Units.largeSpacing
                rightMargin: Kirigami.Units.largeSpacing
                contentWidth: 0 // disable ScrollView horizontal bar

                clip: true
                reuseItems: true
                spacing: Kirigami.Units.largeSpacing

                model: ListModel { id: visibleModel }

                delegate: ActionListViewDelegate {
                    width: visibleView.width - Kirigami.Units.gridUnit * 2
                    height: Kirigami.Units.gridUnit * 3
                    isVisible: true

                    onMoveItem: (newIndex) => {
                        if (newIndex >= visibleView.count) {
                            newIndex = 0
                        } else if (newIndex < 0) {
                            newIndex = visibleView.count - 1
                        }
                        root.moveItem(visibleModel, index, visibleModel, newIndex)
                    }
                    onChangeList: {
                        root.moveItem(visibleModel, index, invisibleModel)
                    } 
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.topMargin: visibleScrollView.viewCount > 0 ? Kirigami.Units.gridUnit : 0
            Layout.rightMargin: Kirigami.Units.gridUnit * 5
        }

        Kirigami.Heading {
            text: i18nc("@title, list of invisible toolbar actions", "Invisible")
            level: 4
            Layout.topMargin: Kirigami.Units.gridUnit
            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing
        }

        Controls.ScrollView { // Prevent the Page from stealing the scroll
            readonly property int maxHeight: Kirigami.Units.gridUnit * 10
            readonly property int viewCount: invisibleView.count

            Layout.fillWidth: true
            Layout.preferredHeight: viewCount > 3 
                ? maxHeight + Kirigami.Units.largeSpacing
                : viewCount > 0
                    ? ((Math.round(maxHeight) / 3) * viewCount) + Kirigami.Units.gridUnit
                    : 0 

            Layout.topMargin: viewCount > 0 ? 0 : Kirigami.Units.gridUnit
            ListView {
                id: invisibleView

                anchors.fill: parent
                topMargin: Kirigami.Units.largeSpacing
                bottomMargin: Kirigami.Units.largeSpacing
                leftMargin: Kirigami.Units.largeSpacing
                rightMargin: Kirigami.Units.largeSpacing
                contentWidth: 0 // disable ScrollView horizontal bar

                clip: true
                reuseItems: true
                spacing: Kirigami.Units.largeSpacing

                model: ListModel { id: invisibleModel }

                delegate: ActionListViewDelegate {
                    width: invisibleView.width - Kirigami.Units.gridUnit * 2
                    height: Kirigami.Units.gridUnit * 3
                    isVisible: false

                    onChangeList: {
                        root.moveItem(invisibleModel, index, visibleModel)
                    }
                }
            }
        }
    }

    ActionsList{ id: actionsList }

    function getActionInfo(currentAction, ogIndex) {
        const actionDesc = currentAction.description
        const actionIcon = currentAction.icon.name

        return {description: actionDesc, iconName: actionIcon, ogIndex: ogIndex}
    }

    function fillModel(model) {
        const currentActionList = actionsList.actions
        
        const visibleIndexes = Config.visibleTools
        const invisibleIndexes = Config.invisibleTools
        
        // prefill the model, so we can set the correct property later
        for (var i = 0 ; i < visibleIndexes.length ; i++) {
            visibleModel.append({})
        }
       
        let unknownActions = []
        let unknownIndexes = []
        for (var i = 0 ; i < currentActionList.length ; i++) {
            const currentAction = currentActionList[i]
            const visibleIndex = visibleIndexes.indexOf(i)
            const invisibleIndex = invisibleIndexes.indexOf(i)

            if (visibleIndex !== -1) {
                visibleModel.set(visibleIndex, getActionInfo(currentAction, i))
            } else if (invisibleIndex !== -1) {
                invisibleModel.append(getActionInfo(currentAction, i))
            } else {
                unknownActions.push(currentAction)
                unknownIndexes.push(i)
            }
        }

        if (unknownIndexes.length > 0) {
            for (var i = 0 ; i < unknownActions.length ; i++) {
                const currentAction = currentActionList[i]
                visibleModel.append(getActionInfo(currentAction, visibleIndexes.length + i))
            }
            Config.visibleTools = visibleIndexes.concat(unknownIndexes)
        }
    }

    function moveItem(modelFrom, indexFrom, modelTo, indexTo) {
        let visibleIndexes = Config.visibleTools
        let invisibleIndexes = Config.invisibleTools
        const fromVisible = modelFrom === visibleModel

        if (modelFrom == modelTo) {
            modelFrom.move(indexFrom, indexTo, 1)
            
            let indexesList = fromVisible ? visibleIndexes : invisibleIndexes
            const element = indexesList[indexFrom];
            indexesList.splice(indexFrom, 1);
            indexesList.splice(indexTo, 0, element);
            
            if (fromVisible) Config.visibleTools = indexesList
        } else {
            const item = modelFrom.get(indexFrom)
            // The item ogIndex will change when we remove it
            const ogMemory = item.ogIndex

            modelTo.append(item)
            modelFrom.remove(indexFrom, 1)

            // Need to set it once again
            modelTo.setProperty(modelTo.count - 1, "ogIndex", ogMemory)

            const newItem = modelTo.get(modelTo.count - 1)
            if (fromVisible) {
                // Can't be in one line, would keep the old value
                invisibleIndexes.push(newItem.ogIndex)
                Config.invisibleTools = invisibleIndexes
                // Can't be in one line, would keep the old value
                visibleIndexes.splice(indexFrom, 1)
                Config.visibleTools = visibleIndexes 
            } else {
                // Can't be in one line, would keep the old value
                visibleIndexes.push(newItem.ogIndex)
                Config.visibleTools = visibleIndexes
                // Can't be in one line, would keep the old value
                invisibleIndexes.splice(indexFrom, 1)
                Config.invisibleTools = invisibleIndexes
            }
        }
    }
}
