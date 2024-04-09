// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "abstractVimAction.h"

class InsertAction : public AbstractVimAction
{
public:
    InsertAction(VimHandler *vimHandler, bool ready = false)
        : AbstractVimAction(vimHandler)
    {
        setReady(ready);
    }

    void trigger() const override
    {
        m_vimHandler->setVimMode(VimHandler::EditorMode::Insert);

        move();
    }

private:
    VimHandler *m_vimHandler = nullptr;
};
