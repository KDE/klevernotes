// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15

import org.kde.kirigami 2.19 as Kirigami

Item {
    readonly property list<QtObject> actions: [ 
        KAction {
            actionName: "𝐇"
            text: "𝐇"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Headers") + " (Ctrl+" + i18nc("@tooltip, short form of 'number'", "num") + ")"

            KAction {
                actionName: "𝐇𝟏"
                text: "𝐇𝟏"
                shortcut: "Ctrl+1"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 1") + " (" + shortcut + ")"
            }
            KAction {
                actionName: "𝐇𝟐"
                text: "𝐇𝟐"
                shortcut: "Ctrl+2"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 2") + " (" + shortcut + ")"
            }
            KAction {
                actionName: "𝐇𝟑"
                text: "𝐇𝟑"
                shortcut: "Ctrl+3"
            }
            KAction {
                actionName: "𝐇𝟒"
                text: "𝐇𝟒"
                shortcut: "Ctrl+4"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 4") + " (" + shortcut + ")"
            }
            KAction {
                actionName: "𝐇𝟓"
                text: "𝐇𝟓"
                shortcut: "Ctrl+5"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 5") + " (" + shortcut + ")"
            }
            KAction {
                actionName: "𝐇𝟔"
                text: "𝐇𝟔"
                shortcut: "Ctrl+6"
                tooltip: i18nc("@tooltip, text format header level, will be followed by the shortcut", "Header 6") + " (" + shortcut + ")"
            }
        },
        KAction {
            actionName: "bold"
            shortcut: "Ctrl+B"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Bold") + " (" + shortcut + ")"
            icon.name: "format-text-bold-symbolic"
        },
        KAction {
            actionName: "italic"
            shortcut: "Ctrl+I"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Italic") + " (" + shortcut + ")"
            icon.name: "format-text-italic-symbolic"
        },
        KAction {
            actionName: "strikethrough"
            shortcut: "Alt+Shift+S"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Strikethrough") + " (" + shortcut + ")"
            icon.name: "format-text-strikethrough-symbolic"
        },
        KAction {
            actionName: "codeBlock"
            shortcut: "Ctrl+Shift+K"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Code") + " (" + shortcut + ")"
            icon.name: "format-text-code-symbolic"
        },
        KAction {
            actionName: "quote"
            shortcut: "Ctrl+Shift+Q"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Quote") + " (" + shortcut + ")"
            icon.name: "format-text-blockquote-symbolic"
        },
        KAction {
            actionName: "image"
            shortcut: "Ctrl+Shift+I"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Image") + " (" + shortcut + ")"
            icon.name: "insert-image-symbolic"
        },
        KAction {
            actionName: "link"
            shortcut: "Ctrl+K"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Link") + " (" + shortcut + ")"
            icon.name: "insert-link-symbolic"
        },
        KAction {
            actionName: "table"
            shortcut: "Ctrl+T"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Table") + " (" + shortcut + ")"
            icon.name: "insert-table-symbolic"
        },
        KAction {
            actionName: "orderedList"
            shortcut: "Ctrl+Shift+O"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Ordered list") + " (" + shortcut + ")"
            icon.name: "format-list-ordered-symbolic"
        },
        KAction {
            actionName: "unorderedList"
            shortcut: "Ctrl+Shift+U"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Unordered list") + " (" + shortcut + ")"
            icon.name: "format-list-unordered-symbolic"
        },
        KAction {
            actionName: "highlight"
            shortcut: "Ctrl+Alt+H"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Text highlight") + " (" + shortcut + ")"
            icon.name: "draw-highlight-symbolic"
        },
        KAction {
            actionName: "emoji"
            shortcut: "Ctrl+Shift+E"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Unordered list") + " (" + shortcut + ")"
            icon.name: "smiley-symbolic"
        },
        KAction {
            actionName: "linkNote"
            shortcut: "Ctrl+Alt+K"
            tooltip: i18nc("@tooltip, text format, will be followed by the shortcut", "Link note") + " (" + shortcut + ")"
            icon.name: "edit-link-symbolic"
        }
    ]
}
