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

Kirigami.Page {
    id: root

    readonly property QtObject editorView: pageStack.get(0).editorView

    property bool cantLeave: false
    property bool isEraser: false
    property color penColor: mouseArea.lastButton === Qt.RightButton ? colorBar.secondaryColor : colorBar.primaryColor
    property bool wantSave
    property var cropRect

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
        },
        Kirigami.Action {
            id: penToggler

            checked: !root.isEraser
            checkable: true
            icon.name: "draw-brush-symbolic"
            
            onTriggered: {
                root.isEraser = false
            }
        },
        Kirigami.Action {
            id: eraserToggler

            checked: root.isEraser
            checkable: true
            icon.name: "draw-eraser-symbolic"
            
            onTriggered: {
                root.isEraser = true
            }
        }
    ]

    onBackRequested: (event) => {
        canvas.cropImage()
        
        root.cantLeave = root.cropRect.width && root.cropRect.height 
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

    contentItem: ColumnLayout {
        spacing: 0

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
                    property int strokeWidth: 5 * (root.isEraser ? 2 : 1)
                    property bool isInit: false

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

                        color: Qt.rgba(1, 0, 0, 0.75) 
                        radius: height / 2
                        visible: mouseArea.isPress && root.isEraser
                    }

                    MouseArea {
                        id: mouseArea

                        property var lastButton
                        property bool isPress: false

                        anchors.fill: parent

                        enabled: true
                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onPositionChanged: function (mouse) {
                            if (isPress) {
                                canvas.drawLine(canvas.lastX, canvas.lastY, mouseX, mouseY);
                                root.cantLeave = true
                            }
                            canvas.lastX = mouseX
                            canvas.lastY = mouseY
                        }
                        onReleased: function (mouse) {
                            if (mouse.button === lastButton) {
                                isPress = false
                            }
                        }
                        onPressed: function (mouse) {
                            canvas.lastX = mouseX
                            canvas.lastY = mouseY
                            if (!isPress) {
                                isPress = true
                                lastButton = mouse.button
                                canvas.drawPoint(mouseX, mouseY);
                                root.cantLeave = true
                            }
                        }
                    }

                    function drawLine(x1, y1, x2, y2) {
                        context.globalCompositeOperation = root.isEraser ? "destination-out" : "source-over"
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
                        context.globalCompositeOperation = root.isEraser ? "destination-out" : "source-over"
                        const radius = canvas.strokeWidth / 1.5 // Arbitrary value, looks not to big and not to small
                        context.fillStyle = root.penColor
                        context.beginPath();
                        context.moveTo(x, y);
                        context.arc(x, y, radius, 0, Math.PI * 2, false);
                        context.fill();
                        markDirty()
                    }

                    function clear() {
                        context.clearRect(0, 0, width, height);
                        markDirty()
                        root.cantLeave = false
                    }

                    // Slightly modified version of: https://stackoverflow.com/a/22267731
                    function cropImage() {
                        let pix = {x:[], y:[]}
                        let w = canvas.width
                        let h = canvas.height
                        let imageData = context.getImageData(0, 0, w, h)

                        let dataStartIndex, dataEndIndex;
                        for (let y = 0; y < h; y++) {
                            for (let x = 0; x < w; x++) {
                                // The data is stored in a single array
                                // The data for a single pixel takes 4 slots in this array, for: RGBA
                                // See: https://doc.qt.io/qt-6/qml-qtquick-canvasimagedata.html#data-prop
                                dataStartIndex = (y * w + x) * 4;
                                dataEndIndex = dataStartIndex + 3
                                if (imageData.data[dataEndIndex] > 100) { // Arbitrary value
                                    pix.x.push(x);
                                    pix.y.push(y);
                                } 
                            }
                        }
                        pix.x.sort(function(a,b){return a-b});
                        pix.y.sort(function(a,b){return a-b});
                        var n = pix.x.length-1;
                      
                        w = 1 + pix.x[n] - pix.x[0];
                        h = 1 + pix.y[n] - pix.y[0];

                        root.cropRect = Qt.rect(pix.x[0], pix.y[0], w, h)
                    }
                }
            }
        }

        ColorBar {
            id: colorBar

            Layout.fillWidth: true
        }
    }

    function showImagePicker(imagePath) {
        const imagePickerDialog = editorView.imagePickerDialog
        imagePickerDialog.open()
        if (autoCropAction.checked) {
            imagePickerDialog.paintClipRect = root.cropRect
        }
        imagePickerDialog.path = imagePath
        imagePickerDialog.paintedImageChoosen = true
        imagePickerDialog.storeCheckbox.checked = wantSave
        imagePickerDialog.storeCheckbox.enabled = false
    }

    function closePage(imagePath) {
        root.cantLeave = false
        canvas.clear()
        applicationWindow().switchToPage("Main")
        showImagePicker(imagePath)
        canvas.isInit = false
        root.cropRect = Qt.rect(0, 0, 0, 0)
    }

    function saveImage() {
        let filePath
        if (root.cantLeave) {
            let date = new Date().valueOf()
            const tmpFileName = "/KNtmpPaint"+date+".png"
            filePath = (StandardPaths.writableLocation(StandardPaths.TempLocation) + tmpFileName)

            canvas.save(filePath.substring(7)) // removes "file://"
        }
        root.wantSave = true
        closePage(filePath)
    }
}
