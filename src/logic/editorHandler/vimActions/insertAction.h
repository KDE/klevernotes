// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "../vimHandler.h"
#include "abstractVimAction.h"

class InsertAction : public AbstractVimAction
{
public:
    InsertAction(VimHandler *vimHandler, EditorHandler *editorHandler)
        : m_vimHandler(vimHandler)
        , m_editorHandler(editorHandler)
    {
    }

    void trigger() const override
    {
        m_vimHandler->setVimMode(VimHandler::EditorMode::Insert);

        move();
    }

private:
    VimHandler *m_vimHandler = nullptr;
    EditorHandler *m_editorHandler = nullptr;
};
