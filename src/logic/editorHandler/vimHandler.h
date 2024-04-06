// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "vimActions/abstractVimAction.h"
#include <memory>
#include <qnamespace.h>
#include <vector>

class EditorHandler;
class Movement;

class VimHandler
{
public:
    explicit VimHandler(EditorHandler *editorHandler);

    // General info
    enum EditorMode {
        Normal = Qt::UserRole + 1,
        Insert,
        Visual,
    };
    int getVimMode() const;
    void setVimMode(const int mode);

    // Key handling
    bool handleKeyPress(const int key, const int modifiers);

private:
    EditorHandler *m_editorHandler;

    // General info
    int m_currentMode;
    bool m_isVisual = false;

    // Key handling
    // Numbers
    int m_repetitionBuffer = 0;
    int getRepetition();
    void addToRepetition(const int number);
    bool handleNumber(const int key);

    // Arrow
    bool handleArrows(const int key);

    // Actions
    enum ActionType {
        DefaultType = Qt::UserRole + 1,
        InsertType,
    };
    std::vector<std::unique_ptr<AbstractVimAction>> m_actionsList;
    void doActions();
};
