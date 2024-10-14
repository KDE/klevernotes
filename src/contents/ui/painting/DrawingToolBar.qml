// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

Components.FloatingToolBar {
    id: root

    enum Tool {
        Pen,
        Eraser,
        Text,
        Rectangle,
        Circle
    }

    property int selectedTool: DrawingToolBar.Tool.Pen
   
    ActionGroup {
        id: mainToolsGroup 

        Kirigami.Action {
            id: penToggler

            checked: true
            checkable: true
            icon.name: "draw-brush-symbolic"
            
            onTriggered: {
                root.selectedTool = DrawingToolBar.Tool.Pen
            }
        }

        Kirigami.Action {
            id: eraserToggler

            checkable: true
            icon.name: "draw-eraser-symbolic"
            
            onTriggered: {
                root.selectedTool = DrawingToolBar.Tool.Eraser
            }
        }

        Kirigami.Action {
            id: textToggler

            checkable: true
            icon.name: "text-field-symbolic"
            
            onTriggered: {
                root.selectedTool = DrawingToolBar.Tool.Text
            }
        }

        Kirigami.Action {
            id: rectToggler

            checkable: true
            icon.name: "draw-rectangle-symbolic"
            
            onTriggered: {
                root.selectedTool = DrawingToolBar.Tool.Rectangle
            }
        }

        Kirigami.Action {
            id: circleToggler

            checkable: true
            icon.name: "draw-circle-symbolic"
            
            onTriggered: {
                root.selectedTool = DrawingToolBar.Tool.Circle
            }
        }
    }

    contentItem: ColumnLayout {
        ToolButton {
            action: penToggler
        }

        ToolButton {
            action: eraserToggler
        }

        ToolButton {
            action: textToggler
        }

        ToolButton {
            action: rectToggler
        }

        ToolButton {
            action: circleToggler
        }
    }
}
