// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import Qt.labs.platform

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

import "qrc:/contents/ui/dialogs"


FormCard.FormCardDialog {
    id: imagePickerDialog

    readonly property Image imageObject: displayImage
    readonly property FormCard.FormCheckDelegate storeCheckbox: storeCheckbox
    readonly property bool imageLoaded: displayImage.visible
    readonly property bool storeImage: storeCheckbox.checked && !storedImageChoosen

    property string noteImagesStoringPath
    property var paintClipRect
    property string path: ""
    property alias imageName: nameTextField.text
    property bool storedImageChoosen: false
    property bool paintedImageChoosen: false
    property bool clipboardImage: false
    property bool storedImagesExist: !KleverUtility.isEmptyDir(noteImagesStoringPath)

    title: i18nc("@title:dialog", "Image selector")

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onPathChanged: {
        displayImage.source = path
    }
    onClosed: {
        clearTmp()
        path = ""
        imageName = ""
        displayImage.height = undefined
    }
    onRejected: {
        close()
    }

    FilePickerDialog {
        id: filePickerDialog

        caller: imagePickerDialog
    }

    URLDialog {
        id: urlDialog

        caller: imagePickerDialog
    }

    // GridLayout was causing to much problem :/
    contentItem: ColumnLayout {
        id: mainItem

        property int iconSize: imageHolder.visible
            ? Kirigami.Units.iconSizes.small
            : Kirigami.Units.iconSizes.large

        width: Kirigami.Units.iconSizes.huge * 4
        spacing: 0

        RowLayout {
            spacing: 0
            
            Layout.fillWidth: true
            Layout.fillHeight: true

            ButtonDelegate {
                id: internetButton

                text: i18nc("@label:button, As in 'image from the internet'", "Web image")
                padding: Kirigami.Units.largeSpacing

                display: imageHolder.visible
                    ? Controls.AbstractButton.TextBesideIcon
                    : Controls.AbstractButton.TextUnderIcon

                icon {
                    name: "internet-amarok"
                    width: mainItem.iconSize
                    height: mainItem.iconSize
                }

                Layout.preferredWidth: Kirigami.Units.iconSizes.huge * 2
                Layout.preferredHeight: imageHolder.visible
                    ? Kirigami.Units.iconSizes.small * 3
                    : width

                onClicked: {
                    imagePickerDialog.paintClipRect = undefined
                    clearTmp()
                    storedImageChoosen = false
                    urlDialog.open()
                }
            }

            ButtonDelegate {
                id: localButton

                text: i18nc("label:button, Image from the local file system", "Local image")
                display: internetButton.display

                icon {
                    name: "document-open-folder"
                    width: mainItem.iconSize
                    height: mainItem.iconSize
                }

                Layout.preferredWidth: Kirigami.Units.iconSizes.huge * 2
                Layout.preferredHeight: internetButton.height

                onClicked: {
                    imagePickerDialog.paintClipRect = undefined
                    clearTmp()
                    storedImageChoosen = false
                    filePickerDialog.currentFolder = StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
                    filePickerDialog.open()
                }
            }
        }

        RowLayout {
            spacing: 0

            Layout.fillWidth: true
            Layout.fillHeight: true

            ButtonDelegate {
                id: paintingButton

                text: i18nc("@label:button", "Paint an image")
                display: internetButton.display

                icon {
                    name: "draw-brush"
                    width: mainItem.iconSize
                    height: mainItem.iconSize
                }

                Layout.preferredWidth: storageButton.visible ? Kirigami.Units.iconSizes.huge * 2 : Kirigami.Units.iconSizes.huge * 4
                Layout.preferredHeight: internetButton.height

                onClicked: {
                    imagePickerDialog.paintClipRect = undefined
                    clearTmp()
                    imagePickerDialog.close()
                    applicationWindow().switchToPage('Painting')
                }
            }

            ButtonDelegate {
                id: storageButton

                text: i18nc("@label:button, Collection of image stored inside the note folder", "Image collection")
                display: internetButton.display
                visible: imagePickerDialog.storedImagesExist

                icon {
                    name: "dblatex"
                    width: mainItem.iconSize
                    height: mainItem.iconSize
                }

                Layout.preferredWidth: visible ? Kirigami.Units.iconSizes.huge * 2 : 0
                Layout.preferredHeight: visible ? internetButton.height : 0

                onClicked: {
                    imagePickerDialog.paintClipRect = undefined
                    clearTmp()
                    storedImageChoosen = true
                    filePickerDialog.folder = "file://"+imagePickerDialog.noteImagesStoringPath
                    filePickerDialog.open()
                }
            }
        }

        Kirigami.Separator {
            visible: imageHolder.visible
            Layout.fillWidth: true
        }

        Item {
            id: imageHolder

            visible: imagePickerDialog.path !== ""

            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.iconSizes.huge * 3 + Kirigami.Units.largeSpacing
            Layout.margins: Kirigami.Units.smallSpacing

            Image {
                id: displayImage

                property int idealWidth
                property int idealHeight

                anchors.centerIn: parent

                source: path
                visible: displayImage.status == Image.Ready
                fillMode: Image.PreserveAspectFit
                sourceClipRect: imagePickerDialog.paintClipRect

                onStatusChanged: if (status == Image.Ready){
                    // If the image is placed inside the note folder, we want it to be max 1024x1024
                    if (Math.max(implicitWidth, implicitHeight, 1024) == 1024) {
                        idealWidth = implicitWidth
                        idealHeight = implicitHeight
                    } else {
                        let divider = (implicitHeight > implicitWidth)
                            ? implicitHeight / 1024
                            : implicitWidth / 1024

                        idealWidth = Math.round(implicitWidth / divider)
                        idealHeight = Math.round(implicitHeight / divider)
                    }
                    height = Kirigami.Units.iconSizes.huge * 3
                }
            }

            Text {
                padding: Kirigami.Units.largeSpacing
                anchors.fill: parent
                anchors.centerIn: parent

                text: i18n("It appears that the image you selected does not exist or is not supported.")
                wrapMode: Text.WordWrap
                visible: !displayImage.visible

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false

                color: Kirigami.Theme.textColor
            }
        }

        Kirigami.Separator {
            visible: imageHolder.visible
            Layout.fillWidth: true
        }

        FormCard.FormTextFieldDelegate {
            id: nameTextField

            label: i18nc("@label:textbox, text associated to the selected image", "Image text: ")
            maximumLength: 40

            Layout.fillWidth: true
        }

        FormCard.FormCheckDelegate {
            id: storeCheckbox

            text: i18nc("@label:checkbox", "Place this image inside the note folder")

            Layout.fillWidth: true
        }
    }

    function clearTmp() {
        if (paintedImageChoosen || clipboardImage) {
            paintedImageChoosen = false
            clipboardImage = false
            storeCheckbox.enabled = true
            storeCheckbox.checked = false
        }
        if (clipboardImage) {
            KleverUtility.remove(path.substring(7)) // Clear the temp image
        }
    }
}
