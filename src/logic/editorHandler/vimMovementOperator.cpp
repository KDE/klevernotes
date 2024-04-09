// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// PARTIALLY BASED ON: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quickcontrols2/texteditor?h=6.2.0

#include "vimMovementOperator.h"
#include "editorHandler.h"
#include <QRegularExpression>
#include <QTextCursor>
#include <qlogging.h>

VimMovementOperator::VimMovementOperator(EditorHandler *editorHandler)
    : m_editorHandler(editorHandler)
{
    resetCursor();
}

void VimMovementOperator::resetCursor()
{
    m_cursor = m_editorHandler->getCursor();
}

int VimMovementOperator::getNewPosition(const QTextCursor::MoveOperation operation)
{
    m_cursor.movePosition(operation);
    return m_cursor.position();
}

bool VimMovementOperator::emptyBlock() const
{
    return m_cursor.atBlockEnd() && m_cursor.atBlockStart();
}

void VimMovementOperator::moveLeft()
{
    const int position = getNewPosition(QTextCursor::Left);
    m_editorHandler->moveCursorTo(position);
}

void VimMovementOperator::moveRight()
{
    const int position = getNewPosition(QTextCursor::Right);
    m_editorHandler->moveCursorTo(position);
}

void VimMovementOperator::moveUp()
{
    const int position = getNewPosition(QTextCursor::Up);
    m_editorHandler->moveCursorTo(position);
}

void VimMovementOperator::moveDown()
{
    const int position = getNewPosition(QTextCursor::Down);
    m_editorHandler->moveCursorTo(position);
}

void VimMovementOperator::moveStartOfBlock()
{
    const int position = getNewPosition(QTextCursor::StartOfBlock);
    m_editorHandler->moveCursorTo(position);
}

void VimMovementOperator::moveEndOfBlock()
{
    const int position = getNewPosition(QTextCursor::EndOfBlock);
    m_editorHandler->moveCursorTo(position);
}

void VimMovementOperator::moveW(const bool isShift)
{
    const int lastBlockPosition = m_editorHandler->getLastBlockPosition();
    bool atLastBlock = lastBlockPosition <= m_editorHandler->cursorPosition();
    if (isShift) {
        static const QRegularExpression whiteSpace = QRegularExpression(QStringLiteral("\\s"));

        const int capturePosition = m_editorHandler->getCapturePosition(whiteSpace);
        if (-1 < capturePosition) {
            m_cursor.setPosition(capturePosition);
        }
    }
    int tempCursorPosition = getNewPosition(QTextCursor::NextWord);

    QString currentChar = m_editorHandler->charAt(tempCursorPosition);
    // We want to stop on empty line. Line with only whitespace should be skipped
    while (currentChar.isEmpty() && !emptyBlock() && !atLastBlock) {
        tempCursorPosition = getNewPosition(QTextCursor::NextWord);
        currentChar = m_editorHandler->charAt(tempCursorPosition);
        atLastBlock = lastBlockPosition <= tempCursorPosition;
    }
    m_editorHandler->moveCursorTo(m_cursor.position());
}

void VimMovementOperator::moveB(const bool isShift)
{
}

void VimMovementOperator::moveE(const bool isShift)
{
}

void VimMovementOperator::move(const int type, const int repeat, const bool isShift)
{
    Q_UNUSED(repeat);
    switch (type) {
    case MoveType::Left:
        moveLeft();
        break;
    case MoveType::Right:
        moveRight();
        break;
    case MoveType::Up:
        moveUp();
        break;
    case MoveType::Down:
        moveDown();
        break;
    case MoveType::StartOfBlock:
        moveStartOfBlock();
        break;
    case MoveType::EndOfBlock:
        moveEndOfBlock();
        break;
    case MoveType::W:
        moveW(isShift);
        break;
    case MoveType::B:
        moveB(isShift);
        break;
    case MoveType::E:
        moveE(isShift);
        break;
        // const int lastBlockPosition = m_editorHandler->getLastBlockPosition();
        // bool atLastBlock = lastBlockPosition <= m_editorHandler->cursorPosition();
        // QTextCursor cursor = m_editorHandler->getCursor();
        // if (isShift) {
        //     static const QRegularExpression whiteSpace = QRegularExpression(QStringLiteral("\\s"));
        //
        //     const int capturePosition = m_editorHandler->getCapturePosition(whiteSpace);
        //     if (-1 < capturePosition) {
        //         cursor.setPosition(capturePosition);
        //     }
        // }
        // int tempCursorPosition = getNewPosition(cursor, QTextCursor::NextWord);
        //
        // QString currentChar = m_editorHandler->charAt(tempCursorPosition);
        // // We want to stop on empty line. Line with only whitespace should be skipped
        // while (currentChar.isEmpty() && !emptyBlock(cursor) && !atLastBlock) {
        //     tempCursorPosition = getNewPosition(cursor, QTextCursor::NextWord);
        //     currentChar = m_editorHandler->charAt(tempCursorPosition);
        //     atLastBlock = lastBlockPosition <= tempCursorPosition;
        // }
        // break;
    }
}
