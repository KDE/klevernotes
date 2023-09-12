// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.3 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import Qt.labs.platform 1.1

import org.kde.Klever 1.0

import "qrc:/contents/ui/dialogs"


Kirigami.Dialog {
    id: imagePickerDialog

    title: i18nc("@title:dialog", "Image selector")

    height: header.height + footer.height + topPadding + bottomPadding + mainItem.height

    property string noteImagesStoringPath

    property var paintClipRect
    property string path: ""
    property alias imageName: nameTextField.text
    property bool storedImageChoosen: false
    property bool paintedImageChoosen: false
    property bool storedImagesExist: !KleverUtility.isEmptyDir(noteImagesStoringPath)

    readonly property QtObject imageObject: displayImage
    readonly property bool imageLoaded: displayImage.visible
    readonly property bool storeImage: storeCheckbox.checked && !storedImageChoosen

    onPathChanged: displayImage.source = path

    FilePickerDialog {
        id: filePickerDialog

        caller: imagePickerDialog
    }

    URLDialog {
        id: urlDialog

        caller: imagePickerDialog
    }

    Column {
        id: mainItem

        padding: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing

        Layout.alignment: Qt.AlignTop

        Row {
            Controls.ToolButton {
                id: internetButton

                icon.name: "internet-amarok"
                icon.width: icon.height
                icon.height: imageHolder.visible
                    ? internetButton.height
                    : Kirigami.Units.iconSizes.large

                text: i18nc("@label:button, As in 'image from the internet'", "Web image")

                display: imageHolder.visible
                    ? Controls.AbstractButton.TextBesideIcon
                    : Controls.AbstractButton.TextUnderIcon


                width: Kirigami.Units.iconSizes.huge * 2
                height: imageHolder.visible
                    ? Kirigami.Units.iconSizes.medium
                    : width

                onClicked: {
                    imagePickerDialog.paintClipRect = undefined
                    clearTmp()
                    storedImageChoosen = false
                    urlDialog.open()
                }

            }


            Controls.ToolButton {
                id: localButton

                icon.name: "document-open-folder"
                icon.height: internetButton.icon.height
                icon.width: icon.height

                text: i18nc("label:button, Image from the local file system", "Local image")

                display: internetButton.display

                width: internetButton.width
                height: internetButton.height

                onClicked: {
                    imagePickerDialog.paintClipRect = undefined
                    clearTmp()
                    storedImageChoosen = false
                    filePickerDialog.folder = StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
                    filePickerDialog.open()
                }
            }
        }
        Row {
            Controls.ToolButton {
                id: paintingButton

                icon {
                    name: "oilpaint"
                    width: icon.height
                    height: internetButton.icon.height
                }

                text: i18nc("@label:button", "Paint an image")

                display: internetButton.display

                width: storageButton.visible ? internetButton.width : 2 * internetButton.width
                height: internetButton.height

                onClicked: {
                    imagePickerDialog.paintClipRect = undefined
                    clearTmp()
                    imagePickerDialog.close()
                    applicationWindow().switchToPage('Painting')
                }
            }
            Controls.ToolButton {
                id: storageButton

                icon.name: "dblatex"
                icon.width: icon.height
                icon.height: internetButton.icon.height

                text: i18nc("@label:button, Collection of image stored inside the note folder", "Image collection")

                display: internetButton.display

                visible: imagePickerDialog.storedImagesExist

                width: internetButton.width
                height: internetButton.height

                onClicked: {
                    imagePickerDialog.paintClipRect = undefined
                    clearTmp()
                    storedImageChoosen = true
                    filePickerDialog.folder = "file://"+imagePickerDialog.noteImagesStoringPath
                    filePickerDialog.open()
                }
            }
        }

        Kirigami.Separator{
            visible: imageHolder.visible
            width: internetButton.width * 2
            anchors.horizontalCenter: parent.horizontalCenter
        }


        Item {
            id: imageHolder

            visible: path != ""
            height: Kirigami.Units.iconSizes.huge * 3
            width: internetButton.width * 2
            Image {
                id: displayImage

                property int idealWidth
                property int idealHeight

                source: path
                visible: displayImage.status == Image.Ready
                fillMode: Image.PreserveAspectFit
                sourceClipRect: imagePickerDialog.paintClipRect

                anchors.horizontalCenter: parent.horizontalCenter

                onStatusChanged: if (status == Image.Ready){
                    // If the image is placed inside the note folder, we want it to be max 1024x1024
                    if (Math.max(implicitWidth,implicitHeight,1024) == 1024) {
                        idealWidth = implicitWidth
                        idealHeight = implicitHeight
                    } else {
                        let divider = (implicitHeight > implicitWidth)
                            ? implicitHeight/1024
                            : implicitWidth/1024

                        idealWidth = Math.round(implicitWidth/divider)
                        idealHeight = Math.round(implicitHeight/divider)
                    }
                    height = Kirigami.Units.iconSizes.huge * 3
                }
            }

            Text {
                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false
                color: Kirigami.Theme.textColor

                text: i18n("It appears that the image you selected does not exist or is not supported.")
                wrapMode: Text.Wrap
                visible: !displayImage.visible

                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Kirigami.Separator{
            width: internetButton.width * 2
            anchors.horizontalCenter: parent.horizontalCenter
        }

        RowLayout {
            width: internetButton.width * 2
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: Kirigami.Units.smallSpacing
            Controls.Label {
                text: i18nc("@label:textbox, text associated to the selected image", "Image text: ")
            }

            Controls.TextField {
                id: nameTextField

                focus: true
                Layout.fillWidth: true
            }

            Layout.margins: Kirigami.Units.largeSpacing
        }

        Controls.CheckBox {
            id: storeCheckbox

            checked: true
            text: i18nc("@label:checkbox", "Place this image inside the note folder")
            width: internetButton.width * 2
            visible: displayImage.visible && !storedImageChoosen && !paintedImageChoosen

            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onClosed: {
        clearTmp()
        path = ""
        imageName = ""
        displayImage.height = undefined
    }

    function clearTmp() {
        if (paintedImageChoosen && (path !== "")) {
            KleverUtility.remove(KleverUtility.getPath(imagePickerDialog.path))
            paintedImageChoosen = false
        }
    }
}

