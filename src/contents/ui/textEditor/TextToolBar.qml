// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Dialogs 1.3

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"

Kirigami.Card {
    id: toolbarHolder

    required property TextArea editorTextArea
    required property string notePath

    readonly property QtObject imagePickerDialog: imagePickerDialog
    // This 'replicate' the DefaultCardBackground and just change the background color
    //(https://api.kde.org/frameworks/kirigami/html/DefaultCardBackground_8qml_source.html)
    background: Kirigami.ShadowedRectangle{
        Kirigami.Theme.colorSet: Kirigami.Theme.Header
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor
        radius: Kirigami.Units.smallSpacing
    }

    ImagePickerDialog {
        id: imagePickerDialog

        noteImagesStoringPath: toolbarHolder.notePath.replace("note.md","") + "Images/"

        onAccepted: if (imageLoaded) {
            let modifiedPath = path

            let useLocalImage = storedImageChoosen
            if (storeImage && !storedImageChoosen) {
                let wantedImageName = imageName

                if (wantedImageName === "" && paintedImageChoosen) {
                    wantedImageName = "painting"
                } else if (!paintedImageChoosen) {
                    const fileName = KleverUtility.getName(path)
                    wantedImageName = fileName.substring(0,fileName.lastIndexOf("."))
                }

                // We can't asign the result to modifiedPath and use it to saveToFile or it won't work !
                const validPath = KleverUtility.getImageStoragingPath(noteImagesStoringPath, wantedImageName)
                modifiedPath = validPath
                console.log(validPath, wantedImageName)

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

            let imageString = `![${imageName}](${modifiedPath}) `
            toolbarHolder.editorTextArea.insert(toolbarHolder.editorTextArea.cursorPosition, imageString)
            storedImageChoosen = false
        }

        onRejected: storedImageChoosen = false

    }

    TableMakerDialog {
        id: tableMakerDialog

        onAccepted: {
            const alignPattern = {"left":":------","center":":------:","right":"------:"}
            const cells = "|"+"       |".repeat(tableMakerDialog.columnCount)+"\n"
            const headers = "|"+(i18n("Header")+"|").repeat(tableMakerDialog.columnCount)+"\n"

            let columnsAlignments = "|"

            for(var childIdx = 0; childIdx < tableMakerDialog.listView.count ; childIdx++) {
                const columnAlignment = tableMakerDialog.listView.itemAtIndex(childIdx).children[0].data[0].checkedButton.align

                columnsAlignments = columnsAlignments.concat(alignPattern[columnAlignment],"|")
            }
            columnsAlignments +="\n"

            const result = "\n"+headers+columnsAlignments+cells.repeat(tableMakerDialog.rowCount-1)

            toolbarHolder.editorTextArea.insert(toolbarHolder.editorTextArea.cursorPosition, result)
        }
    }

    Kirigami.ActionToolBar {
        id: mainToolBar

        function applyInstructions(selectionEnd,info,givenSpecialChars,multiPlaceApply,applyIncrement){
            const instructions = info.instructions
            const lines = info.lines
            let end = selectionEnd
            let applied = false
            let specialChars = givenSpecialChars

            for (var i = lines.length-1 ; i >= 0; i--){
                const line = lines[i]
                const instruction = instructions[i]

                const start = end-line.length

                // Currently only used for ordered list
                if (applyIncrement) {
                    if (line.trim().length === 0) continue
                    specialChars = (i + 1).toString() + givenSpecialChars
                }

                switch(instruction) {
                case "apply":
                    if (multiPlaceApply) toolbarHolder.editorTextArea.insert(end,specialChars)
                    toolbarHolder.editorTextArea.insert(start,specialChars)

                    applied = true
                    break;
                case "remove":
                    if (multiPlaceApply) toolbarHolder.editorTextArea.remove(end-specialChars.length,end)
                    toolbarHolder.editorTextArea.remove(start,start+specialChars.length)
                    break;
                default:
                    break
                }
                end = start-1
            }
            if (applied) toolbarHolder.editorTextArea.select(toolbarHolder.editorTextArea.selectionStart-specialChars.length,toolbarHolder.editorTextArea.selectionEnd)
        }

        function handleAction(selectionStart,selectionEnd,specialChars,multiPlaceApply,applyIncrement) {
            const selectedText = editorTextArea.getText(selectionStart,selectionEnd)
            const info = MDHandler.getInstructions(selectedText,specialChars,multiPlaceApply,applyIncrement)

            const appliedSpecialChars = specialChars[0]
            mainToolBar.applyInstructions(selectionEnd,info,appliedSpecialChars,multiPlaceApply,applyIncrement)
        }

        function getLinesBlock(selectionStart,selectionEnd) {
            const startingText = editorTextArea.getText(0,editorTextArea.selectionStart)
            const endingText = editorTextArea.getText(editorTextArea.selectionEnd,editorTextArea.text.length)


            const startBlockIndex = startingText.lastIndexOf('\n')+1

            return [startBlockIndex,editorTextArea.selectionEnd]
        }

        actions: [
            Kirigami.Action {
                text: "𝐇"

                Kirigami.Action {
                    text: "𝐇𝟏"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(editorTextArea.selectionStart,editorTextArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd, ["# "], false, false)
                    }
                }
                Kirigami.Action {
                    text: "𝐇𝟐"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(editorTextArea.selectionStart,editorTextArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd, ["## "], false, false)
                    }
                }
                Kirigami.Action {
                    text: "𝐇𝟑"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(editorTextArea.selectionStart,editorTextArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd, ["### "], false, false)
                    }
                }
                Kirigami.Action {
                    text: "𝐇𝟒"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(editorTextArea.selectionStart,editorTextArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd, ["#### "], false, false)
                    }
                }
                Kirigami.Action {
                    text: "𝐇𝟓"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(editorTextArea.selectionStart,editorTextArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd, ["###### "], false, false)
                    }
                }
                Kirigami.Action {
                    text: "𝐇𝟔"
                    onTriggered: {
                        const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(editorTextArea.selectionStart,editorTextArea.selectionEnd);

                        mainToolBar.handleAction(selectionStart, selectionEnd, ["####### "], false, false)
                    }
                }

            },
            Kirigami.Action {
                id: boldAction
                icon.name: "format-text-bold"
                onTriggered: mainToolBar.handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["**","__"], true, false)
            },
            Kirigami.Action {
                id: italicAction
                icon.name: "format-text-italic"
                onTriggered: mainToolBar.handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["_","*"], true, false)
            },
            Kirigami.Action {
                id: strikethroughAction
                icon.name: "format-text-strikethrough"
                onTriggered: mainToolBar.handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["~~"], true, false)
            },
            Kirigami.Action {
                id: codeBlockAction
                icon.name: "format-text-code"
                onTriggered: mainToolBar.handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["\n```\n"], true, false)
            },
            Kirigami.Action {
                id: quoteAction
                icon.name: "format-text-blockquote"
                onTriggered: mainToolBar.handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["> "], false, false)
            },
            Kirigami.Action {
                id: imageAction
                icon.name: "insert-image"
                onTriggered: imagePickerDialog.open()
            },
            Kirigami.Action {
                id: tableAction
                icon.name: "insert-table"
                onTriggered: tableMakerDialog.open()
            },
            Kirigami.Action {
                id: orderedListAction
                icon.name: "format-ordered-list-symbolic"
                onTriggered: {
                    const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(editorTextArea.selectionStart,editorTextArea.selectionEnd);

                    mainToolBar.handleAction(selectionStart, selectionEnd, [". "], false, true)
                }
            },
            Kirigami.Action {
                id: unorderedListAction
                icon.name: "format-unordered-list-symbolic"
                onTriggered: {
                    const [selectionStart, selectionEnd] = mainToolBar.getLinesBlock(editorTextArea.selectionStart,editorTextArea.selectionEnd);

                    mainToolBar.handleAction(selectionStart, selectionEnd, ["- "], false, false)
                }
            }
        ]
    }
}
