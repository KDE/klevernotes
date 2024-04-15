// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "abstractVimAction.h"

class VisualAction : public AbstractVimAction
{
public:
    VisualAction(VimHandler *vimHandler, EditorHandler *editorHandler)
        : AbstractVimAction(vimHandler, editorHandler)
    {
        setReady(true);
        setType('v');
    }

    void trigger() const override
    {
        m_vimHandler->setVimMode(VimHandler::EditorMode::Visual);
    }
};
