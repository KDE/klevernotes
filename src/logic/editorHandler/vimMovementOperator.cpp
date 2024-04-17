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

int VimMovementOperator::getBlockLastNonEmptyCharPosition() const
{
    const QTextBlock block = m_cursor.block();
    const QRegularExpression traillingWhiteSpace = QRegularExpression(QStringLiteral("\\s+$"));
    const int len = block.text().remove(traillingWhiteSpace).length() - 1;
    return block.position() + (0 < len ? len : 0);
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
    const int previousPosition = m_cursor.position();
    if (isShift) {
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

    const int blockLastCharPosition = getBlockLastNonEmptyCharPosition();
    const int docLastCharPosition = m_editorHandler->getLastCharPosition() - 1;

    if (previousPosition == docLastCharPosition) {
        return previousPosition;
    }

    int tempCursorPosition = getNewPosition(QTextCursor::EndOfWord);
    if (previousPosition == tempCursorPosition - 1 || previousPosition == tempCursorPosition) {
        tempCursorPosition = moveW(false);
        if (blockLastCharPosition < tempCursorPosition && blockLastCharPosition != previousPosition) {
            return blockLastCharPosition - 1;
        }
        if (emptyBlock()) {
            moveW(true);
            tempCursorPosition = getNewPosition(QTextCursor::EndOfWord);
        }
        if (m_editorHandler->trimmedCharAt(tempCursorPosition - 1).isEmpty()) {
            tempCursorPosition = getNewPosition(QTextCursor::EndOfWord);
        }
    }

    const int finalPos = emptyBlock() || tempCursorPosition == docLastCharPosition ? tempCursorPosition : tempCursorPosition - 1;

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
    case MoveType::Left: {
        int pos;
        int i = 0;
        while (i < repeat && pos != 0) {
            getNewPosition(QTextCursor::Left);
            pos = nonEOLPosition();
            i++;
        }
        return pos;
    }

    case MoveType::Right: {
        int pos;
        int i = 0;
        const int docLastCharPosition = m_editorHandler->getLastCharPosition();
        while (i < repeat && pos != docLastCharPosition) {
            getNewPosition(QTextCursor::Right);
            pos = nonEOLPosition(false);
            i++;
        }
        return pos;
    }

    case MoveType::Up: {
        int previousPos = -1;
        int pos = m_cursor.position();
        int i = 0;
        while (i < repeat && pos != previousPos) {
            previousPos = pos;
            getNewPosition(QTextCursor::Up);
            pos = nonEOLPosition();
            i++;
        }
        return pos;
    }

    case MoveType::Down: {
        int previousPos = -1;
        int pos = m_cursor.position();
        int i = 0;
        while (i < repeat && pos != previousPos) {
            previousPos = pos;
            getNewPosition(QTextCursor::Down);
            pos = nonEOLPosition(false);
            i++;
        }
        return pos;
    }

    case MoveType::StartOfBlock:
        return getNewPosition(QTextCursor::StartOfBlock);

    case MoveType::EndOfBlock: {
        int previousPos = -1;
        int pos = m_cursor.position();
        int i = 0;
        while (i < repeat - 1 && pos != previousPos) {
            previousPos = pos;
            getNewPosition(QTextCursor::Down);
            pos = nonEOLPosition(false);
            i++;
        }
        return getNewPosition(QTextCursor::EndOfBlock);
    }

    case MoveType::W: {
        int previousPos = -1;
        int pos = m_cursor.position();
        int i = 0;
        while (i < repeat && pos != previousPos) {
            previousPos = pos;
            pos = moveW(isShift);
            i++;
        }
        return pos;
    }

    case MoveType::B: {
        int previousPos = -1;
        int pos = m_cursor.position();
        int i = 0;
        while (i < repeat && pos != previousPos) {
            previousPos = pos;
            pos = moveB(isShift);
            i++;
        }
        return pos;
    }

    case MoveType::E: {
        int previousPos = -1;
        int pos = m_cursor.position();
        int i = 0;
        while (i < repeat && pos != previousPos) {
            previousPos = pos;
            pos = moveE(isShift);
            i++;
            // qDebug() << previousPos << pos;
        }
        return pos;
    }

    case MoveType::Top: {
        int previousPos = m_cursor.position();
        int pos = moveTop();
        int i = 0;
        while (i < repeat - 1 && pos != previousPos) {
            previousPos = pos;
            getNewPosition(QTextCursor::Down);
            pos = nonEOLPosition(false);
            i++;
        }
        return pos;
    }

    case MoveType::Bottom: {
        int previousPos = m_cursor.position();
        int pos = moveBottom();
        int i = 0;
        while (i < repeat - 1 && pos != previousPos) {
            previousPos = pos;
            getNewPosition(QTextCursor::Up);
            pos = nonEOLPosition(false);
            i++;
        }
        return pos;
    }
    default:
        qDebug() << "Unreachable MoveType";
        Q_UNREACHABLE();
    }
}

void VimMovementOperator::setNotInsert(const bool notInsert)
{
    m_notInsert = notInsert;
}

int VimMovementOperator::nonEOLPosition(const bool goLeft) const
{
    int pos = m_cursor.position();
    if (m_notInsert && m_cursor.atBlockEnd() && !emptyBlock()) {
        pos += goLeft ? -1 : 1;
    }

    return pos;
}
