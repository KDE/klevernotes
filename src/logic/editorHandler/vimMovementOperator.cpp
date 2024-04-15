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

int VimMovementOperator::moveW(const bool isShift)
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
                return nextBlockPosition;
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
    return tempCursorPosition;
}

int VimMovementOperator::moveB(const bool isShift)
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
    return tempCursorPosition;
}

int VimMovementOperator::moveE(const bool isShift)
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

        return m_cursor.position() - 1;
    }
    moveW(false);

    int tempCursorPosition = getNewPosition(QTextCursor::EndOfWord);
    const int finalPos = emptyBlock() ? tempCursorPosition : tempCursorPosition - 1;
    return finalPos;
}

int VimMovementOperator::moveTop()
{
    const int inBlockPos = m_cursor.positionInBlock();
    m_cursor.setPosition(0);

    const int blockSize = m_cursor.block().length() - 1; // -1 to remove the \n
    const int finalPos = inBlockPos <= blockSize ? inBlockPos : blockSize;

    return finalPos;
}

int VimMovementOperator::moveBottom()
{
    const int inBlockPos = m_cursor.positionInBlock();

    const int lastBlockPosition = m_editorHandler->getLastBlockPosition();
    const int lastCharPosition = m_editorHandler->getLastCharPosition();

    const int futureInBlockPos = lastBlockPosition + inBlockPos;
    const int finalPos = futureInBlockPos <= lastCharPosition ? futureInBlockPos : lastCharPosition;
    return finalPos;
}

int VimMovementOperator::move(const int type, const int repeat, const bool isShift)
{
    Q_UNUSED(repeat);
    switch (type) {
    case MoveType::Left:
        return getNewPosition(QTextCursor::Left);
    case MoveType::Right:
        return getNewPosition(QTextCursor::Right);
    case MoveType::Up:
        return getNewPosition(QTextCursor::Up);
    case MoveType::Down:
        return getNewPosition(QTextCursor::Down);
    case MoveType::StartOfBlock:
        return getNewPosition(QTextCursor::StartOfBlock);
    case MoveType::EndOfBlock:
        return getNewPosition(QTextCursor::EndOfBlock);
    case MoveType::W:
        return moveW(isShift);
    case MoveType::B:
        return moveB(isShift);
    case MoveType::E:
        return moveE(isShift);
    case MoveType::Top:
        return moveTop();
    case MoveType::Bottom:
        return moveBottom();
    default:
        Q_UNREACHABLE();
    }
}
