// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.Klever

import "qrc:/contents/ui/dialogs"

ColumnLayout {
    id: root

    required property string path
    readonly property TextEditor editor: editor
    readonly property TextDisplay display: display
    readonly property QtObject imagePickerDialog: toolbar.imagePickerDialog
    // Not perfect but easier then pulling those color from C++
    readonly property var defaultColors: [
        Kirigami.Theme.backgroundColor,
        Kirigami.Theme.textColor,
        Kirigami.Theme.disabledTextColor,
        Kirigami.Theme.linkColor,
        Kirigami.Theme.visitedLinkColor,
        Kirigami.Theme.alternateBackgroundColor,
        Kirigami.Theme.highlightColor,
    ]

    property list<Kirigami.Action> actions: [
        Kirigami.Action {
            id: linkedNotesAction

            enabled: Config.noteMapEnabled
            visible: enabled
            shortcut: "Ctrl+M"
            tooltip: i18nc("@tooltip, will be followed by the shortcut", "Linked notes") + " (" + shortcut + ")"
            icon.name: "gnumeric-link-internal-symbolic"
            
            onTriggered: {
                noteMapLoader.item.open()
            }
        },
        Kirigami.Action {
            id: pdfPrinter
            
            shortcut: "Ctrl+P"
            tooltip: i18nc("@tooltip, Print action, will be followed by the shortcut", "Print") + " (" + shortcut + ")"
            icon.name: "pdftex-symbolic"
            
            onTriggered: {
                printingDialog.open()
            }
        },
        Kirigami.Action {
            id: editorToggler
            
            shortcut: "Ctrl+Shift+8"
            tooltip: i18nc("@tooltip, will be followed by the shortcut", "View/Hide editor") + " (" + shortcut + ")"
            checked: Config.editorVisible
            checkable: true
            icon.name: editorToggler.checked ? "text-flow-into-frame-symbolic" : "text-unflow-symbolic"
            
            onTriggered: if (!editorToggler.checked && !viewToggler.checked) {
                editorToggler.checked = true
            }
            onCheckedChanged: if (Config.editorVisible !== editorToggler.checked) {
                Config.editorVisible = editorToggler.checked
            }
        },
        Kirigami.Action {
            id: viewToggler
            
            shortcut: "Ctrl+Shift+9"
            tooltip: i18nc("@tooltip, display as in 'the note preview', will be followed by the shortcut", "View/Hide preview") + " (" + shortcut + ")"
            checked: Config.previewVisible
            checkable: true
            icon.name: viewToggler.checked ? "quickview-symbolic" : "view-hidden-symbolic"
            
            onTriggered: if (!viewToggler.checked && !editorToggler.checked) {
                viewToggler.checked = true
            }
            onCheckedChanged: if (Config.previewVisible !== viewToggler.checked) {
                Config.previewVisible = viewToggler.checked
            }
        }
    ]

    spacing: 0
    
    onPathChanged: {
        focusEditor()
    }
    onDefaultColorsChanged: if (EditorHandler.notePath != "qrc:") {
        StyleHandler.setDefault(root.defaultColors)
    }
    Component.onCompleted: {
        focusEditor()
    }

    FileSaverDialog {
        id: htmlSaver

        caller: printingDialog
        title: i18nc("@title:dialog, choose the location of where the file will be saved", "Save note as html")
        nameFilters: [ "Html file (*.html)"]
    }

    PrintingDialog {
        id: printingDialog

        onPdf: {
            StyleHandler.inMain = false
            applicationWindow().switchToPage('Printing')
        }
        onHtml: {
            htmlSaver.open()
        }
        onPathChanged: if (path.length !== 0) {
            display.view.runJavaScript("document.documentElement.innerHTML", function (result) {
                DocumentHandler.writeFile(result, printingDialog.path.substring(7))
                printingDialog.close()
            })
        }
    }

    TextToolBar {
        id: toolbar

        notePath: root.path
        editorTextArea: root.editor.textArea
        visible: editor.visible
        Layout.margins: Kirigami.Units.largeSpacing
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
        GridLayout {
            id: generalLayout

            readonly property int totalSpacing: Kirigami.Units.smallSpacing * 2 
            readonly property bool isHorizontal: parent.width > Kirigami.Units.gridUnit * 30 

            flow: isHorizontal ? GridLayout.LeftToRight : GridLayout.TopToBottom
            rowSpacing: Kirigami.Units.smallSpacing
            columnSpacing: Kirigami.Units.smallSpacing

            anchors.fill: parent
            TextEditor {
                id: editor

                readonly property int divider: display.visible 
                    ? 2
                    : 1
                    
                path: root.path
                visible: editorToggler.checked // make sure that the textDisplay while correctly grow

                Layout.fillWidth: visible
                Layout.fillHeight: visible
                Layout.preferredWidth: generalLayout.isHorizontal
                    ? display.visible 
                        ? Math.round(generalLayout.width / 2) - generalLayout.totalSpacing
                        : generalLayout.width - generalLayout.totalSpacing
                    : root.width

                Layout.preferredHeight: generalLayout.isHorizontal
                    ? display.visible
                        ? Math.round(generalLayout.height / 2) - generalLayout.totalSpacing
                        : generalLayout.height - generalLayout.totalSpacing
                    : generalLayout.height

                LayoutMirroring.enabled: generalLayout.isHorizontal

                onOpenImageDialog: (imagePath) => {
                    toolbar.imagePickerDialog.path = "file://" + imagePath
                    toolbar.imagePickerDialog.clipboardImage = true
                    toolbar.imagePickerDialog.storeCheckbox.checked = true
                    toolbar.imagePickerDialog.storeCheckbox.enabled = false
                    toolbar.imagePickerDialog.open()
                }
            }

            Kirigami.Separator {
                visible: editor.visible && display.visible

                Layout.fillWidth: !generalLayout.isHorizontal
                Layout.fillHeight: generalLayout.isHorizontal
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            }

            TextDisplay {
                id: display

                readonly property int divider: editor.visible 
                    ? 2
                    : 1

                visible: viewToggler.checked // make sure that the textEditor while correctly grow
                
                Layout.fillWidth: visible
                Layout.fillHeight: visible
                Layout.preferredWidth: generalLayout.isHorizontal
                    ? editor.visible 
                        ? Math.round(generalLayout.width / 2) - generalLayout.totalSpacing
                        : generalLayout.width - generalLayout.totalSpacing
                    : root.width

                Layout.preferredHeight: generalLayout.isHorizontal
                    ? editor.visible
                        ? Math.round(generalLayout.height / 2) - generalLayout.totalSpacing
                        : generalLayout.height - generalLayout.totalSpacing
                    : generalLayout.height        
            }
        }
    }

    Loader {
        id: noteMapLoader

        sourceComponent: NotesMap {
            id: linkedNotesMap
        }
        active: Config.noteMapEnabled
    }

    Connections {
        id: editorHandlerConnections
        target: EditorHandler

        function onSurroundingDelimsChanged(delimsTypes) {
            toolbar.checkSourrindingDelimsActions(delimsTypes)
        }

        function onFocusEditor() {
            editor.textArea.forceActiveFocus()
        }
    }


    Connections {
        id: styleHandlerConnections

        target: StyleHandler
        function onStyleChanged(styles) {
            EditorHandler.changeStyles(styles)
        }
    }

    function focusEditor() {
        if (editor.visible) {
            editor.textArea.forceActiveFocus()
        }
    }
}
