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
    void setNotInsert(const bool notInsert);
    int nonEOLPosition(const bool goLeft = true) const;

private:
    EditorHandler *m_editorHandler = nullptr;
    void setCursor();
    QTextCursor m_cursor;
    bool m_notInsert = true;

    int getNewPosition(const QTextCursor::MoveOperation operation);
    bool emptyBlock() const;
    int getBlockLastNonEmptyCharPosition() const;

    int moveW(const bool isShift);
    int moveB(const bool isShift);
    int moveE(const bool isShift);
    int moveTop();
    int moveBottom();
};
