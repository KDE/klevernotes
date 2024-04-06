// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

class EditorHandler;
class VimMovement
{
public:
    VimMovement(EditorHandler *editorHandler, const int key, const bool isShift, const int repeat = 1);

    void move() const;

private:
    EditorHandler *m_editorHandler = nullptr;
    int m_key;
    bool m_isShift;
    int m_repeat;
};
