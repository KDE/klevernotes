// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import Qt.labs.platform
import QtQuick.Shapes

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

import org.kde.klevernotes

Kirigami.Page {
    id: root

    readonly property QtObject editorView: pageStack.get(0).editorView

    property bool cantLeave: false
    property color penColor: mouseArea.lastButton === Qt.RightButton ? colorBar.secondaryColor : colorBar.primaryColor
    property bool wantSave

    title: i18nc("@title:page", "Paint!")

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    actions: [
        Kirigami.Action {
            text: i18nc("@label:button", "Save")
            icon.name: "document-save-symbolic"

            onTriggered: {
                root.saveImage()
            }
        },
        Kirigami.Action {
            text: i18nc("@label:button, as in 'erase everything'", "Clear")
            icon.name: "edit-clear-symbolic"

            onTriggered: {
                canvas.clear()
            }
        },
        Kirigami.Action {
            id: autoCropAction

            text: i18nc("@label:action", "Auto crop")
            checked: true
            checkable: true
            icon.name: "image-crop-symbolic"
        }
    ]

    onBackRequested: (event) => {
        if (root.cantLeave) {
            event.accepted = true;
            leavingDialog.open();
            return
        }
        root.wantSave = false
        closePage("")
    }

    LeavePaintingDialog {
        id: leavingDialog

        onAccepted: {
            root.saveImage()
            leavingDialog.close()
        }
        onDiscarded: {
            leavingDialog.close()
            root.wantSave = false
            root.closePage("")
        }
    }

    TextDialog {
        id: textDialog

        title: i18nc("@title:dialog", "Text to draw")

        onAccepted: {
            canvas.drawText(textDialog.text, canvas.lastX, canvas.lastY)
            root.cantLeave = true
        }
        onClosed: {
            clear()
        }
    }

    contentItem: ColumnLayout {
        spacing: 0

        DrawingToolBarOptions {
            id: drawingToolBarOptions

            primaryColor: colorBar.primaryColor
            secondaryColor: colorBar.secondaryColor
            toolMode: drawingToolBar.mode

            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 4 + Kirigami.Units.smallSpacing
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            DrawingToolBar {
                id: drawingToolBar

                readonly property bool isBaseMode: (mode === "draw" || mode === "erase")
                readonly property bool isShape: (mode === "rectangle" || mode === "circle")

                Layout.preferredHeight: Kirigami.Units.gridUnit * 12
                Layout.margins: Kirigami.Units.largeSpacing
            }

            Controls.ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Flickable {
                    id: flickable

                    contentWidth: canvasBorder.width + Kirigami.Units.largeSpacing * 4
                    contentHeight: canvasBorder.height + Kirigami.Units.largeSpacing * 4
                    anchors.fill: parent

                    clip: true
                    interactive: !mouseArea.isPress

                    Rectangle {
                        id: canvasBorder
                        width: canvas.width + 8
                        height: canvas.width + 8
                        x: Math.max(Kirigami.Units.largeSpacing * 2, (root.width - width) / 2)
                        y: (Kirigami.Units.largeSpacing * 2) - 4

                        Kirigami.Theme.inherit: false
                        Kirigami.Theme.colorSet: Kirigami.Theme.View
                        color: Kirigami.Theme.backgroundColor
                        radius: 5
                        border {
                            color: Kirigami.Theme.highlightColor
                            width: 4
                        }
                    }

                    Canvas {
                        id: canvas

                        width: 1024
                        height: 1024
                        x: Math.max(Kirigami.Units.largeSpacing * 2, (root.width - width) / 2)
                        y: Kirigami.Units.largeSpacing * 2

                        property real lastX
                        property real lastY
                        property real startX: -1
                        property real startY: -1
                        property bool shiftHeld : false
                        property int strokeWidth: drawingToolBarOptions.lineWidth * (drawingToolBar.mode === "erase" ? 2 : 1)
                        property bool isInit: false
                        readonly property bool shapeIsStarted: canvas.startX !== -1 && canvas.startY !== -1


                        onPaint: if (!isInit) {
                            getContext("2d")
                            isInit = true
                        }

                        // Make the eraser a bit easier to use
                        Rectangle {
                            id: eraserCursor

                            x: canvas.lastX - width / 2
                            y: canvas.lastY - height / 2
                            width: height
                            height: canvas.strokeWidth

                            border {
                                width: 5
                                color: Qt.rgba(1, 0, 0, 0.75)
                            }
                            radius: height / 2
                            visible: mouseArea.isPress && drawingToolBar.mode === "erase"
                        }

                        Rectangle {
                            id: shapeBox

                            x: Math.min(canvas.startX, canvas.lastX)
                            y: canvas.yManagerForBoxes()
                            width: Math.abs(canvas.startX - canvas.lastX)
                            height: canvas.heightManagerForBoxes()

                            color: Kirigami.Theme.highlightColor 
                            opacity: 0.3
                            border {
                                width: 2
                                color: Kirigami.Theme.textColor
                            }

                            visible: canvas.shapeIsStarted && drawingToolBar.isShape && drawingToolBar.mode === "rectangle"
                        }


                        // an ELLIPSE preview shown when using the circle tool in ellipse mode (shift key NOT HELD)
                        Shape {
                            id : ellipseBox

                            width : shapeBox.width
                            height : shapeBox.height

                            readonly property real arcStartX : (canvas.startX + canvas.lastX - width)/2
                            readonly property real arcStartY : (canvas.startY + canvas.lastY)/2
                            readonly property real arcEndX : (canvas.startX + canvas.lastX + width)/2
                            readonly property real arcEndY : (canvas.startY + canvas.lastY)/2

                            opacity: 0.3

                            ShapePath{
                                fillColor: Kirigami.Theme.highlightColor
                                strokeColor: Kirigami.Theme.textColor
                                startX: ellipseBox.arcStartX
                                startY : ellipseBox.arcStartY

                                PathArc {
                                    direction: PathArc.Clockwise
                                    radiusX: ellipseBox.width/2
                                    radiusY : ellipseBox.height/2
                                    x : ellipseBox.arcEndX
                                    y: ellipseBox.arcEndY
                                }
                            }


                            ShapePath{
                                fillColor: Kirigami.Theme.highlightColor
                                strokeColor: Kirigami.Theme.textColor
                                startX: ellipseBox.arcStartX
                                startY : ellipseBox.arcStartY

                                PathArc {
                                    direction: PathArc.Counterclockwise
                                    radiusX: ellipseBox.width/2
                                    radiusY : ellipseBox.height/2
                                    x : ellipseBox.arcEndX
                                    y: ellipseBox.arcEndY
                                }
                            }

                            visible: canvas.shapeIsStarted && drawingToolBar.isShape && drawingToolBar.mode === "circle" && !canvas.shiftHeld
                        }


                        // a CIRCLE preview shown when using the circle tool in circle mode (shift key HELD)
                        Rectangle {
                            x: Math.min(canvas.startX, canvas.lastX)
                            y: canvas.yManagerForBoxes()
                            width: Math.abs(canvas.startX - canvas.lastX)
                            height: canvas.heightManagerForBoxes()

                            color: Kirigami.Theme.highlightColor
                            opacity: 0.3
                            border {
                                width: 2
                                color: Kirigami.Theme.textColor
                            }
                            radius: width

                            visible: canvas.shapeIsStarted && drawingToolBar.isShape && drawingToolBar.mode === "circle" && canvas.shiftHeld
                        }


                        MouseArea {
                            id: mouseArea

                            property var lastButton
                            property bool isPress: false
                            anchors.fill: canvas

                            enabled: true
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            hoverEnabled: true

                            onPositionChanged: function (mouse) {
                                if (isPress) {
                                    if (drawingToolBar.isBaseMode) {
                                        canvas.drawLine(canvas.lastX, canvas.lastY, mouseX, mouseY);
                                        root.cantLeave = true
                                    }
                                    if (mouse.modifiers & Qt.ShiftModifier) {
                                        canvas.shiftHeld = true
                                    }
                                    else {
                                        canvas.shiftHeld = false
                                    }

                                    canvas.lastX = getX()
                                    canvas.lastY = getY()
                                }
                            }
                            onReleased: function (mouse) {
                                if (mouse.button === lastButton && !drawingToolBar.isShape) {
                                    isPress = false
                                }
                            }
                            onPressed: function (mouse) {
                                if (!isPress) {
                                    isPress = true
                                    canvas.lastX = getX()
                                    canvas.lastY = getY()
                                    lastButton = mouse.button

                                    switch(drawingToolBar.mode) {
                                    case "draw":
                                    case "erase": {
                                        canvas.drawPoint(canvas.lastX, canvas.lastY)
                                        root.cantLeave = true
                                        break
                                    }
                                    case "text": {
                                        isPress = false
                                        textDialog.open()
                                        break
                                    }
                                    case "rectangle":
                                    case "circle": {
                                        canvas.startX = canvas.lastX
                                        canvas.startY = canvas.lastY
                                        break
                                    }
                                    }
                                } else if (drawingToolBar.isShape) {
                                    isPress = false
                                    if (mouse.button === lastButton) {
                                        if (drawingToolBar.mode === "rectangle") {
                                            canvas.drawRect(shapeBox.x, shapeBox.y, shapeBox.width, shapeBox.height)
                                        } else {
                                            canvas.drawCircle(shapeBox.x, shapeBox.y, shapeBox.width, shapeBox.height)
                                        }
                                    }
                                    cantLeave = true
                                    canvas.startX = -1
                                    canvas.startY = -1
                                }
                            }

                            function getX() {
                                if (mouseX < 0) return 0
                                if (1024 < mouseX) return 1024
                                return mouseX
                            }

                            function getY() {
                                if (mouseY < 0) return 0
                                if (1024 < mouseY) return 1024
                                return mouseY
                            }
                        }

                        function drawLine(x1, y1, x2, y2) {
                            context.globalCompositeOperation = drawingToolBar.mode === "erase" ? "destination-out" : "source-over"
                            context.strokeStyle = root.penColor
                            context.lineWidth = canvas.strokeWidth
                            context.lineCap = "round"
                            context.beginPath();
                            context.moveTo(x1, y1);
                            context.lineTo(x2, y2);
                            context.stroke();
                            markDirty()
                        }

                        function drawPoint(x, y) {
                            context.globalCompositeOperation = drawingToolBar.mode === "erase" ? "destination-out" : "source-over"
                            const radius = canvas.strokeWidth / 1.5 // Arbitrary value, looks not to big and not to small
                            context.fillStyle = root.penColor
                            context.beginPath();
                            context.moveTo(x, y);
                            context.arc(x, y, radius, 0, Math.PI * 2, false);
                            context.fill();
                            markDirty()
                        }

                        function drawText(text, x, y) {
                            const bold = drawingToolBarOptions.isBold ? "bold" : ""
                            const italic = drawingToolBarOptions.isItalic ? "italic" : ""
                            const fontInfo = drawingToolBarOptions.fontPointSize + "pt " + drawingToolBarOptions.fontFamily
                            context.font = `${bold} ${italic} ${fontInfo}`;

                            if (drawingToolBarOptions.shapeStyle !== "fill") {
                                context.lineWidth = canvas.strokeWidth
                                context.strokeStyle = root.penColor
                                context.strokeText(text, x, y);
                            }
                            if (drawingToolBarOptions.shapeStyle !== "outline") {
                                context.fillStyle = drawingToolBarOptions.fillColor;
                                context.fillText(text, x, y);
                            }
                            markDirty()
                        }

                        function drawRect(x, y, w, h) {
                            if (drawingToolBarOptions.shapeStyle !== "fill") {
                                context.lineWidth = canvas.strokeWidth
                                context.strokeStyle = root.penColor
                                context.strokeRect(x, y, w, h)
                            }
                            if (drawingToolBarOptions.shapeStyle !== "outline") {
                                context.fillStyle = drawingToolBarOptions.fillColor;
                                context.fillRect(x, y, w, h)
                            }
                            markDirty()
                        }

                        function drawCircle(x, y, w, h) {
                            context.beginPath();
                            context.ellipse(x, y, w, h)
                            if (drawingToolBarOptions.shapeStyle !== "fill") {
                                context.lineWidth = canvas.strokeWidth
                                context.strokeStyle = root.penColor
                                context.stroke();
                            }
                            if (drawingToolBarOptions.shapeStyle !== "outline") {
                                context.fillStyle = drawingToolBarOptions.fillColor;
                                context.fill()
                            }
                            markDirty()
                        }

                        function clear() {
                            context.clearRect(0, 0, width, height);
                            markDirty()
                            root.cantLeave = false
                        }

                        function yManagerForBoxes() {
                            if (shiftHeld){
                                if (lastY >= startY)
                                    return startY;
                                return startY - Math.abs(startX - lastX);
                            }
                            return Math.min(startY, lastY);
                        }

                        function heightManagerForBoxes() {
                            return shiftHeld
                                ? Math.abs(canvas.startX - canvas.lastX)
                                : Math.abs(startY - lastY)
                        }
                    }
                }
            }
        }

        ColorBar {
            id: colorBar

            Layout.fillWidth: true
        }
    }

    function prepareImagePicker(imagePath) {
        const imagePickerDialog = editorView.imagePickerDialog

        imagePickerDialog.path = imagePath
        imagePickerDialog.paintedImageChoosen = true
        imagePickerDialog.storeCheckbox.checked = wantSave
        imagePickerDialog.storeCheckbox.enabled = false
    }

    function closePage(imagePath) {
        root.cantLeave = false
        canvas.clear()
        prepareImagePicker(imagePath)
        canvas.isInit = false

        applicationWindow().switchToPage("Main", editorView.imagePickerDialog)
    }

    function saveImage() {
        if (root.cantLeave) {
            let date = new Date().valueOf()
            const tmpFileName = "/KNtmpPaint" + date + ".png"

            let filePath = (StandardPaths.writableLocation(StandardPaths.TempLocation) + tmpFileName)
            imageSaverConnection.savedPath = filePath

            filePath = filePath.substring("file://".length)
            ImageSaver.saveImage(canvas, filePath, autoCropAction.checked)
        }
        root.wantSave = true
    }

    Connections {
        id: imageSaverConnection

        target: ImageSaver

        property string savedPath: ""

        function onSaved(success) {
            if (success && savedPath) closePage(savedPath)
            savedPath = ""
        }
    }
}
