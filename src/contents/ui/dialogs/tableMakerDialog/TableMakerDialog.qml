// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Templates 2.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

Kirigami.Dialog {
    id: scrollableDialog

    property int rowCount: 1
    property int columnCount: 1
    property string alignment: optionsColumn.isExpended ? alignmentBox.currentValue : "left"
    property int hoveredRow: 0
    property int hoveredColumn: 0
    property bool isHovered: false

    title: i18nc("@title:dialog, table => an html table", "Table creation")

    width: Kirigami.Units.gridUnit * 15
    padding: 0

    onRowCountChanged: if (rowCount !== rowSpin.value) {
        rowSpin.value = rowCount
    }
    onColumnCountChanged: if (columnCount !== columnSpin.value) {
        columnSpin.value = columnCount
    }

    ColumnLayout {
        id: itemLayout

        property int generalTiming: Kirigami.Units.longDuration
        
        height: Kirigami.Units.gridUnit * 21
        spacing: 0

        Controls.Label {
            text: i18nc("@label, display the final size of the table %1 is the number of row and %2 the number of column","Size: %1 x %2", rowCount, columnCount)

            Layout.alignment: Qt.AlignTop
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.leftMargin: Kirigami.Units.gridUnit
            Layout.rightMargin: Kirigami.Units.gridUnit
        }

        GridLayout {
            id: mainHolder

            rows: 10
            columns: 5
            rowSpacing: 0
            columnSpacing: 0

            Layout.preferredHeight: Kirigami.Units.gridUnit * 15
            Layout.alignment: Qt.AlignTop || Qt.AlignHCenter
            Layout.margins: 0
            Layout.leftMargin: Kirigami.Units.largeSpacing * 3
            Layout.rightMargin: Kirigami.Units.largeSpacing * 3

            states: State {
                name: "invisible"
                when: optionsColumn.isExpended

                PropertyChanges { target: mainHolder; Layout.preferredHeight: 0 }
                PropertyChanges { target: mainHolder; opacity: 0 }
            }

            Behavior on Layout.preferredHeight {
                NumberAnimation { duration: itemLayout.generalTiming }
            }
            Behavior on opacity {
                NumberAnimation { duration: itemLayout.generalTiming - 20 }
            }

            Repeater {
                model: mainHolder.rows * mainHolder.columns

                delegate: ItemDelegate {
                    id: box

                    readonly property int visualColumn: (index % mainHolder.columns) + 1
                    readonly property int visualRow: ((index - visualColumn + 1) / mainHolder.columns) + 1

                    property bool insideSelectedBound: visualRow <= scrollableDialog.rowCount && visualColumn <= scrollableDialog.columnCount

                    property bool insideHoveredBound: scrollableDialog.isHovered
                        ? (visualRow <= scrollableDialog.hoveredRow && visualColumn <= scrollableDialog.hoveredColumn)
                        : false


                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    focusPolicy: Qt.StrongFocus
                    hoverEnabled: true

                    background: Rectangle {
                        color: {
                            let colorOpacity = 0.7;
                            let selectedColor = Kirigami.Theme.textColor

                            if (box.pressed ||
                                box.insideSelectedBound && box.insideHoveredBound ||
                                !scrollableDialog.isHovered && box.insideSelectedBound
                            ) {
                                selectedColor = Kirigami.Theme.highlightColor
                                colorOpacity = 0.7;
                            } else if (box.insideHoveredBound) {
                                selectedColor = Kirigami.Theme.highlightColor
                                colorOpacity = 0.5;
                            }

                            return Qt.rgba(selectedColor.r, selectedColor.g, selectedColor.b, colorOpacity)
                        }

                        border.width: 1
                        border.color: Kirigami.Theme.backgroundColor

                        Behavior on color {
                            ColorAnimation { duration: Kirigami.Units.shortDuration }
                        }
                    }

                    onHoveredChanged: {
                        scrollableDialog.isHovered = hovered
                        if (hovered) {
                            scrollableDialog.hoveredRow = visualRow
                            scrollableDialog.hoveredColumn = visualColumn
                        } else {
                            scrollableDialog.hoveredRow = 0
                            scrollableDialog.hoveredColumn = 0
                        }
                    }
                    onClicked: {
                        scrollableDialog.rowCount = visualRow
                        scrollableDialog.columnCount = visualColumn
                    }
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.smallSpacing
        }

        UpDownButtonDelegate {
            id: optionButton

            text: i18nc("@label:button", "More options")
            arrowDirection: optionsColumn.isExpended ? Qt.DownArrow : Qt.UpArrow

            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.alignment: optionsColumn.isExpended ? Qt.AlignTop : Qt.AlignBottom
            Layout.bottomMargin: optionsColumn.isExpended ? 0 : -Kirigami.Units.smallSpacing


            onClicked: {
                optionsColumn.isExpended = !optionsColumn.isExpended

                if (scrollableDialog.rowCount > mainHolder.rows) scrollableDialog.rowCount = mainHolder.rows
                if (scrollableDialog.columnCount > mainHolder.columns) scrollableDialog.columnCount = mainHolder.columns
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.smallSpacing
            visible: optionsColumn.isExpended
        }

        Item {
            id: optionsColumn

            property bool isExpended: false

            Layout.fillWidth: true
            Layout.preferredHeight: 0
            Layout.alignment: Qt.AlignBottom
            opacity: 0

            states: State {
                name: "visible"
                when: optionsColumn.isExpended
                PropertyChanges { target: optionsColumn; Layout.preferredHeight: Kirigami.Units.gridUnit * 15 }
                PropertyChanges { target: optionsColumn; opacity: 1 }
            }

            Behavior on Layout.preferredHeight {
                NumberAnimation { duration: itemLayout.generalTiming }
            }
            Behavior on opacity {
                NumberAnimation { duration: itemLayout.generalTiming - 20}
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: Kirigami.Units.smallSpacing

                FormCard.FormSpinBoxDelegate {
                    id: rowSpin

                    label: i18nc("@label:spinbox, name, 'a row'", "Row")
                    from: 1

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height / 3

                    onValueChanged: if (scrollableDialog.rowCount !== value) scrollableDialog.rowCount = value
                }

                FormCard.FormSpinBoxDelegate {
                    id: columnSpin

                    label: i18nc("@label:spinbox, name, 'a column'", "Column")
                    from: 1

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height / 3

                    onValueChanged: if (scrollableDialog.columnCount !== value) scrollableDialog.columnCount = value
                }

                FormCard.FormComboBoxDelegate {
                    id: alignmentBox

                    text: i18nc("@label:combobox", "Alignment :")

                    textRole: "text"
                    valueRole: "value"
                    model: [
                        { text: i18nc("@entry:combobox, alignment name", "Left"), value: "left" },
                        { text: i18nc("@entry:combobox, alignment name", "Center"), value: "center" },
                        { text: i18nc("@entry:combobox, alignment name", "Right"), value: "right" }
                    ]
                    currentIndex: 0

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height / 3
                }
            }
        }
    }

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onClosed: {
        rowCount = 1
        columnCount = 1
        optionsColumn.isExpended = false
    }
}
