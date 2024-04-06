// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "vimHandler.h"
#include "editorHandler.h"
#include "vimActions/abstractVimAction.h"
#include "vimActions/insertAction.h"

VimHandler::VimHandler(EditorHandler *editorHandler)
    : m_editorHandler(editorHandler)
{
    setVimMode(EditorMode::Normal);
}

// General Info
// ============

int VimHandler::getVimMode() const
{
    return m_currentMode;
}

void VimHandler::setVimMode(const int mode)
{
    qDebug() << (mode == EditorMode::Insert);
    if (mode == m_currentMode)
        return;

    if (mode == EditorMode::Normal || mode == EditorMode::Insert || mode == EditorMode::Visual) {
        m_isVisual = mode == EditorMode::Visual;
        m_currentMode = mode;
        Q_EMIT m_editorHandler->modeChanged(m_currentMode);
    }
}

// Key handling
// ============

// Number
int VimHandler::getRepetition()
{
    return m_repetitionBuffer;
}

void VimHandler::addToRepetition(const int number)
{
    m_repetitionBuffer *= 10;
    m_repetitionBuffer += number;
}

bool VimHandler::handleNumber(const int key)
{
    // Range of 0-9 keys
    if (48 <= key && key <= 57) {
        addToRepetition(key - 48);
        return true;
    }

    return false;
}

// Arrow
bool VimHandler::handleArrows(const int key)
{
    // Arrow range
    if (16777234 <= key && key <= 16777237) {
        switch (key) {
        case Qt::Key_Right:
            // moveCursor(QTextCursor::Right);
            return true;

        case Qt::Key_Left:
            // moveCursor(QTextCursor::Left);
            return true;

        case Qt::Key_Up:
            // moveCursor(QTextCursor::Up);
            return true;

        case Qt::Key_Down:
            // moveCursor(QTextCursor::Down);
            return true;
        }
    }

    return false;
}

// Actions
void VimHandler::doActions()
{
    for (auto &actionInfo : m_actionsList) {
        // const ActionType actionType = actionInfo.second;

        actionInfo->trigger();
        // switch (actionType) {
        //     case ActionType::DefaultType: {
        //         break;
        //     }
        //
        //     case ActionType::InsertType: {
        //     }
        // }
    }
}

bool VimHandler::handleKeyPress(const int key, const int modifiers)
{
    if (key == Qt::Key_Escape) {
        setVimMode(EditorMode::Normal);
        Q_EMIT m_editorHandler->deselect();
        return true;
    }

    // "Disable" the shift arrow in insert mode
    if (handleArrows(key)) {
        return true;
    }

    if (m_currentMode == EditorMode::Insert) {
        return true;
    }

    if (handleNumber(key)) {
        return true;
    }

    const bool isShift = modifiers == Qt::ShiftModifier;
    switch (key) {
    case Qt::Key_I: {
        m_actionsList.push_back(std::make_unique<InsertAction>(this, m_editorHandler));
        break;
    }
    case Qt::Key_A: {
        break;
    }

    case Qt::Key_V: {
        break;
    }
    }

    doActions();
    return true;
}
