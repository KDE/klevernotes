// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// PARTIALLY BASED ON: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quickcontrols2/texteditor?h=6.2.0

#include "vimMovementOperator.h"
#include "editorHandler.h"
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <qlogging.h>

VimMovementOperator::VimMovementOperator(EditorHandler *editorHandler)
    : m_editorHandler(editorHandler)
{
    setCursor();
}

void VimMovementOperator::setCursor()
{
    m_cursor = m_editorHandler->getCursor();
}

void VimMovementOperator::resetCursor()
{
    if (m_cursor.position() == -1) {
        setCursor();
    }
    const int currentPos = m_editorHandler->cursorPosition();
    m_cursor.setPosition(currentPos);
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
        const int previousPos = m_cursor.position();
        const int previousBlockNumber = m_cursor.blockNumber();
        m_cursor.movePosition(QTextCursor::NextBlock);
        const int nextBlockPosition = m_cursor.position();
        m_cursor.setPosition(previousPos);

        static const QRegularExpression whiteSpace = QRegularExpression(QStringLiteral("\\s"));

        const int capturePosition = m_editorHandler->getCapturePosition(whiteSpace, m_cursor.position());
        if (-1 < capturePosition) {
            const int currentBlockNumber = m_cursor.blockNumber();
            if (previousBlockNumber != currentBlockNumber && capturePosition != nextBlockPosition) {
                m_editorHandler->moveCursorTo(nextBlockPosition);
                return;
            }
            m_cursor.setPosition(capturePosition);
        }
    }
    int tempCursorPosition = getNewPosition(QTextCursor::NextWord);

    QString currentChar = m_editorHandler->trimmedCharAt(tempCursorPosition);
    // We want to stop on empty line. Line with only whitespace should be skipped
    while (currentChar.isEmpty() && !emptyBlock() && !atLastBlock) {
        tempCursorPosition = getNewPosition(QTextCursor::NextWord);
        currentChar = m_editorHandler->trimmedCharAt(tempCursorPosition);
        atLastBlock = lastBlockPosition <= tempCursorPosition;
    }
    m_editorHandler->moveCursorTo(m_cursor.position());
}

void VimMovementOperator::moveB(const bool isShift)
{
    const int previousPosition = m_cursor.position();
    int tempCursorPosition = getNewPosition(QTextCursor::StartOfWord);
    if (emptyBlock()) {
        tempCursorPosition = getNewPosition(QTextCursor::PreviousWord);
        tempCursorPosition = getNewPosition(QTextCursor::StartOfWord);
    }

    QString currentChar = m_editorHandler->trimmedCharAt(tempCursorPosition);
    while ((previousPosition == tempCursorPosition || currentChar.isEmpty()) && tempCursorPosition != 0 && !emptyBlock()) {
        tempCursorPosition = getNewPosition(QTextCursor::PreviousWord);
        tempCursorPosition = getNewPosition(QTextCursor::StartOfWord);
        currentChar = m_editorHandler->trimmedCharAt(tempCursorPosition);
    }

    if (isShift) {
        QString previousChar = m_editorHandler->trimmedCharAt(tempCursorPosition - 1);

        while (tempCursorPosition != 0 && !previousChar.isEmpty()) {
            tempCursorPosition = getNewPosition(QTextCursor::PreviousWord);
            previousChar = m_editorHandler->trimmedCharAt(tempCursorPosition - 1);
        }
    }

    m_editorHandler->moveCursorTo(tempCursorPosition);
}

void VimMovementOperator::moveE(const bool isShift)
{
    if (isShift) {
        const int previousPosition = m_cursor.position();
        static const QRegularExpression whiteSpace = QRegularExpression(QStringLiteral("\\s"));

        const int capturePosition = m_editorHandler->getCapturePosition(whiteSpace, previousPosition);
        if (-1 < capturePosition) {
            m_cursor.setPosition(capturePosition);
        }

        int currentPos = m_cursor.position() - 1;
        if (previousPosition == currentPos) {
            int tempCursorPosition = getNewPosition(QTextCursor::NextWord);
            static const QRegularExpression nonWhiteSpace = QRegularExpression(QStringLiteral("\\S"));
            int capturePosition = m_editorHandler->getCapturePosition(nonWhiteSpace, tempCursorPosition);
            if (-1 < capturePosition) {
                m_cursor.setPosition(capturePosition);
            }

            capturePosition = m_editorHandler->getCapturePosition(whiteSpace, m_cursor.position());
            if (-1 < capturePosition) {
                m_cursor.setPosition(capturePosition);
            }
        }

        m_editorHandler->moveCursorTo(m_cursor.position() - 1);
        return;
    }
    moveW(false);

    int tempCursorPosition = getNewPosition(QTextCursor::EndOfWord);
    const int finalPos = emptyBlock() ? tempCursorPosition : tempCursorPosition - 1;
    m_editorHandler->moveCursorTo(finalPos);
    return;
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
    }
}
