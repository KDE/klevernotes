// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// PARTIALLY BASED ON: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quickcontrols2/texteditor?h=6.2.0

#include "vimHandler.h"
#include <QRegularExpression>
#include <QTextBlock>
// #include <QDebug>

VimHandler::VimHandler(QObject *parent)
    : QObject(parent)
{
    setMode(EditorMode::Normal);
}

bool VimHandler::earlyReturn(const int key)
{
    if (key == Qt::Key_Escape) {
        setMode(EditorMode::Normal);

        Q_EMIT deselect();
    } else if (m_currentMode == EditorMode::Insert) {
        return false;
    }

    return true;
}

bool VimHandler::handleNormalSwitch(const int key, const int modifiers)
{
    const bool isShift = modifiers == Qt::ShiftModifier;
    switch (key) {
    case Qt::Key_I:
        setMode(EditorMode::Insert);
        if (isShift) {
            moveCursor(QTextCursor::StartOfBlock);
        }
        return true;
    case Qt::Key_A: {
        setMode(EditorMode::Insert);
        if (isShift) {
            moveCursor(QTextCursor::EndOfBlock);
        } else {
            moveCursor(QTextCursor::Right);
        }
        return true;
    }
    case Qt::Key_V:
        setMode(EditorMode::Visual);
        return true;
    }
    return false;
}

bool VimHandler::handleMove(const int key, const int modifiers)
{
    const bool isNormal = m_currentMode == EditorMode::Normal;
    const bool isVisual = m_currentMode == EditorMode::Visual;
    if (isNormal || isVisual) {
        const bool isShift = modifiers == Qt::ShiftModifier;
        switch (key) {
        case Qt::Key_Left:
            moveCursor(QTextCursor::Left);
            return true;
        case Qt::Key_H:
            moveCursor(QTextCursor::Left);
            return true;

        case Qt::Key_Right:
            moveCursor(QTextCursor::Right);
            return true;
        case Qt::Key_L:
            moveCursor(QTextCursor::Right);
            return true;

        case Qt::Key_Down:
            moveCursor(QTextCursor::Down);
            return true;
        case Qt::Key_J:
            moveCursor(QTextCursor::Down);
            return true;

        case Qt::Key_Up:
            moveCursor(QTextCursor::Up);
            return true;
        case Qt::Key_K:
            moveCursor(QTextCursor::Up);
            return true;

        case Qt::Key_Dollar:
            moveCursor(QTextCursor::EndOfBlock);
            return true;
        case Qt::Key_0:
            moveCursor(QTextCursor::StartOfBlock);
            return true;

        case Qt::Key_W: {
            const int lastBlockPosition = m_textDocument->lastBlock().position();
            bool atLastBlock = lastBlockPosition <= cursorPosition();
            if (isShift) {
                static const QRegularExpression whiteSpace = QRegularExpression(QStringLiteral("\\s"));

                const QString text = m_textDocument->toPlainText();
                const auto capture = whiteSpace.match(text, cursorPosition());
                if (capture.hasMatch()) {
                    setCursorPostion(capture.capturedStart());
                }
            }
            moveCursor(QTextCursor::NextWord);

            QString currentChar = QString(m_textDocument->characterAt(cursorPosition())).trimmed();
            // We want to stop on empty line. Line with only whitespace should be skipped
            while (currentChar.isEmpty() && !emptyBlock() && !atLastBlock) {
                moveCursor(QTextCursor::NextWord);
                currentChar = QString(m_textDocument->characterAt(cursorPosition())).trimmed();
                atLastBlock = lastBlockPosition <= cursorPosition();
            }
            return true;
        }

        case Qt::Key_B: {
            const int previousPosition = cursorPosition();
            moveCursor(QTextCursor::StartOfWord);
            if (emptyBlock()) {
                moveCursor(QTextCursor::PreviousWord);
                moveCursor(QTextCursor::StartOfWord);
            }

            int currentPosition = cursorPosition();
            QString currentChar = QString(m_textDocument->characterAt(cursorPosition())).trimmed();
            while ((previousPosition == currentPosition || currentChar.isEmpty()) && currentPosition != 0 && !emptyBlock()) {
                moveCursor(QTextCursor::PreviousWord);
                moveCursor(QTextCursor::StartOfWord);
                currentChar = QString(m_textDocument->characterAt(cursorPosition())).trimmed();
                currentPosition = cursorPosition();
            }

            if (isShift) {
                QString previousChar = QString(m_textDocument->characterAt(cursorPosition() - 1)).trimmed();

                while (cursorPosition() != 0 && !previousChar.isEmpty()) {
                    moveCursor(QTextCursor::PreviousWord);
                    previousChar = QString(m_textDocument->characterAt(cursorPosition() - 1)).trimmed();
                }
            }

            return true;
        }

        case Qt::Key_E: {
            const QTextBlock lastBlock = m_textDocument->lastBlock();
            const int endPosition = lastBlock.position() + lastBlock.length() - 1;
            const int previousPosition = cursorPosition();

            if (previousPosition == endPosition) {
                return true;
            }
            moveCursor(QTextCursor::EndOfWord);

            // Prevent it from being stuck at the end of the word
            if (previousPosition == cursorPosition() - 1 || previousPosition == cursorPosition()) {
                moveCursor(QTextCursor::WordRight);

                if (cursorPosition() == endPosition) {
                    return true;
                }
                moveCursor(QTextCursor::EndOfWord);

                const QString previousChar = QString(m_textDocument->characterAt(cursorPosition() - 1)).trimmed();
                if (previousChar.isEmpty()) {
                    // Skip all whiteSpace
                    static const QRegularExpression nonWhiteSpace = QRegularExpression(QStringLiteral("\\S"));
                    const QTextCursor cursor = m_textDocument->find(nonWhiteSpace, cursorPosition());
                    setCursorPostion(cursor.position());
                    moveCursor(QTextCursor::EndOfWord);
                }
                if (isShift) {
                    QString nextChar = QString(m_textDocument->characterAt(cursorPosition())).trimmed();
                    while (!nextChar.isEmpty() && !emptyBlock() && cursorPosition() != endPosition) {
                        moveCursor(QTextCursor::EndOfWord);
                        nextChar = QString(m_textDocument->characterAt(cursorPosition())).trimmed();
                    }
                }
            }
            if (cursorPosition() != endPosition) {
                setCursorPostion(cursorPosition() - 1); // The actual last char of the word
            }
            return true;
        }

        case Qt::Key_G:
            const int positionInBlock = getCursor().positionInBlock();
            if (isShift) {
                if (m_keyBuffer.isEmpty() || m_keyBuffer[0] != Qt::Key_G) {
                    const QTextBlock lastBlock = m_textDocument->lastBlock();
                    const int lastBlockLength = lastBlock.length() - 1;
                    const int lastBlockPosition = lastBlock.position();

                    const int futurePosition = lastBlockPosition + positionInBlock <= lastBlockLength ? positionInBlock : lastBlockLength;
                    setCursorPostion(futurePosition);
                } else {
                    m_keyBuffer.clear();
                }
                return true;
            }

            if (m_keyBuffer.isEmpty()) {
                m_keyBuffer.append(Qt::Key_G);
                return true;
            }
            if (m_keyBuffer[0] == Qt::Key_G) {
                const int firstBlockLength = m_textDocument->firstBlock().length() - 1;

                if (positionInBlock <= firstBlockLength) {
                    setCursorPostion(positionInBlock);
                } else {
                    setCursorPostion(0);
                    moveCursor(QTextCursor::EndOfBlock);
                }
            }
            m_keyBuffer.clear();
            return true;
        }
    }
    return false;
}

bool VimHandler::handleKeyPress(const int key, const int modifiers)
{
    if (!earlyReturn(key)) {
        return false;
    }

    if (handleMove(key, modifiers)) {
        return true;
    }

    if (m_currentMode == EditorMode::Normal) {
        if (handleNormalSwitch(key, modifiers)) {
            return true;
        }
        return true;
    }
    return true;
}

QTextCursor VimHandler::getCursor() const
{
    if (!m_textDocument)
        return QTextCursor();

    QTextCursor cursor = QTextCursor(m_textDocument);
    if (m_selectionStart != m_selectionEnd) {
        cursor.setPosition(m_selectionStart);
        cursor.setPosition(m_selectionEnd, QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(m_cursorPosition);
    }
    return cursor;
}

void VimHandler::moveCursor(const QTextCursor::MoveOperation moveOperation)
{
    QTextCursor cursor = getCursor();

    cursor.movePosition(moveOperation);

    setCursorPostion(cursor.position());
}

bool VimHandler::emptyBlock() const
{
    const QTextCursor cursor = getCursor();
    return cursor.atBlockEnd() && cursor.atBlockStart();
}

int VimHandler::getMode() const
{
    return m_currentMode;
}

void VimHandler::setMode(const int mode)
{
    if (mode == m_currentMode)
        return;

    if (mode == EditorMode::Normal || mode == EditorMode::Insert || mode == EditorMode::Visual) {
        m_currentMode = mode;
        Q_EMIT modeChanged(m_currentMode);
    }
}

void VimHandler::setVimOn(const bool vimOn)
{
    m_vimOn = vimOn;
}

bool VimHandler::getVimOn() const
{
    return m_vimOn;
}

QQuickTextDocument *VimHandler::document() const
{
    return m_document;
}

void VimHandler::setDocument(QQuickTextDocument *document)
{
    if (document == m_document)
        return;

    if (m_document)
        m_document->textDocument()->disconnect(this);
    m_document = document;
    if (m_document)
        connect(m_document->textDocument(), &QTextDocument::modificationChanged, this, &VimHandler::modifiedChanged);

    m_textDocument = m_document->textDocument();
    Q_EMIT documentChanged();
}

int VimHandler::cursorPosition() const
{
    return m_cursorPosition;
}

void VimHandler::setCursorPostion(const int cursorPosition)
{
    if (cursorPosition == m_cursorPosition)
        return;

    m_cursorPosition = cursorPosition;
    Q_EMIT cursorPositionChanged(cursorPosition);
}

int VimHandler::selectionStart() const
{
    return m_selectionStart;
}

void VimHandler::setSelectionStart(const int selectionStart)
{
    if (selectionStart == m_selectionStart)
        return;

    m_selectionStart = selectionStart;
    Q_EMIT selectionStartChanged();
}

int VimHandler::selectionEnd() const
{
    return m_selectionEnd;
}

void VimHandler::setSelectionEnd(const int selectionEnd)
{
    if (selectionEnd == m_selectionEnd)
        return;

    m_selectionEnd = selectionEnd;
    Q_EMIT selectionEndChanged();
}
