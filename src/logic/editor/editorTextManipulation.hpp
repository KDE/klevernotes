/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

// md4qt include.
#define MD4QT_QT_SUPPORT
#include "logic/parser/md4qt/doc.h"
#include "logic/parser/md4qt/traits.h"

#define HighlightDelim 8

enum keyModif {
    ShiftModifer = 1,
    AltModifier,
};

namespace MdEditor
{
class EditorHandler;
}

namespace editorTextManipulation
{
/**
 * @brief Strip the given string on the right.
 *
 * @param str The string to strip.
 * @return The given string stripped on the right.
 */
QString rstrip(const QString &str);

/**
 * @brief Remove the surrounding delim corresponding to the delimType.
 *
 * @param editor The editor handler.
 * @param delimType The type of delim to remove.
 */
void removeDelims(const MdEditor::EditorHandler *editor, const int delimType);

/**
 * @brief Add surrounding delim corresponding to the delimType.
 *
 * @param editor The editor handler.
 * @param delimType The type of delim to add.
 * @return True if the delims were added, false otherwise.
 */
bool addDelims(const MdEditor::EditorHandler *editor, const int delimType);

/**
 * @brief Performs custom actions when <Tab> is pressed.
 *
 * @param editor The editor handler.
 * @param useSpaceForTab Whether we use space for tab or not.
 * @param spaceForTab The number of space use to in place of <Tab>.
 * @param backtab Whether <Shift> was pressed when pressing tab (resulting in a backtab).
 */
void handleTabPressed(const MdEditor::EditorHandler *editor, const bool useSpaceForTab, const int spaceForTab, const bool backtab);

/**
 * @brief Performs custom actions when <Return> is pressed.
 *
 * @param editor The editor handler.
 * @param listItem A possible list item on which the cursor was when <Return> was pressed. Could be null.
 * @param useSpaceForTab Whether we use space for tab or not.
 * @param modifier The modifiers key pressed when <Return> was pressed.
 */
void handleReturnPressed(const MdEditor::EditorHandler *editor, const MD::ListItem<MD::QStringTrait> *listItem, const bool useSpaceForTab, const int modifier);
}
