// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

// BASED ON FormTextFieldDelegate :
/*
 * Copyright 2022 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0

AbstractFormDelegate {
    id: root

    /**
     * @brief A label containing primary text that appears above and
     * to the left the text field.
     */
    required property string label

    /**
     * @brief set the maximum length of the text inside the TextField if maxLength > 0
     */
    property int maximumLength: 0

    /**
     * @brief This hold the activeFocus state of the internal TextField.
     */
    property alias fieldActiveFocus: textArea.activeFocus

    /**
     * @brief This hold the `readOnly` state of the internal TextField.
     */
    property alias readOnly: textArea.readOnly

    /**
     * @brief This property holds the `inputMethodHints` of the
     * internal TextField.
     *
     * This consists of hints on the expected content or behavior of
     * the text field, be it sensitive data, in a date format, or whether
     * the characters will be hidden, for example.
     *
     * @see <a href="https://doc.qt.io/qt-6/qml-qtquick-textinput.html#inputMethodHints-prop">TextInput.inputMethodHints</a>
     */
    property alias inputMethodHints: textArea.inputMethodHints

    /**
     * @brief This property holds the `wrapMode` of the
     * internal TextArea.
     *
     * @see <a href="https://doc.qt.io/qt-6/qml-qtquick-textedit.html#wrapMode-prop">TextEdit.wrapMode</a>
     */
    property alias wrapMode: textArea.wrapMode

    /**
     * @brief This property holds the `placeholderText` of the
     * internal TextField.
     *
     * This consists of secondary text shown by default on the text field
     * if no text has been written in it.
     */
    property alias placeholderText: textArea.placeholderText

    /**
     * @brief This property holds the current status message type of
     * the text field.
     *
     * This consists of an inline message with a colorful background
     * and an appropriate icon.
     *
     * The status property will affect the color of ::statusMessage used.
     *
     * Accepted values:
     * - `Kirigami.MessageType.Information` (blue color)
     * - `Kirigami.MessageType.Positive` (green color)
     * - `Kirigami.MessageType.Warning` (orange color)
     * - `Kirigami.MessageType.Error` (red color)
     *
     * default: `Kirigami.MessageType.Information` if ::statusMessage is set,
     * nothing otherwise.
     *
     * @see Kirigami.MessageType
     */
    property var status: Kirigami.MessageType.Information

    /**
     * @brief This property holds the current status message of
     * the text field.
     *
     * If this property is not set, no ::status will be shown.
     */
    property string statusMessage: ""

    /**
     * @This signal is emitted when the Return or Enter key is pressed.
     *
     * Note that if there is a validator or inputMask set on the text input,
     * the signal will only be emitted if the input is in an acceptable
     * state.
     */
    signal accepted();

    /**
     * @brief This signal is emitted when the Return or Enter key is pressed
     * or the text input loses focus.
     *
     * Note that if there is a validator or inputMask set on the text input
     * and enter/return is pressed, this signal will only be emitted if
     * the input follows the inputMask and the validator returns an
     * acceptable state.
     */
    signal editingFinished();

    /**
     * @brief This signal is emitted whenever the text is edited.
     *
     * Unlike textChanged(), this signal is not emitted when the text
     * is changed programmatically, for example, by changing the
     * value of the text property or by calling ::clear().
     */
    signal textEdited();

    background: null

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        RowLayout {
            spacing: Kirigami.Units.largeSpacing

            Label {
                text: label
                elide: Text.ElideRight
                color: root.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                wrapMode: Text.Wrap
                maximumLineCount: 2

                Layout.fillWidth: true
            }
            Label {
                text: metrics.label(textArea.text.length, root.maximumLength)
                font: Kirigami.Theme.smallFont
                color: textArea.text.length === root.maximumLength
                    ? Kirigami.Theme.neutralTextColor
                    : Kirigami.Theme.textColor

                horizontalAlignment: Text.AlignRight
                visible: root.maximumLength > 0

                Layout.margins: Kirigami.Units.smallSpacing
                Layout.preferredWidth: metrics.advanceWidth

                TextMetrics {
                    id: metrics

                    text: label(root.maximumLength, root.maximumLength)
                    font: Kirigami.Theme.smallFont

                    function label(current: int, maximum: int): string {
                        return i18nc("@label %1 is current text length, %2 is maximum length of text field", "%1/%2", current, maximum)
                    }
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextArea {
                id: textArea

                text: root.text
                placeholderText: root.placeholderText
                activeFocusOnTab: false

                Accessible.description: label

                onTextChanged: {
                    if (0 < root.maximumLength && root.maximumLength < text.length) {
                        const excess = text.length - root.maximumLength

                        let newCursorPosition = cursorPosition
                        let oldCursorPosition = newCursorPosition - excess

                        if (newCursorPosition < oldCursorPosition) {
                            newCursorPosition = oldCursorPosition
                            oldCursorPosition = cursorPosition
                        }

                        const a = text.substring(0, oldCursorPosition);
                        const b = text.substring(newCursorPosition);
                        text = a + b

                        cursorPosition = oldCursorPosition
                    }
                    root.text = text
                }
                onEditingFinished: { 
                    root.editingFinished()
                }
            }
        }

        Kirigami.InlineMessage {
            id: formErrorHandler

            text: root.statusMessage
            type: root.status
            visible: root.statusMessage.length > 0

            Layout.fillWidth: true
            Layout.topMargin: visible ? Kirigami.Units.smallSpacing : 0
        }
    }

    Accessible.role: Accessible.EditableText

    onActiveFocusChanged: { // propagate focus to the text field
        if (activeFocus) {
            textArea.forceActiveFocus();
        }
    }
    onClicked: { 
        textArea.forceActiveFocus()
    }

    /**
     * @brief Clears the contents of the text input and resets partial
     * text input from an input method.
     */
    function clear() {
        textArea.clear();
    }

    /**
     * Inserts text into the TextInput at position.
     */
    function insert(position: int, text: string) {
        textArea.insert(position, text);
    }
}
