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
    void move(const int moveType, const int repeat = 1, const bool isShift = false);

private:
    EditorHandler *m_editorHandler = nullptr;
    void setCursor();
    QTextCursor m_cursor;

    int getNewPosition(const QTextCursor::MoveOperation operation);
    bool emptyBlock() const;

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void moveStartOfBlock();
    void moveEndOfBlock();
    void moveW(const bool isShift);
    void moveB(const bool isShift);
    void moveE(const bool isShift);
    void moveTop();
    void moveBottom();
};
