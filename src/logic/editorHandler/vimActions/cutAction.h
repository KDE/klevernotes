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
        reposition();
        m_vimHandler->setVimMode(VimHandler::EditorMode::Visual);
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
    std::vector<std::tuple<int, int, bool>> m_repositionList;
};
