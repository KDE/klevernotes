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

    title: i18n("Image selector")

    height: header.height + footer.height + topPadding + bottomPadding + mainItem.height

    property string noteImagesStoringPath

    property bool storedImagesExist: !KleverUtility.isEmptyDir(noteImagesStoringPath)
    property string path: ""
    property alias imageName: nameTextField.text
    property bool storedImageChoosen: false

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
            id: buttonHolder

            property int visibleChildrenCount: (storageButton.visible) ? 3 : 2

            spacing: Kirigami.Units.largeSpacing

            Controls.ToolButton {
                id: internetButton

                icon.name: "internet-amarok"
                icon.width: icon.height
                icon.height: imageHolder.visible
                    ? internetButton.height
                    : Kirigami.Units.iconSizes.large * (5-buttonHolder.visibleChildrenCount)

                text: i18n("Web image")

                display: imageHolder.visible
                    ? Controls.AbstractButton.TextBesideIcon
                    : Controls.AbstractButton.TextUnderIcon

                width: Kirigami.Units.iconSizes.huge * (5-buttonHolder.visibleChildrenCount)

                height: imageHolder.visible
                    ? Kirigami.Units.iconSizes.medium
                    : width

                onClicked: {
                    storedImageChoosen = false
                    urlDialog.open()
                }
            }

            Kirigami.Separator{
                height: internetButton.height
            }

            Controls.ToolButton {
                id: localButton

                icon.name: "document-open-folder"
                icon.height: internetButton.icon.height
                icon.width: icon.height

                text: i18n("Local image")

                display: internetButton.display
                width: internetButton.width
                height: internetButton.height

                onClicked: {
                    storedImageChoosen = false
                    filePickerDialog.folder = StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
                    filePickerDialog.open()
                }
            }

            Kirigami.Separator{
                height: internetButton.height
                visible: imagePickerDialog.storedImagesExist
            }

            Controls.ToolButton {
                id: storageButton

                icon.name: "dblatex"
                icon.width: icon.height
                icon.height: internetButton.icon.height

                text: i18n("Stored image")

                display: internetButton.display
                width: internetButton.width
                height: internetButton.height

                visible: imagePickerDialog.storedImagesExist

                onClicked: {
                    storedImageChoosen = true
                    filePickerDialog.folder = "file://"+imagePickerDialog.noteImagesStoringPath
                    filePickerDialog.open()
                }
            }
        }

        Kirigami.Separator{
            visible: imageHolder.visible
            width: internetButton.width * buttonHolder.visibleChildrenCount
            anchors.horizontalCenter: parent.horizontalCenter
        }


        Item {
            id: imageHolder

            visible: path != ""
            height: Kirigami.Units.iconSizes.huge * 3
            width: internetButton.width * buttonHolder.visibleChildrenCount
            Image {
                id: displayImage

                source: path
                fillMode: Image.PreserveAspectFit
                visible: displayImage.status == Image.Ready

                property int idealWidth
                property int idealHeight

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

                text: i18n("It seems that the image you select doesn't exist or is not supported.")
                wrapMode: Text.Wrap
                visible: !displayImage.visible

                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Kirigami.Separator{
            width: internetButton.width * buttonHolder.visibleChildrenCount
            anchors.horizontalCenter: parent.horizontalCenter
        }

        RowLayout {
            width: internetButton.width * buttonHolder.visibleChildrenCount
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: Kirigami.Units.smallSpacing
            Controls.Label {
                text: i18n("Image text: ")
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
            text: i18n("Place this image inside the note folder")
            width: internetButton.width * buttonHolder.visibleChildrenCount
            visible: displayImage.visible && !storedImageChoosen

            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onOpened: {path = "" ; imageName = ""; displayImage.height = undefined ;}
}

