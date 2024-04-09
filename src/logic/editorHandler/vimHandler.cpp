// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "vimHandler.h"
#include "editorHandler.h"
#include "logic/editorHandler/vimMovementOperator.h"
#include "vimActions/abstractVimAction.h"
#include "vimActions/insertAction.h"
#include "vimActions/visualAction.h"
#include <tuple>

VimHandler::VimHandler(EditorHandler *editorHandler)
    : m_editorHandler(editorHandler)
{
    setVimMode(EditorMode::Normal);
    m_movementOperator = new VimMovementOperator(m_editorHandler);
}

// General Info
// ============

int VimHandler::getVimMode() const
{
    return m_currentMode;
}

void VimHandler::setVimMode(const int mode)
{
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
    const int returnValue = m_repetitionBuffer;
    m_repetitionBuffer = 0;
    return returnValue;
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
        if (!m_repetitionBuffer) { // Doesn't grab 0 when the buffer is empty
            return false;
        }
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
        const int repeat = getRepetition();
        switch (key) {
        case Qt::Key_Right: {
            handleMovement(VimMovementOperator::MoveType::Right, repeat);
            return true;
        }

        case Qt::Key_Left: {
            handleMovement(VimMovementOperator::MoveType::Left, repeat);
            return true;
        }

        case Qt::Key_Up: {
            handleMovement(VimMovementOperator::MoveType::Up, repeat);
            return true;
        }

        case Qt::Key_Down: {
            handleMovement(VimMovementOperator::MoveType::Down, repeat);
            return true;
        }
        }
    }

    return false;
}

// Actions
void VimHandler::doActions()
{
    if (!m_actionsList.empty() && m_actionsList.at(m_actionsList.size() - 1)->isReady()) {
        for (const auto &action : m_actionsList) {
            action->trigger();
        }
        m_actionsList.clear();
    }
}

bool VimHandler::handleKeyPress(const int key, const int modifiers)
{
    // Start with a fresh cursor for each key
    m_movementOperator->resetCursor();

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
    case Qt::Key_H: {
        const int repeat = getRepetition();
        handleMovement(VimMovementOperator::MoveType::Left, repeat);
        return true;
    }

    case Qt::Key_L: {
        const int repeat = getRepetition();
        handleMovement(VimMovementOperator::MoveType::Right, repeat);
        return true;
    }

    case Qt::Key_J: {
        const int repeat = getRepetition();
        handleMovement(VimMovementOperator::MoveType::Down, repeat);
        return true;
    }

    case Qt::Key_K: {
        const int repeat = getRepetition();
        handleMovement(VimMovementOperator::MoveType::Up, repeat);
        return true;
    }

    case Qt::Key_Dollar: {
        const int repeat = getRepetition();
        handleMovement(VimMovementOperator::MoveType::EndOfBlock, repeat);
        return true;
    }

    case Qt::Key_0: {
        const int repeat = getRepetition();
        handleMovement(VimMovementOperator::MoveType::StartOfBlock, repeat);
        return true;
    }

    case Qt::Key_W: {
        const int repeat = getRepetition();
        handleMovement(VimMovementOperator::MoveType::W, repeat, isShift);
        return true;
    }

    case Qt::Key_B: {
        const int repeat = getRepetition();
        handleMovement(VimMovementOperator::MoveType::B, repeat, isShift);
        return true;
    }

    case Qt::Key_E: {
        const int repeat = getRepetition();
        handleMovement(VimMovementOperator::MoveType::E, repeat, isShift);
        return true;
    }

    case Qt::Key_I: {
        if (m_currentMode == EditorMode::Visual) {
            return true;
        }
        // There's no point in repeating this action
        getRepetition();
        auto action = std::make_unique<InsertAction>(this, true);
        if (isShift) {
            const auto movement = std::make_tuple(VimMovementOperator::MoveType::StartOfBlock, 1, isShift);
            action->addMovement(movement);
        }
        m_actionsList.push_back(std::move(action));
        break;
    }
    case Qt::Key_A: {
        if (m_currentMode == EditorMode::Visual) {
            return true;
        }
        // There's no point in repeating this action
        getRepetition();
        auto action = std::make_unique<InsertAction>(this, true);
        const auto moveType = isShift ? VimMovementOperator::MoveType::EndOfBlock : VimMovementOperator::MoveType::Right;
        const auto movement = std::make_tuple(moveType, 1, isShift);
        action->addMovement(movement);
        m_actionsList.push_back(std::move(action));
        break;
    }

    case Qt::Key_V: {
        // There's no point in repeating this action
        getRepetition();
        m_actionsList.push_back(std::make_unique<VisualAction>(this));
        break;
    }
    }

    doActions();
    return true;
}

// Movement
void VimHandler::handleMovement(const int moveType, const int repeat, const bool isShift)
{
    if (!m_actionsList.empty()) {
        const auto movement = std::make_tuple(moveType, repeat, isShift);
        m_actionsList.at(m_actionsList.size() - 1)->addMovement(movement);
        return;
    }
    m_movementOperator->move(moveType, repeat, isShift);
}

VimMovementOperator *VimHandler::getMovementOperator() const
{
    return m_movementOperator;
}
