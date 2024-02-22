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

Kirigami.ActionToolBar {
    id: toolbar

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

    actions: [
        Kirigami.Action {
            text: "𝐇"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Headers") + " (Ctrl+" + i18nc("@tooltip, short form of 'number'", "num") + ")"
            enabled: toolbar.visible

            Kirigami.Action {
                text: "𝐇𝟏"
                shortcut: "Ctrl+1"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 1") + " (" + shortcut + ")"
                enabled: toolbar.visible

                onTriggered: {
                    const [blockStart, blockEnd] = MDHandler.getBlockLimits(editorTextArea.selectionStart, editorTextArea.selectionEnd, editorTextArea.text);

                    handleAction(blockStart, blockEnd, ["# "], false, false, false)
                }
            }
            Kirigami.Action {
                text: "𝐇𝟐"
                shortcut: "Ctrl+2"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 2") + " (" + shortcut + ")"
                enabled: toolbar.visible

                onTriggered: {
                    const [blockStart, blockEnd] = MDHandler.getBlockLimits(editorTextArea.selectionStart, editorTextArea.selectionEnd, editorTextArea.text);

                    handleAction(blockStart, blockEnd, ["## "], false, false, false)
                }
            }
            Kirigami.Action {
                text: "𝐇𝟑"
                shortcut: "Ctrl+3"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 3") + " (" + shortcut + ")"
                enabled: toolbar.visible

                onTriggered: {
                    const [blockStart, blockEnd] = MDHandler.getBlockLimits(editorTextArea.selectionStart, editorTextArea.selectionEnd, editorTextArea.text);

                    handleAction(blockStart, blockEnd, ["### "], false, false, false)
                }
            }
            Kirigami.Action {
                text: "𝐇𝟒"
                shortcut: "Ctrl+4"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 4") + " (" + shortcut + ")"
                enabled: toolbar.visible

                onTriggered: {
                    const [blockStart, blockEnd] = MDHandler.getBlockLimits(editorTextArea.selectionStart, editorTextArea.selectionEnd, editorTextArea.text);

                    handleAction(blockStart, blockEnd, ["#### "], false, false, false)
                }
            }
            Kirigami.Action {
                text: "𝐇𝟓"
                shortcut: "Ctrl+5"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 5") + " (" + shortcut + ")"
                enabled: toolbar.visible

                onTriggered: {
                    const [blockStart, blockEnd] = MDHandler.getBlockLimits(editorTextArea.selectionStart, editorTextArea.selectionEnd, editorTextArea.text);

                    handleAction(blockStart, blockEnd, ["###### "], false, false, false)
                }
            }
            Kirigami.Action {
                text: "𝐇𝟔"
                shortcut: "Ctrl+6"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 6") + " (" + shortcut + ")"
                enabled: toolbar.visible

                onTriggered: {
                    const [blockStart, blockEnd] = MDHandler.getBlockLimits(editorTextArea.selectionStart, editorTextArea.selectionEnd, editorTextArea.text);

                    handleAction(blockStart, blockEnd, ["####### "], false, false, false)
                }
            }

        },
        Kirigami.Action {
            id: boldAction
            shortcut: "Ctrl+B"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Bold") + " (" + shortcut + ")"
            icon.name: "format-text-bold"
            enabled: toolbar.visible

            onTriggered: handleAction(editorTextArea.selectionStart,
                                                  editorTextArea.selectionEnd, ["**","__"],
                                                  true, false, false)
        },
        Kirigami.Action {
            id: italicAction
            shortcut: "Ctrl+I"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Italic") + " (" + shortcut + ")"
            icon.name: "format-text-italic"
            enabled: toolbar.visible

            onTriggered: handleAction(editorTextArea.selectionStart,
                                                  editorTextArea.selectionEnd, ["_","*"],
                                                  true, false, false)
        },
        Kirigami.Action {
            id: strikethroughAction
            shortcut: "Alt+Shift+S"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Strikethrough") + " (" + shortcut + ")"
            icon.name: "format-text-strikethrough"
            enabled: toolbar.visible

            onTriggered: handleAction(editorTextArea.selectionStart,
                                                  editorTextArea.selectionEnd, ["~~"],
                                                  true, false, false)
        },
        Kirigami.Action {
            id: codeBlockAction
            shortcut: "Ctrl+Shift+K"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Code") + " (" + shortcut + ")"
            icon.name: "format-text-code"
            enabled: toolbar.visible

            onTriggered: handleAction(editorTextArea.selectionStart,
                                                  editorTextArea.selectionEnd, ["\n```\n"],
                                                  true, false, true)
        },
        Kirigami.Action {
            id: quoteAction
            shortcut: "Ctrl+Shift+Q"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Quote") + " (" + shortcut + ")"
            icon.name: "format-text-blockquote"
            enabled: toolbar.visible

            onTriggered: handleAction(editorTextArea.selectionStart,
                                                  editorTextArea.selectionEnd, ["> "],
                                                  false, false, false)
        },
        Kirigami.Action {
            id: imageAction
            shortcut: "Ctrl+Shift+I"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Image") + " (" + shortcut + ")"
            icon.name: "insert-image"
            enabled: toolbar.visible

            onTriggered: imagePickerDialog.open()
        },
        Kirigami.Action {
            id: linkAction
            shortcut: "Ctrl+K"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Link") + " (" + shortcut + ")"
            icon.name: "insert-link-symbolic"
            enabled: toolbar.visible

            onTriggered: linkDialog.open()
        },
        Kirigami.Action {
            id: tableAction
            shortcut: "Ctrl+T"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Table") + " (" + shortcut + ")"
            icon.name: "insert-table"
            enabled: toolbar.visible

            onTriggered: tableMakerDialog.open()
        },
        Kirigami.Action {
            id: orderedListAction
            shortcut: "Ctrl+Shift+O"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Ordered list") + " (" + shortcut + ")"
            icon.name: "format-list-ordered"
            enabled: toolbar.visible

            onTriggered: {
                const [blockStart, blockEnd] = MDHandler.getBlockLimits(editorTextArea.selectionStart, editorTextArea.selectionEnd, editorTextArea.text);

                handleAction(blockStart, blockEnd, [". "], false, true, false)
            }
        },
        Kirigami.Action {
            id: unorderedListAction
            shortcut: "Ctrl+Shift+U"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Unordered list") + " (" + shortcut + ")"
            icon.name: "format-list-unordered"
            enabled: toolbar.visible

            onTriggered: {
                const [blockStart, blockEnd] = MDHandler.getBlockLimits(editorTextArea.selectionStart, editorTextArea.selectionEnd, editorTextArea.text);

                handleAction(blockStart, blockEnd, ["- "], false, false, false)
            }
        },
        Kirigami.Action {
            id: highlightAction
            shortcut: "Ctrl+Alt+H"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Text highlight") + " (" + shortcut + ")"
            icon.name: "draw-highlight"
            enabled: toolbar.visible

            onTriggered: {
                handleAction(editorTextArea.selectionStart, editorTextArea.selectionEnd, ["=="], true, false, false)
            }
        },
        Kirigami.Action {
            id: emojiAction
            shortcut: "Ctrl+Shift+E"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Unordered list") + " (" + shortcut + ")"
            icon.name: "smiley"
            enabled: toolbar.visible

            onTriggered: {
                emojiDialog.open()
            }
        },
        Kirigami.Action {
            id: linkNoteAction
            visible: Config.noteMapEnabled
            shortcut: "Ctrl+Alt+K"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Link note") + " (" + shortcut + ")"
            icon.name: "edit-link"
            enabled: toolbar.visible

            onTriggered: {
                linkNoteDialog.open()
            }
        }
    ]

    EmojiDialog {
        id: emojiDialog

        onChosen: function (emoji) {
            editorTextArea.insert(editorTextArea.selectionStart, Config.quickEmojiEnabled && Config.quickEmojiDialogEnabled ? (":" + emoji + ":") : emoji)
        }
    }

    ImagePickerDialog {
        id: imagePickerDialog

        noteImagesStoringPath: toolbar.notePath.replace("note.md","") + "Images/"

        onRejected: {
            storedImageChoosen = false
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
}
