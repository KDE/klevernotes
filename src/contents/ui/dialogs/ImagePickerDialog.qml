// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.3 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

import "qrc:/contents/ui/dialogs"


Kirigami.Dialog {
    id: imagePickerDialog
    title: i18n("Image selector")

    height: header.height + footer.height + topPadding + bottomPadding + mainItem.height

    property bool localImageChoosen: false
    property string path
    property alias imageName: nameTextField.text
    readonly property bool imageLoaded: displayImage.visible
    readonly property bool storeImage: storeCheckbox.checked
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
            spacing: Kirigami.Units.largeSpacing

            Controls.ToolButton {
                id: internetButton

                icon.name: "internet-amarok"
                icon.width: icon.height
                icon.height: imageHolder.visible
                    ? internetButton.height
                    : Kirigami.Units.iconSizes.huge * 2

                text: i18n("Web image")

                display: imageHolder.visible
                    ? Controls.AbstractButton.TextBesideIcon
                    : Controls.AbstractButton.TextUnderIcon
                width: Kirigami.Units.iconSizes.huge * 3
                height: imageHolder.visible
                    ? Kirigami.Units.iconSizes.medium
                    : width

                onClicked: {localImageChoosen = false ; urlDialog.open()}
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

                onClicked: {localImageChoosen = true ; filePickerDialog.open()}
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

                source: path
                fillMode: Image.PreserveAspectFit
                visible: displayImage.status == Image.Ready

                sourceSize.height: Kirigami.Units.iconSizes.huge * 3
                anchors.horizontalCenter: parent.horizontalCenter
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
            width: internetButton.width * 2
            anchors.horizontalCenter: parent.horizontalCenter
        }

        RowLayout {
            width: internetButton.width * 2
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
            width: internetButton.width * 2
            visible: displayImage.visible && localImageChoosen

            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onClosed: {path = "" ; imageName = ""}
}

