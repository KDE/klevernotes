// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "../vimHandler.h"
#include <QDebug>
#include <tuple>
#include <vector>

class AbstractVimAction
{
public:
    AbstractVimAction(VimHandler *vimHandler)
        : m_vimHandler(vimHandler)
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
            m_vimHandler->getMovementOperator()->move(moveType, repeat, isShift);
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

    VimHandler *m_vimHandler = nullptr;

private:
    bool m_ready = false;
    std::vector<std::tuple<int, int, bool>> m_movementList;
};
