// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami

import org.kde.Klever

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
            "h1": function (checked) {
                EditorHandler.handleDelims(checked, -10)
            },
            "h2": function (checked) {
                EditorHandler.handleDelims(checked, -9)
            },
            "h3": function (checked) {
                EditorHandler.handleDelims(checked, -8)
            },
            "h4": function (checked) {
                EditorHandler.handleDelims(checked, -7)
            },
            "h5": function (checked) {
                EditorHandler.handleDelims(checked, -6)
            },
            "h6": function (checked) {
                EditorHandler.handleDelims(checked, -5)
            },
        "bold": function (checked) {
            EditorHandler.handleDelims(checked, 1)
        },
        "italic": function (checked) {
            EditorHandler.handleDelims(checked, 2)
        },
        "strikethrough": function (checked) {
            EditorHandler.handleDelims(checked, 4)
        },
        "codeBlock": function (checked) {
            EditorHandler.handleDelims(checked, -4)
        },
        "quote": function () {
            EditorHandler.handleDelims(checked, -3)
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
        "orderedList": function (checked) {
            EditorHandler.handleDelims(checked, -2)
        },
        "unorderedList": function (checked) {
            EditorHandler.handleDelims(checked, -1)
        },
        "highlight": function (checked) {
            EditorHandler.handleDelims(checked, 8)
        },
        "emoji": function () {
            emojiDialog.open()
        },
        "linkNote": function () {
            linkNoteDialog.open()
        }
    }
    readonly property list<int> visibleTools: Config.visibleTools

    property var actionPositionMap;

    // This 'replicate' the DefaultCardBackground and just change the background color
    //(https://api.kde.org/frameworks/kirigami/html/DefaultCardBackground_8qml_source.html)
    background: Kirigami.ShadowedRectangle{
        Kirigami.Theme.colorSet: Kirigami.Theme.Header
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor
        radius: Kirigami.Units.smallSpacing
    }

    onVisibleToolsChanged: {
        actions = setupActions()
    }

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
                } else if (wantedImageName.length === 0 && clipboardImage) {
                    wantedImageName = "clipboard"
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

            imagePickerDialog.close()
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

            const result = "\n" + headers + columnsAlignments + cells.repeat(tableMakerDialog.rowCount)

            toolbar.editorTextArea.insert(toolbar.editorTextArea.cursorPosition, result)

            tableMakerDialog.close()
        }
    }

    LinkDialog {
        id: linkDialog

        onAccepted: {
            let linkString = '[' + linkText + '](' + urlText + ') '
            toolbar.editorTextArea.insert(toolbar.editorTextArea.cursorPosition, linkString)

            close()
        }
    }

    LinkNoteDialog {
        id: linkNoteDialog

        listModel: applicationWindow().globalDrawer.treeModel

        onAccepted: {
            const text = linkText.trim()
            const notePath = path.substring(Config.storagePath.length, path.length - 3)
            const headerPart = headerString.length > 0 
                ? " : " + headerString 
                : ""
            const textPart = text.length > 0 
                ? " | " + linkText 
                : ""
            const linkString = '[[' + notePath + headerPart + textPart + ']]'
            toolbar.editorTextArea.insert(toolbar.editorTextArea.cursorPosition, linkString)

            close()
        }
    }

    function getDelimInfo(delimType) {
        let actionIndex = undefined;
        let headingLevel = 0;
        switch (delimType) {
            case -10:
                actionIndex = toolbar.actionPositionMap["h"]
                headingLevel = 1
                break;
            case -9:
                actionIndex = toolbar.actionPositionMap["h"]
                headingLevel = 2
                break;
            case -8:
                actionIndex = toolbar.actionPositionMap["h"]
                headingLevel = 3
                break;
            case -7:
                actionIndex = toolbar.actionPositionMap["h"]
                headingLevel = 4
                break;
            case -6:
                actionIndex = toolbar.actionPositionMap["h"]
                headingLevel = 5
                break;
            case -5:
                actionIndex = toolbar.actionPositionMap["h"]
                headingLevel = 6
                break;
            case -4:
                actionIndex = toolbar.actionPositionMap["codeBlock"]
                break;
            case -3:
                actionIndex = toolbar.actionPositionMap["quote"]
                break;
            case -2:
                actionIndex = toolbar.actionPositionMap["orderedList"]
                break;
            case -1:
                actionIndex = toolbar.actionPositionMap["unorderedList"]
                break;
            case 1:
                actionIndex = toolbar.actionPositionMap["bold"]
                break;
            case 2:
                actionIndex = toolbar.actionPositionMap["italic"]
                break;
            case 4:
                actionIndex = toolbar.actionPositionMap["strikethrough"]
                break;
            case 8:
                actionIndex = toolbar.actionPositionMap["highlight"]
                break;
        }
        return [actionIndex, headingLevel]
    }

    function checkSourrindingDelimsActions(delimsTypes) {
        for (let i = 0 ; i < toolbar.actions.length ; i++) {
            let currentAction = toolbar.actions[i]
            for (let j = 0 ; j < currentAction.children.length ; j++) {
                const actionChild = currentAction.children[j].checked = false
            }
            currentAction.checked = false
        }

        for (let i = 0 ; i < delimsTypes.length ; i++) {
            const [actionIndex, headingLevel] = getDelimInfo(delimsTypes[i])
            if (actionIndex !== undefined) {
                if (headingLevel) {
                    // TODO: rework toolbar to better show when this is checked
                    toolbar.actions[actionIndex].children[headingLevel - 1].checked = true
                } else {
                    toolbar.actions[actionIndex].checked = true
                }
            }
        }
    }

    function uncheckAction(delimType) {
        const [actionIndex, headingLevel] = getDelimInfo(delimType)
        if (actionIndex !== undefined) {
            if (headingLevel) {
                // TODO: rework toolbar to better show when this is checked
                toolbar.actions[actionIndex].children[headingLevel - 1].checked = false
            } else {
                toolbar.actions[actionIndex].checked = false
            }
        }

    }

    function addActionTrigger(currentAction) {
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
        const allActionsLen = currentActionList.length

        const visibleToolsIndexes = Config.visibleTools
        const invisibleIndexes = Config.invisibleTools

        let visibleIndexes = [];
        for (let i = 0 ; i < visibleToolsIndexes.length ; i++) {
            const val = visibleToolsIndexes[i]
            if (val < allActionsLen) {
                visibleIndexes.push(val)
            }
            if (allActionsLen === visibleIndexes.length) {
                break;
            }
        }   

        let visibleActions = Array(visibleIndexes.length)

        let unknownActions = []
        let unknownIndexes = []
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
                unknownIndexes.push(i)
            }
        }

        const finalList = visibleActions.concat(unknownActions)
        
        let actionPosition = {}
        for (let i = 0 ; i < finalList.length ; i++) {
            const currentAction = finalList[i]
            actionPosition[currentAction.actionName] = i
        }
        toolbar.actionPositionMap = actionPosition

        if (finalList.length !== visibleIndexes.length) {
            Config.visibleTools = visibleIndexes.concat(unknownIndexes) 
        }

        return finalList
    }
}
