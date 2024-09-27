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
QString rstrip(const QString &str);

void removeDelims(const MdEditor::EditorHandler *editor, const int delimType);

bool addDelims(const MdEditor::EditorHandler *editor, const int delimType);

void handleTabPressed(const MdEditor::EditorHandler *editor, const bool useSpaceForTab, const int spaceForTab, const bool backtab);

void handleReturnPressed(const MdEditor::EditorHandler *editor, const MD::ListItem<MD::QStringTrait> *listItem, const bool useSpaceForTab, const int modifier);
}
