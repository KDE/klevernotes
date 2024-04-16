// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardDialog {
    id: root

    property int rowCount: 1
    property int columnCount: 1
    property string alignment: optionsColumn.isExpanded ? alignmentBox.currentValue : "left"
    property int hoveredRow: 0
    property int hoveredColumn: 0

    title: i18nc("@title:dialog, table => an html table", "Table creation")

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onRowCountChanged: if (rowCount !== rowSpin.value) {
        rowSpin.value = rowCount
    }
    onColumnCountChanged: if (columnCount !== columnSpin.value) {
        columnSpin.value = columnCount
    }
    onRejected: {
        close()
    }
    onClosed: {
        rowCount = 1
        columnCount = 1
        optionsColumn.isExpanded = false
    }

    contentItem: ColumnLayout {
        id: itemLayout

        property int generalTiming: Kirigami.Units.longDuration
        
        height: Kirigami.Units.gridUnit * 18
        spacing: 0

        Controls.Label {
            text: i18nc("@label, display the final size of the table %1 is the number of row and %2 the number of column","Size: %1 x %2", rowCount, columnCount)

            Layout.alignment: Qt.AlignTop
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.margins: 0
            Layout.leftMargin: Kirigami.Units.gridUnit
            Layout.rightMargin: Kirigami.Units.gridUnit
        }

        ItemDelegate {
            id: mainHolder

            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 15
            Layout.margins: 0
            Layout.leftMargin: Kirigami.Units.largeSpacing * 3
            Layout.rightMargin: Kirigami.Units.largeSpacing * 3

            states: State {
                name: "invisible"
                when: optionsColumn.isExpanded

                PropertyChanges { target: mainHolder; Layout.preferredHeight: 0 }
                PropertyChanges { target: mainHolder; opacity: 0 }
            }

            Behavior on Layout.preferredHeight {
                NumberAnimation { duration: itemLayout.generalTiming }
            }
            Behavior on opacity {
                NumberAnimation { duration: itemLayout.generalTiming - 20 }
            }

            onHoveredChanged: {
                root.hoveredRow = 0
                root.hoveredColumn = 0
            }
            GridLayout {
                id: holderLayout
                rows: 10
                columns: 5
                rowSpacing: 0
                columnSpacing: 0

                anchors.fill: parent

                Repeater {
                    model: holderLayout.rows * holderLayout.columns

                    delegate: ItemDelegate {
                        id: box

                        readonly property int visualColumn: (index % holderLayout.columns) + 1
                        readonly property int visualRow: ((index - visualColumn + 1) / holderLayout.columns) + 1

                        property bool insideSelectedBound: visualRow <= root.rowCount && visualColumn <= root.columnCount

                        property bool insideHoveredBound: visualRow <= root.hoveredRow && visualColumn <= root.hoveredColumn

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
                                    !root.isHovered && box.insideSelectedBound
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
                            // root.isHovered = hovered
                            if (hovered) {
                                root.hoveredRow = visualRow
                                root.hoveredColumn = visualColumn
                            }
                        }
                        onClicked: {
                            root.rowCount = visualRow
                            root.columnCount = visualColumn
                        }
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
            arrowDirection: optionsColumn.isExpanded ? Qt.DownArrow : Qt.UpArrow

            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.alignment: optionsColumn.isExpanded ? Qt.AlignTop : Qt.AlignBottom
            Layout.bottomMargin: optionsColumn.isExpanded ? 0 : -Kirigami.Units.smallSpacing


            onClicked: {
                optionsColumn.isExpanded = !optionsColumn.isExpanded

                if (root.rowCount > mainHolder.rows) root.rowCount = mainHolder.rows
                if (root.columnCount > mainHolder.columns) root.columnCount = mainHolder.columns
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.smallSpacing
            visible: optionsColumn.isExpanded
        }

        Item {
            id: optionsColumn

            property bool isExpanded: false

            Layout.fillWidth: true
            Layout.preferredHeight: 0
            Layout.alignment: Qt.AlignBottom
            opacity: 0

            states: State {
                name: "visible"
                when: optionsColumn.isExpanded
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

                    onValueChanged: if (root.rowCount !== value) root.rowCount = value
                }

                FormCard.FormSpinBoxDelegate {
                    id: columnSpin

                    label: i18nc("@label:spinbox, name, 'a column'", "Column")
                    from: 1

                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height / 3

                    onValueChanged: if (root.columnCount !== value) root.columnCount = value
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

}
