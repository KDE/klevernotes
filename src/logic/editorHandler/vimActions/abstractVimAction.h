// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "../vimMovement.h"
#include <QDebug>
#include <vector>

class EditorHandler;

class AbstractVimAction
{
public:
    virtual ~AbstractVimAction() = default;

    virtual void trigger() const
    {
        move();
    }

    virtual void move() const
    {
        qDebug() << "move from abstractVimAction";
        for (const auto &movement : m_movementList) {
            movement.move();
        }
    }

    virtual void addMovement(const VimMovement &movement)
    {
        m_movementList.push_back(movement);
    }

private:
    std::vector<VimMovement> m_movementList;
};
