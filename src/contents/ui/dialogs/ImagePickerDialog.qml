// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.3 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.Dialog {
    id: imagePickerDialog
    title: i18n("Image selector")

    height: header.height + footer.height + topPadding + bottomPadding + mainItem.height

    property bool localImageChoosen: false
    property string path
    onPathChanged: {
        displayImage.visible = true

        internetButton.height = Kirigami.Units.iconSizes.medium
        internetButton.display = Controls.AbstractButton.TextBesideIcon
        internetButton.icon.height = internetButton.height
        localButton.icon.height = internetButton.height

        displayImage.source = path
    }

    function getImagePath() {
        let component = (localImageChoosen)
            ? Qt.createComponent("qrc:/contents/ui/dialogs/FilePickerDialog.qml")
            : Qt.createComponent("qrc:/contents/ui/dialogs/URLDialog.qml")

        if (component.status == Component.Ready) {
            var dialog = component.createObject(imagePickerDialog);
            dialog.par = imagePickerDialog
            dialog.open()
        }
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
                icon.height: Kirigami.Units.iconSizes.huge * 2

                text: i18n("Web image")

                display: Controls.AbstractButton.TextUnderIcon
                width: Kirigami.Units.iconSizes.huge * 3
                height: width

                onClicked: {localImageChoosen = false ; getImagePath()}
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

                onClicked: {localImageChoosen = true ; getImagePath()}
            }
        }

        Kirigami.Separator{
            visible: displayImage.visible
            width: internetButton.width * 2
            anchors.horizontalCenter: parent.horizontalCenter
        }


        Image {
            id: displayImage

            fillMode: Image.PreserveAspectFit
            visible: false

            sourceSize.height: Kirigami.Units.iconSizes.huge * 3
            anchors.horizontalCenter: parent.horizontalCenter
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
            checked: true
            text: i18n("Place this image inside the note folder")
            width: internetButton.width * 2
            visible: displayImage.visible && localImageChoosen

            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
}

