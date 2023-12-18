// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

/*
 * BASED ON FormCheckDelegate :
 * Copyright 2022 Devin Lin <devin@kde.org>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Templates 2.15 as T
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0

import org.kde.Klever 1.0

T.CheckDelegate {
    id: root

    property alias highlighterCombobox: highlighterCombobox
    property alias styleCombobox: styleCombobox
    /**
     * @brief A label containing secondary text that appears under the
     * inherited text property.
     *
     * This provides additional information shown in a faint gray color.
     */
    property string description: ""

    /**
     * @brief This property holds the padding after the leading item.
     */
    property real leadingPadding: Kirigami.Units.smallSpacing

    /**
     * @brief This property holds the padding before the trailing item.
     */
    property real trailingPadding: Kirigami.Units.smallSpacing

    /**
     * @brief This property allows to override the internal description
     * item (a QtQuick.Controls.Label) with a custom component.
     */
    property alias descriptionItem: internalDescriptionItem

    leftPadding: Kirigami.Units.gridUnit
    topPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
    bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
    rightPadding: Kirigami.Units.gridUnit

    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding

    focusPolicy: Qt.StrongFocus
    hoverEnabled: true
    background: FormDelegateBackground { control: root }

    contentItem: ColumnLayout {
        spacing: 0
        RowLayout {
            spacing: 0

            Controls.CheckBox {
                id: checkBoxItem

                focusPolicy: Qt.NoFocus // provided by delegate
                enabled: root.enabled
                checked: root.checked
                checkState: root.checkState
                nextCheckState: root.nextCheckState
                tristate: root.tristate

                Layout.rightMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing

                onToggled: {
                    root.toggle();
                    root.toggled();
                }
                onClicked: {
                    root.clicked()
                }
                onPressAndHold: {
                    root.pressAndHold()
                }
                onDoubleClicked: {
                    root.doubleClicked()
                }
            }

            ColumnLayout {
                spacing: Kirigami.Units.smallSpacing
                Layout.fillWidth: true

                Controls.Label {
                    text: root.text
                    color: root.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    maximumLineCount: 2

                    Layout.fillWidth: true
                }

                Controls.Label {
                    id: internalDescriptionItem

                    text: root.description
                    color: Kirigami.Theme.disabledTextColor
                    visible: root.description !== ""
                    wrapMode: Text.Wrap
                    textFormat: Text.StyledText // This way we can display a link to the supported Highlighters
                    onLinkActivated: Qt.openUrlExternally(link)

                    Layout.fillWidth: true
                }
            }
        }

        ColumnLayout {
            spacing: 0
            visible: checkBoxItem.checked

            FormComboBoxDelegate {
                id: highlighterCombobox

                text: i18nc("@label:combobox", "Highlighter")

                onModelChanged: if (model.length !== 0) {
                    const baseIndex = 0;

                    if (Config.codeSynthaxHighlighter.length === 0) {
                        highlighterCombobox.currentIndex = baseIndex
                        return
                    }

                    const inModelIndex = model.indexOf(Config.codeSynthaxHighlighter)

                    highlighterCombobox.currentIndex = inModelIndex === -1
                        ? baseIndex
                        : inModelIndex
                }
            } 

            FormComboBoxDelegate {
                id: styleCombobox
                
                property bool configStyleSet: false

                text: i18nc("@label:combobox", "Highlighter style")

                onCurrentValueChanged: if (!styleCombobox.configStyleSet) {
                    const baseIndex = 0;

                    if (Config.codeSynthaxHighlighterStyle.length === 0) {
                        styleCombobox.currentIndex = baseIndex
                        return
                    }

                    const inModelIndex = model.indexOf(Config.codeSynthaxHighlighterStyle)

                    styleCombobox.currentIndex = inModelIndex === -1
                        ? baseIndex
                        : inModelIndex

                    styleCombobox.configStyleSet = true
                }
            }
        }
    }

    Layout.fillWidth: true
}
