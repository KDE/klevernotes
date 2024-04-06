// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// PARTIALLY BASED ON: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quickcontrols2/texteditor?h=6.2.0

#include "vimMovement.h"
#include "editorHandler.h"

VimMovement::VimMovement(EditorHandler *editorHandler, const int key, const bool isShift, const int repeat)
    : m_editorHandler(editorHandler)
    , m_key(key)
    , m_isShift(isShift)
    , m_repeat(repeat)
{
}

void VimMovement::move() const
{
}
