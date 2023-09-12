// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami

Kirigami.Dialog {
    id: scrollableDialog

    readonly property QtObject listView: listView
    readonly property int rowCount: rowSpin.value
    readonly property int columnCount: columnSpin.value

    title: i18nc("@title:dialog, table => an html table", "Table creation")

    width: Kirigami.Units.gridUnit * 18

    Column {
        width: parent.width
        spacing: Kirigami.Units.largeSpacing
        RowLayout {
            width: parent.width
            height: 50
            Text {
                text: i18nc("Name, 'a row'", "Row")
                horizontalAlignment: Text.AlignHCenter
                color: Kirigami.Theme.textColor

                Layout.alignment: Qt.AlignHCenter
                Layout.leftMargin: Kirigami.Units.largeSpacing
            }
            Controls.SpinBox{
                id: rowSpin

                from: 1

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: i18n("Column")
                horizontalAlignment: Text.AlignHCenter
                color: Kirigami.Theme.textColor

                Layout.alignment: Qt.AlignHCenter
            }
            Controls.SpinBox{
                id: columnSpin

                from: 1

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.rightMargin: Kirigami.Units.largeSpacing
            }

            Layout.margins: Kirigami.Units.largeSpacing
        }

        Item {
            implicitWidth: Kirigami.Units.gridUnit * 15
            implicitHeight: 30
            anchors.horizontalCenter: parent.horizontalCenter
            RowLayout {
                anchors.fill: parent

                Item {
                    height: 20
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 1.5
                    Layout.margins: 0
                }

                Kirigami.Icon {
                    source: "format-justify-left"

                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 0
                }
                Kirigami.Icon {
                    source: "format-justify-center"

                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 0
                }
                Kirigami.Icon {
                    source: "format-justify-right"

                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 0
                }
            }
        }


        Item {
            implicitWidth: Kirigami.Units.gridUnit * 18
            implicitHeight: Kirigami.Units.gridUnit * 6
            anchors.horizontalCenter: parent.horizontalCenter
            ListView {
                id: listView

                implicitWidth: Kirigami.Units.gridUnit * 15
                implicitHeight: Kirigami.Units.gridUnit * 4

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                Controls.ScrollBar.vertical: Controls.ScrollBar {
                    active: true
                }


                model: columnSpin.value
                delegate: Item {
                    height: 30
                    width: listView.width

                    RowLayout {
                        anchors.fill: parent
                        spacing: 0

                        Controls.ButtonGroup {
                            id: radioGroup
                        }

                        Text {
                            text: modelData+1
                            Layout.preferredWidth: Kirigami.Units.gridUnit * 1
                            color: Kirigami.Theme.textColor
                        }

                        Controls.RadioButton {
                            id: control

                            property string align: "left"

                            indicator.x: control.width/2

                            checked: checkAllLeft.checked
                            Controls.ButtonGroup.group: radioGroup

                            Layout.fillWidth: true
                            Layout.margins: 0

                            background: Rectangle {
                                color: "transparent"
                            }
                        }

                        Controls.RadioButton {
                            property string align: "center"

                            indicator.x: control.width/2

                            checked: checkAllCenter.checked
                            Controls.ButtonGroup.group: radioGroup

                            Layout.fillWidth: true
                            Layout.margins: 0


                            background: Rectangle {
                                color: "transparent"
                            }
                        }

                        Controls.RadioButton {
                            property string align: "right"

                            indicator.x: control.width/2

                            checked: checkAllRight.checked
                            Controls.ButtonGroup.group: radioGroup

                            Layout.fillWidth: true
                            Layout.margins: 0

                            background: Rectangle {
                                color: "transparent"
                            }
                        }
                    }
                }
            }
        }

        Item {
            implicitWidth: Kirigami.Units.gridUnit * 15
            implicitHeight: 30
            anchors.horizontalCenter: parent.horizontalCenter
            Controls.ButtonGroup { id: alignGroup }
            RowLayout {
                anchors.fill: parent

                Text {
                    text: i18n("Align all:")

                    color: Kirigami.Theme.textColor

                    Layout.preferredWidth: Kirigami.Units.gridUnit * 1
                    Layout.margins: 0
                }

                Controls.RadioButton {
                    id: checkAllLeft

                    indicator.x: checkAllLeft.width/2.1

                    checked: true

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 0
                }

                Controls.RadioButton {
                    id: checkAllCenter

                    indicator.x: checkAllCenter.width/2.1

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 0
                }

                Controls.RadioButton {
                    id: checkAllRight

                    indicator.x: checkAllRight.width/2.1

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 0
                }
            }
        }
    }

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onOpened: {
        columnSpin.value = 1
        rowSpin.value = 1
        checkAllLeft.checked = true
    }
}
