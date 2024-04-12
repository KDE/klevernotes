// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "abstractVimAction.h"

class GAction : public AbstractVimAction
{
public:
    GAction(VimHandler *vimHandler, bool ready = false)
        : AbstractVimAction(vimHandler)
    {
        setReady(ready);
        setType('g');
    }

    void trigger() const override
    {
        move();
    }
};
