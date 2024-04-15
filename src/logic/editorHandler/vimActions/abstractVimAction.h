// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "../vimHandler.h"
#include "logic/editorHandler/editorHandler.h"
#include <QDebug>
#include <tuple>
#include <vector>

class AbstractVimAction
{
public:
    AbstractVimAction(VimHandler *vimHandler, EditorHandler *editorHandler)
        : m_vimHandler(vimHandler)
        , m_editorHandler(editorHandler)
    {
    }

    virtual ~AbstractVimAction() = default;

    virtual void trigger() const
    {
        move();
    }

    virtual void move() const
    {
        for (const auto &movement : m_movementList) {
            int moveType;
            int repeat;
            bool isShift;
            std::tie(moveType, repeat, isShift) = movement;
            const int position = m_vimHandler->getMovementOperator()->move(moveType, repeat, isShift);
            m_editorHandler->moveCursorTo(position);
        }
    }

    virtual void addMovement(const std::tuple<int, int, bool> &movement)
    {
        m_movementList.push_back(movement);
    }

    virtual bool isReady() const
    {
        return m_ready;
    }

    virtual void setReady(const bool ready)
    {
        m_ready = ready;
    }

    virtual char getType() const
    {
        return m_type;
    }

    virtual void setType(const char type)
    {
        m_type = type;
    }

    VimHandler *m_vimHandler = nullptr;
    EditorHandler *m_editorHandler = nullptr;

private:
    char m_type = '0';
    bool m_ready = false;
    std::vector<std::tuple<int, int, bool>> m_movementList;
};
