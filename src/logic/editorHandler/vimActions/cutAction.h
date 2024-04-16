// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "abstractVimAction.h"

class CutAction : public AbstractVimAction
{
public:
    CutAction(VimHandler *vimHandler, EditorHandler *editorHandler, const char type, bool ready = false)
        : AbstractVimAction(vimHandler, editorHandler)
    {
        setReady(ready);
        setType(type);
    }

    void trigger() const override
    {
        const int selectionEnd = m_editorHandler->selectionEnd();
        const bool sameBlockSelection = m_editorHandler->sameBlockSelection();
        if (hasReposition()) {
            m_vimHandler->setVimMode(VimHandler::EditorMode::Normal);

            const int selectionStart = m_editorHandler->selectionStart();
            m_editorHandler->moveCursorTo(selectionStart);
            m_vimHandler->getMovementOperator()->resetCursor();

            reposition();
        }
        m_vimHandler->setVimMode(VimHandler::EditorMode::Visual);
        if (!sameBlockSelection) {
            m_editorHandler->moveCursorTo(selectionEnd);
        }
        m_vimHandler->getMovementOperator()->resetCursor();

        move();

        Q_EMIT m_editorHandler->cut();
        m_vimHandler->setVimMode(VimHandler::EditorMode::Normal);
    }

    void addRepositionning(const std::tuple<int, int, bool> &movement)
    {
        m_repositionList.push_back(movement);
    }

private:
    void reposition() const
    {
        for (const auto &movement : m_repositionList) {
            int moveType;
            int repeat;
            bool isShift;
            std::tie(moveType, repeat, isShift) = movement;
            const int position = m_vimHandler->getMovementOperator()->move(moveType, repeat, isShift);
            m_editorHandler->moveCursorTo(position);
        }
    }

    bool hasReposition() const
    {
        return !m_repositionList.empty();
    }
    std::vector<std::tuple<int, int, bool>> m_repositionList;
};
