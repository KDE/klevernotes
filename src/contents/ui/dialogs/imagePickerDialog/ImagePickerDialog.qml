// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.3 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import Qt.labs.platform 1.1
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

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

    // GridLayout was causing to much problem :/
    ColumnLayout {
        id: mainItem

        property int iconSize: imageHolder.visible
            ? Kirigami.Units.iconSizes.small
            : Kirigami.Units.iconSizes.large

        width: Kirigami.Units.iconSizes.huge * 4
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0
            ButtonDelegate {
                id: internetButton

                icon {
                    name: "internet-amarok"
                    width: mainItem.iconSize
                    height: mainItem.iconSize
                }

                padding: Kirigami.Units.largeSpacing

                text: i18nc("@label:button, As in 'image from the internet'", "Web image")

                display: imageHolder.visible
                    ? Controls.AbstractButton.TextBesideIcon
                    : Controls.AbstractButton.TextUnderIcon

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

                icon {
                    name: "document-open-folder"
                    width: mainItem.iconSize
                    height: mainItem.iconSize
                }

                text: i18nc("label:button, Image from the local file system", "Local image")

                display: internetButton.display

                Layout.preferredWidth: Kirigami.Units.iconSizes.huge * 2
                Layout.preferredHeight: internetButton.height

                onClicked: {
                    imagePickerDialog.paintClipRect = undefined
                    clearTmp()
                    storedImageChoosen = false
                    filePickerDialog.folder = StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
                    filePickerDialog.open()
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0
            ButtonDelegate {
                id: paintingButton

                icon {
                    name: "draw-brush"
                    width: mainItem.iconSize
                    height: mainItem.iconSize
                }

                text: i18nc("@label:button", "Paint an image")

                display: internetButton.display

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

                icon {
                    name: "dblatex"
                    width: mainItem.iconSize
                    height: mainItem.iconSize
                }

                text: i18nc("@label:button, Collection of image stored inside the note folder", "Image collection")

                display: internetButton.display

                visible: imagePickerDialog.storedImagesExist

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
            Layout.fillWidth: true
            visible: imageHolder.visible
        }

        Item {
            id: imageHolder

            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.iconSizes.huge * 3 + Kirigami.Units.largeSpacing
            Layout.margins: Kirigami.Units.smallSpacing

            visible: imagePickerDialog.path !== ""

            Image {
                id: displayImage

                property int idealWidth
                property int idealHeight

                source: path
                visible: displayImage.status == Image.Ready
                fillMode: Image.PreserveAspectFit
                sourceClipRect: imagePickerDialog.paintClipRect

                anchors.centerIn: parent

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
                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false
                color: Kirigami.Theme.textColor

                text: i18n("It appears that the image you selected does not exist or is not supported.")
                wrapMode: Text.WordWrap
                visible: !displayImage.visible

                anchors.fill: parent
                anchors.centerIn: parent
                padding: Kirigami.Units.largeSpacing
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            visible: imageHolder.visible
        }

        FormCard.FormTextFieldDelegate {
            id: nameTextField

            Layout.fillWidth: true

            label: i18nc("@label:textbox, text associated to the selected image", "Image text: ")
            maximumLength: 40
        }

        FormCard.FormCheckDelegate {
            id: storeCheckbox

            Layout.fillWidth: true

            text: i18nc("@label:checkbox", "Place this image inside the note folder")
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

