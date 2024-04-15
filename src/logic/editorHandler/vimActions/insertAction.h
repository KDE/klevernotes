// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "abstractVimAction.h"

class InsertAction : public AbstractVimAction
{
public:
    InsertAction(VimHandler *vimHandler, EditorHandler *editorHandler, const char type, bool ready = false)
        : AbstractVimAction(vimHandler, editorHandler)
    {
        setReady(ready);
        setType(type);
    }

    void trigger() const override
    {
        m_vimHandler->setVimMode(VimHandler::EditorMode::Insert);
        move();
    }
};
