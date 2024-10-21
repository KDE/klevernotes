// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import Qt.labs.platform

import org.kde.kirigami as Kirigami

import "qrc:/contents/ui/dialogs"
import "qrc:/contents/ui/painting"

import org.kde.Klever
import WashiPadComponents

Kirigami.Page {
    id: root

    readonly property QtObject editorView: pageStack.get(0).editorView
    readonly property size size: Qt.size(sketchContent.width, sketchContent.height)
    readonly property point cursorPos: Qt.point(handler.point.x, handler.point.y)
    readonly property var penType: Stroke.Fill

    property bool cantLeave: false
    property bool isEraser: false
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
                root.clearCanvas()
            }
        },
        Kirigami.Action {
            id: autoCropAction

            text: i18nc("@label:action", "Auto crop")
            checked: true
            checkable: true
            icon.name: "image-crop-symbolic"
        },
        Kirigami.Action {
            id: penToggler

            checked: !root.isEraser
            checkable: true
            icon.name: "draw-brush-symbolic"
            
            onTriggered: {
                handler.changePointer(0)
            }
        },
        Kirigami.Action {
            id: eraserToggler

            checked: root.isEraser
            checkable: true
            icon.name: "draw-eraser-symbolic"
            
            onTriggered: {
                handler.changePointer(1)
            }
        }
    ]

    onBackRequested: (event) => {
        if (root.cantLeave) {
            event.accepted = true;
            leavingDialog.open();
            return
        }
        closePage("", {})
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
            root.closePage("", {})
        }
    }

    contentItem: ColumnLayout {
        spacing: 0

        Controls.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Flickable {
                id: flickable

                contentWidth: sketchContent.width + Kirigami.Units.largeSpacing * 2
                contentHeight: sketchContent.height + Kirigami.Units.largeSpacing * 2

                clip: true
                interactive: !mouseArea.isPress

                Rectangle {
                    id: sketchContent

                    x: Math.max(Kirigami.Units.largeSpacing, (root.width - width) / 2)
                    y: Kirigami.Units.largeSpacing

                    width: 1024
                    height: 1024

                    StrokeListItem {
                        id: fillStrokes

                        z: 0
                        anchors.fill: parent

                        type: Stroke.Fill
                        model: sketchModel
                    }

                    StrokeItem {
                        id: currentStroke

                        z: Stroke.type === Stroke.Outline ? 1 : 0
                        anchors.fill: parent
                    }
                }

                MouseArea {
                    id: mouseArea

                    property var lastButton
                    property bool isPress: false

                    anchors.fill: sketchContent

                    enabled: true
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onPositionChanged: function (mouse) {
                        if (isPress) {
                            handler.mouseMoved(mouse.x-flickable.contentX, mouse.y-flickable.contentY)
                        }
                    }
                    onReleased: function (mouse) {
                        if (mouse.button === lastButton) {
                            isPress = false

                            handler.changeMousePress(isPress)
                        }
                    }
                    onPressed: function (mouse) {
                        if (!isPress) {
                            isPress = true
                            lastButton = mouse.button

                            handler.changeMousePress(isPress)
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

    Rectangle {
        id: cursor

        x: handler.point.x - width / 2 + sketchContent.x
        y: handler.point.y - height / 2  + sketchContent.y
        width: height
        height: pressureEquation.width * 1.5

        color: "transparent"
        border {
            color: root.isEraser ? Qt.rgba(1, 0, 0, 0.75) : Qt.rgba(0, 0, 1, 0.75)
            width: 10
        }
        radius: height / 2
        visible: mouseArea.isPress
    }

    SketchViewHandler {
        id: handler

        onPointChanged: {
            if (!pressed) return

            if (!root.isEraser){
                addSample()
            } else {
                eraseSamples()
            }
        }
        onIsEraserChanged: function (sketchViewIsEraser) {
            root.isEraser = sketchViewIsEraser
        }
        onPressedChanged: function (pressed) {
            if (root.isEraser) return

            if (!pressed && !isEmpty(currentStroke.stroke.boundingRect())) {
                addSample()
                fillStrokes.addStroke(currentStroke.stroke)
                currentStroke.stroke = createStroke(penType, root.penColor)
                return
            }

            currentStroke.stroke = createStroke(penType, root.penColor)
        }

        function isEmpty(rect) {
            return rect.left === rect.right
                && rect.top === rect.bottom
        }

        function createPoint() {
            return Qt.vector2d(point.x + flickable.contentX,
                        point.y + flickable.contentY)
        }

        function addSample() {
            if (!root.cantLeave) root.cantLeave = true
            var sample = createSample(createPoint(), pressureEquation.width)
            currentStroke.addSample(sample)
        }

        function eraseSamples() {
            var point = createPoint()
            var radius = cursor.height / 2

            fillStrokes.eraseArea(point, radius)
        }
    }

    PressureEquation {
        id: pressureEquation

        readonly property real minFillWidth: 4
        readonly property real maxFillWidth: 20
        readonly property real minEraserWidth: 8
        readonly property real maxEraserWidth: 80

        pressure: handler.point.pressure
        minWidth: root.isEraser ? minEraserWidth : minFillWidth
        maxWidth: root.isEraser ? maxEraserWidth : maxFillWidth
    }

    SketchModel {
        id: sketchModel
    }

    SketchSerializer {
        id: serializer
    }

    function clearCanvas() {
        const point = Qt.vector2d(512, 512)
        fillStrokes.eraseArea(point, 1024)
        root.cantLeave = false
    }

    function showImagePicker(imagePath, cropRect) {
        const imagePickerDialog = editorView.imagePickerDialog
        imagePickerDialog.open()
        imagePickerDialog.paintClipRect = cropRect
        imagePickerDialog.path = imagePath
        imagePickerDialog.paintedImageChoosen = true
        imagePickerDialog.storeCheckbox.checked = wantSave
        imagePickerDialog.storeCheckbox.enabled = false
    }

    function closePage(imagePath, cropRect, paintedImageChoosen) {
        root.cantLeave = false
        clearCanvas()
        applicationWindow().switchToPage("Main")
        showImagePicker(imagePath, cropRect)
    }

    function saveImage() {
        let filePath
        let cropRect
        if (root.cantLeave) {
            let date = new Date().valueOf()
            const tmpFileName = "/KNtmpPaint"+date+".png"

            filePath = StandardPaths.writableLocation(StandardPaths.TempLocation) + tmpFileName
            serializer.serialize(sketchModel, Qt.size(1024, 1024), filePath)

            if (autoCropAction.checked) cropRect = serializer.getCropRect()
        }
        root.wantSave = true
        closePage(filePath, cropRect)
    }
}
