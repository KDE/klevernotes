// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QTextCursor>

class EditorHandler;
class VimMovementOperator
{
public:
    VimMovementOperator(EditorHandler *editorHandler);
    ~VimMovementOperator() = default;

    enum MoveType {
        Right = Qt::UserRole + 1,
        Left,
        Up,
        Down,
        StartOfBlock,
        EndOfBlock,
        W,
        B,
        E,
        Top,
        Bottom,
    };

    void resetCursor();
    int move(const int moveType, const int repeat = 1, const bool isShift = false);

private:
    EditorHandler *m_editorHandler = nullptr;
    void setCursor();
    QTextCursor m_cursor;

    int getNewPosition(const QTextCursor::MoveOperation operation);
    bool emptyBlock() const;

    int moveW(const bool isShift);
    int moveB(const bool isShift);
    int moveE(const bool isShift);
    int moveTop();
    int moveBottom();
};
