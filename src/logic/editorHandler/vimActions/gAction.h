// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "abstractVimAction.h"

class GAction : public AbstractVimAction
{
public:
    GAction(VimHandler *vimHandler, EditorHandler *editorHandler, bool ready = false)
        : AbstractVimAction(vimHandler, editorHandler)
    {
        setReady(ready);
        setType('g');
    }

    void trigger() const override
    {
        move();
    }
};
