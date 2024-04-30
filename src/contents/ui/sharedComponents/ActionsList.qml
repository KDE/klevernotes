// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick

import org.kde.kirigami as Kirigami

import org.kde.Klever

Item {
    readonly property list<QtObject> actions: [ 
        KAction {
            actionName: "𝐇"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Headers")
            text: "𝐇"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Headers") + " (Ctrl+" + i18nc("@tooltip, short form of 'number'", "num") + ")"

            KAction {
                actionName: "h1"
                description: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 1")
                text: "𝐇𝟏"
                shortcut: "Ctrl+1"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 1") + " (" + shortcut + ")"
            }
            KAction {
                actionName: "h2"
                description: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 2")
                text: "𝐇𝟐"
                shortcut: "Ctrl+2"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 2") + " (" + shortcut + ")"
            }
            KAction {
                actionName: "h3"
                description: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 3")
                text: "𝐇𝟑"
                shortcut: "Ctrl+3"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 3") + " (" + shortcut + ")"
            }
            KAction {
                actionName: "h4"
                description: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 4")
                text: "𝐇𝟒"
                shortcut: "Ctrl+4"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 4") + " (" + shortcut + ")"
            }
            KAction {
                actionName: "h5"
                description: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 5")
                text: "𝐇𝟓"
                shortcut: "Ctrl+5"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 5") + " (" + shortcut + ")"
            }
            KAction {
                actionName: "h6"
                description: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 6")
                text: "𝐇𝟔"
                shortcut: "Ctrl+6"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 6") + " (" + shortcut + ")"
            }
        },
        KAction {
            actionName: "bold"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Bold")
            shortcut: "Ctrl+B"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Bold") + " (" + shortcut + ")"
            icon.name: "format-text-bold-symbolic"
        },
        KAction {
            actionName: "italic"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Italic")
            shortcut: "Ctrl+I"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Italic") + " (" + shortcut + ")"
            icon.name: "format-text-italic-symbolic"
        },
        KAction {
            actionName: "strikethrough"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Strikethrough")
            shortcut: "Alt+Shift+S"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Strikethrough") + " (" + shortcut + ")"
            icon.name: "format-text-strikethrough-symbolic"
        },
        KAction {
            actionName: "codeBlock"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Code")
            shortcut: "Ctrl+Shift+K"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Code") + " (" + shortcut + ")"
            icon.name: "format-text-code-symbolic"
        },
        KAction {
            actionName: "quote"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Quote")
            shortcut: "Ctrl+Shift+Q"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Quote") + " (" + shortcut + ")"
            icon.name: "format-text-blockquote-symbolic"
        },
        KAction {
            actionName: "image"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Image")
            shortcut: "Ctrl+Shift+I"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Image") + " (" + shortcut + ")"
            icon.name: "insert-image-symbolic"
        },
        KAction {
            actionName: "link"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Link")
            shortcut: "Ctrl+K"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Link") + " (" + shortcut + ")"
            icon.name: "insert-link-symbolic"
        },
        KAction {
            actionName: "table"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Table")
            shortcut: "Ctrl+T"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Table") + " (" + shortcut + ")"
            icon.name: "insert-table-symbolic"
        },
        KAction {
            actionName: "orderedList"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Ordered List")
            shortcut: "Ctrl+Shift+O"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Ordered list") + " (" + shortcut + ")"
            icon.name: "format-list-ordered-symbolic"
        },
        KAction {
            actionName: "unorderedList"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Unordered List")
            shortcut: "Ctrl+Shift+U"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Unordered list") + " (" + shortcut + ")"
            icon.name: "format-list-unordered-symbolic"
        },
        KAction {
            actionName: "highlight"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Highlight")
            shortcut: "Ctrl+Alt+H"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Text highlight") + " (" + shortcut + ")"
            icon.name: "draw-highlight-symbolic"
        },
        KAction {
            actionName: "emoji"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Emoji")
            shortcut: "Ctrl+Shift+E"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Emoji") + " (" + shortcut + ")"
            icon.name: "smiley-symbolic"
        },
        KAction {
            actionName: "linkNote"
			description: i18nc("@tooltip, text format, will be followed by the shortcut", "Link Note")
            shortcut: "Ctrl+Alt+K"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Link note") + " (" + shortcut + ")"
            icon.name: "edit-link-symbolic"
            visible: Config.noteMapEnabled
            enabled: visible
        }
    ]
}
