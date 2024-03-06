// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs

import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"
import "qrc:/contents/ui/dialogs/emojiDialog"
import "qrc:/contents/ui/dialogs/imagePickerDialog"
import "qrc:/contents/ui/dialogs/tableMakerDialog"
import "qrc:/contents/ui/sharedComponents"

Kirigami.ActionToolBar {
    id: toolbar

    required property TextArea editorTextArea
    required property string notePath

    readonly property QtObject imagePickerDialog: imagePickerDialog

    readonly property var actionsTrigger: {
            "𝐇𝟏": function () {
                const [blockStart, blockEnd] = getBlockLimits()
                handleAction(blockStart, blockEnd, ["# "], false, false, false)
            },
            "𝐇𝟐": function () {
                const [blockStart, blockEnd] = getBlockLimits()
                handleAction(blockStart, blockEnd, ["## "], false, false, false)
            },
            "𝐇𝟑": function (){
                const [blockStart, blockEnd] = getBlockLimits()
                handleAction(blockStart, blockEnd, ["### "], false, false, false)
            },
            "𝐇𝟒": function () {
                const [blockStart, blockEnd] = getBlockLimits()
                handleAction(blockStart, blockEnd, ["#### "], false, false, false)
            },
            "𝐇𝟓": function (){
                const [blockStart, blockEnd] = getBlockLimits()
                handleAction(blockStart, blockEnd, ["###### "], false, false, false)
            },
            "𝐇𝟔": function () {
                const [blockStart, blockEnd] = getBlockLimits()
                handleAction(blockStart, blockEnd, ["####### "], false, false, false)
            },
        "bold": function () {
            handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["**","__"], true, false, false)
        },
        "italic": function () {
            handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["_","*"], true, false, false)
        },
        "strikethrough": function () {
            handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["~~"], true, false, false)
        },
        "codeBlock": function () {
            handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["\n```\n"], true, false, true)
        },
        "quote": function () {
            handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["> "], false, false, false)
        },
        "image": function () {
            imagePickerDialog.open()
        },
        "link": function () {
            linkDialog.open()
        },
        "table": function () {
            tableMakerDialog.open()
        },
        "orderedList": function () {
            const [blockStart, blockEnd] = getBlockLimits()
            handleAction(blockStart, blockEnd, [". "], false, true, false)
        },
        "unorderedList": function () {
            const [blockStart, blockEnd] = getBlockLimits()
            handleAction(blockStart, blockEnd, ["- "], false, false, false)
        },
        "highlight": function () {
            handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["=="], true, false, false)
        },
        "emoji": function () {
            emojiDialog.open()
        },
        "linkNote": function () {
            linkNoteDialog.open()
        }
    }

    // This 'replicate' the DefaultCardBackground and just change the background color
    //(https://api.kde.org/frameworks/kirigami/html/DefaultCardBackground_8qml_source.html)
    background: Kirigami.ShadowedRectangle{
        Kirigami.Theme.colorSet: Kirigami.Theme.Header
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor
        radius: Kirigami.Units.smallSpacing
    }

    actions: setupActions()

    ActionsList { id: actionsList } 

    EmojiDialog {
        id: emojiDialog

        onChosen: function (emoji) {
            editorTextArea.insert(editorTextArea.selectionStart, 
                Config.quickEmojiEnabled && Config.quickEmojiDialogEnabled ? (":" + emoji + ":") : emoji)
        }
    }

    ImagePickerDialog {
        id: imagePickerDialog

        noteImagesStoringPath: toolbar.notePath.replace("note.md","") + "Images/"

        onRejected: {
            storedImageChoosen = false
            applicationWindow().currentPageName = "Main"
        }
        onAccepted: if (imageLoaded) {
            let modifiedPath = path

            let useLocalImage = storedImageChoosen
            if (storeImage && !storedImageChoosen) {
                let wantedImageName = imageName

                if (wantedImageName.length === 0 && paintedImageChoosen) {
                    wantedImageName = "painting"
                } else if (!paintedImageChoosen) {
                    const fileName = KleverUtility.getName(path)
                    wantedImageName = fileName.substring(0,fileName.lastIndexOf("."))
                }

                // We can't asign the result to modifiedPath and use it to saveToFile or it won't work !
                const validPath = KleverUtility.getImageStoragingPath(noteImagesStoringPath, wantedImageName)
                modifiedPath = validPath

                imageObject.grabToImage(function(result) {
                    result.saveToFile(validPath)
                },Qt.size(imageObject.idealWidth,imageObject.idealHeight));

                useLocalImage = true

                storedImagesExist = true
            }

            if (modifiedPath.startsWith("file://")) modifiedPath = modifiedPath.replace("file://","")

            if (useLocalImage) modifiedPath = "./Images/"+modifiedPath.replace(noteImagesStoringPath,"")

            if (modifiedPath.startsWith("/home/")) {
                // Get the first "/" after the /home/username
                modifiedPath = modifiedPath.replace("/home/","")
                const idx = modifiedPath.indexOf("/")
                modifiedPath = "~" + modifiedPath.substring(idx)
            }

            let imageString = '![' + imageName + '](' + modifiedPath + ') '

            toolbar.editorTextArea.insert(toolbar.editorTextArea.cursorPosition, imageString)

            storedImageChoosen = false

            applicationWindow().currentPageName = "Main"
        }
    }

    TableMakerDialog {
        id: tableMakerDialog

        onAccepted: {
            const alignPattern = {
                "left": ":" + "-".repeat(i18n("Header").length - 1),
                "center": ":" + "-".repeat(i18n("Header").length - 2) + ":",
                "right": "-".repeat(i18n("Header").length - 1) + ":"
            }
            const cells = "|" + (" ".repeat(i18n("Header").length) + "|").repeat(tableMakerDialog.columnCount) + "\n"
            const headers = "|" + (i18n("Header") + "|").repeat(tableMakerDialog.columnCount) + "\n"

            let columnsAlignments = "|"

            for(var childIdx = 0; childIdx < tableMakerDialog.columnCount; childIdx++) {
                columnsAlignments = columnsAlignments.concat(alignPattern[tableMakerDialog.alignment],"|")
            }
            columnsAlignments += "\n"

            const result = "\n" + headers + columnsAlignments + cells.repeat(tableMakerDialog.rowCount-1)

            toolbar.editorTextArea.insert(toolbar.editorTextArea.cursorPosition, result)
        }
    }

    LinkDialog {
        id: linkDialog

        onAccepted: {
            let linkString = '[' + linkText + '](' + urlText + ') '
            toolbar.editorTextArea.insert(toolbar.editorTextArea.cursorPosition, linkString)
        }
    }

    LinkNoteDialog {
        id: linkNoteDialog

        listModel: applicationWindow().globalDrawer.treeModel

        onAccepted: {
            const text = linkText.trim()
            const notePath = path.substring(Config.storagePath.length)
            const headerPart = headerString.length > 0 
                ? " : " + headerString 
                : ""
            const textPart = text.length > 0 
                ? " | " + linkText 
                : ""
            const linkString = '[[' + notePath + headerPart + textPart + ']]'
            toolbar.editorTextArea.insert(toolbar.editorTextArea.cursorPosition, linkString)
        }
    }

    function handleAction(selectionStart, selectionEnd, specialChars,
                          multiPlaceApply, applyIncrement, checkByBlock) {

        const selectedText = editorTextArea.getText(selectionStart, selectionEnd)
        const newString = MDHandler.getNewText(selectedText, specialChars, multiPlaceApply, applyIncrement, checkByBlock)

        editorTextArea.remove(selectionStart, selectionEnd)
        editorTextArea.insert(selectionStart, newString)
        editorTextArea.select(selectionStart, selectionStart + newString.length)
    }

    function getBlockLimits() {
        return MDHandler.getBlockLimits(editorTextArea.selectionStart, 
            editorTextArea.selectionEnd, editorTextArea.text);
    }

    function addActionTrigger(currentAction) {
        const actionName = currentAction.name
        const currentActionName = currentAction.actionName

        const currentTrigger = actionsTrigger[currentActionName]
        if (currentAction.children.length > 0) {
            for (let i = 0 ; i < currentAction.children.length ; i++) {
                const actionChild = currentAction.children[i]
                const actionChildName = actionChild.actionName
                
                const actionChildTrigger = actionsTrigger[actionChildName]
                if (actionChildTrigger) {
                    actionChild.triggerFunction = actionChildTrigger
                }
            }
        } else if (currentTrigger) {
            currentAction.triggerFunction = currentTrigger
        }
    }

    function setupActions() {
        const currentActionList = actionsList.actions
        
        // Will be replaced by Config values
        const visibleIndexes = [] //[6, 13, 11, 4, 8, 5, 9] // test
        const invisibleIndexes = [] //[3, 7, 12, 2, 10] // test
        let visibleActions = Array(visible.length)

        let unknownActions = []
        for (var i = 0 ; i < currentActionList.length ; i++) {
            const currentAction = currentActionList[i]
            const visibleIndex = visibleIndexes.indexOf(i)
            const invisibleIndex = invisibleIndexes.indexOf(i)

            if (visibleIndex !== -1) {
                addActionTrigger(currentAction)
                visibleActions[visibleIndex] = currentAction
            } else if (invisibleIndex === -1) {
                addActionTrigger(currentAction)
                unknownActions.push(currentAction)
            }
        }

        const finalList = visibleActions.concat(unknownActions)

        return finalList
    }
}
