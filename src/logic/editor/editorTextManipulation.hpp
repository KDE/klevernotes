/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#define HighlightDelim 8

namespace MdEditor
{
class EditorHandler;
}

namespace editorTextManipulation
{
void removeDelims(const MdEditor::EditorHandler *editor, const int delimType);

bool addDelims(const MdEditor::EditorHandler *editor, const int delimType);

void handleTabPressed(const MdEditor::EditorHandler *editor, const bool useSpaceForTab, const int spaceForTab, const bool backtab);
}
