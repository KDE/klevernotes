// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// PARTIALLY BASED ON: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quickcontrols2/texteditor?h=6.2.0

#include "vimHandler.h"
#include <QKeyEvent>
#include <QRegularExpression>
#include <QTextBlock>

VimHandler::VimHandler(QObject *parent)
    : QObject(parent)
{
    setMode(EditorMode::Normal);
}

bool VimHandler::earlyReturn(const int key, const int modifiers, const bool visualMove)
{
    const bool isShift = modifiers == Qt::ShiftModifier;
    switch (key) {
    case Qt::Key_Escape:
        setMode(EditorMode::Normal);
        Q_EMIT deselect();
        return true;

    // Allow us to disable Shift+Arrow select, while keeping the move
    // We accept Shift+Arrow only while on visual mode
    // The rest is transformed in a simple arrow operation
    case Qt::Key_Right:
        if (visualMove && isShift) {
            return true;
        }
        moveCursor(QTextCursor::Right);
        moveCursorTo(m_tempCursorPosition);
        return true;

    case Qt::Key_Left:
        if (visualMove && isShift) {
            return true;
        }
        moveCursor(QTextCursor::Left);
        moveCursorTo(m_tempCursorPosition);
        return true;

    case Qt::Key_Up:
        if (visualMove && isShift) {
            return true;
        }
        moveCursor(QTextCursor::Up);
        moveCursorTo(m_tempCursorPosition);
        return true;

    case Qt::Key_Down:
        if (visualMove && isShift) {
            return true;
        }
        moveCursor(QTextCursor::Down);
        moveCursorTo(m_tempCursorPosition);
        return true;
    }

    if (m_currentMode == EditorMode::Insert) {
        return true;
    }

    return false;
}

bool VimHandler::handleNormalMode(const int key, const int modifiers)
{
    const bool isShift = modifiers == Qt::ShiftModifier;
    switch (key) {
    case Qt::Key_I:
        setMode(EditorMode::Insert);
        if (isShift) {
            moveCursor(QTextCursor::StartOfBlock);
            moveCursorTo(m_tempCursorPosition);
        }
        return true;

    case Qt::Key_A: {
        setMode(EditorMode::Insert);
        if (isShift) {
            moveCursor(QTextCursor::EndOfBlock);
            moveCursorTo(m_tempCursorPosition);
        } else {
            moveCursor(QTextCursor::Right);
            moveCursorTo(m_tempCursorPosition);
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
        case Qt::Key_H: {
            moveCursor(QTextCursor::Left);
            return true;
        }

        case Qt::Key_L: {
            moveCursor(QTextCursor::Right);
            return true;
        }

        case Qt::Key_J: {
            moveCursor(QTextCursor::Down);
            return true;
        }

        case Qt::Key_K: {
            moveCursor(QTextCursor::Up);
            return true;
        }

        case Qt::Key_Dollar: {
            moveCursor(QTextCursor::EndOfBlock);
            return true;
        }

        case Qt::Key_0: {
            moveCursor(QTextCursor::StartOfBlock);
            return true;
        }

        case Qt::Key_W: {
            const int lastBlockPosition = m_textDocument->lastBlock().position();
            bool atLastBlock = lastBlockPosition <= cursorPosition();
            if (isShift) {
                static const QRegularExpression whiteSpace = QRegularExpression(QStringLiteral("\\s"));

                const QString text = m_textDocument->toPlainText();
                const auto capture = whiteSpace.match(text, m_tempCursorPosition);
                if (capture.hasMatch()) {
                    m_tempCursorPosition = capture.capturedStart();
                }
            }
            moveCursor(QTextCursor::NextWord);

            QString currentChar = QString(m_textDocument->characterAt(m_tempCursorPosition)).trimmed();
            // We want to stop on empty line. Line with only whitespace should be skipped
            while (currentChar.isEmpty() && !emptyBlock() && !atLastBlock) {
                moveCursor(QTextCursor::NextWord);
                currentChar = QString(m_textDocument->characterAt(m_tempCursorPosition)).trimmed();
                atLastBlock = lastBlockPosition <= m_tempCursorPosition;
            }
            return true;
        }

        case Qt::Key_B: {
            const int previousPosition = m_tempCursorPosition;
            moveCursor(QTextCursor::StartOfWord);
            if (emptyBlock()) {
                moveCursor(QTextCursor::PreviousWord);
                moveCursor(QTextCursor::StartOfWord);
            }

            QString currentChar = QString(m_textDocument->characterAt(m_tempCursorPosition)).trimmed();
            while ((previousPosition == m_tempCursorPosition || currentChar.isEmpty()) && m_tempCursorPosition != 0 && !emptyBlock()) {
                moveCursor(QTextCursor::PreviousWord);
                moveCursor(QTextCursor::StartOfWord);
                currentChar = QString(m_textDocument->characterAt(m_tempCursorPosition)).trimmed();
            }

            if (isShift) {
                QString previousChar = QString(m_textDocument->characterAt(m_tempCursorPosition - 1)).trimmed();

                while (m_tempCursorPosition != 0 && !previousChar.isEmpty()) {
                    moveCursor(QTextCursor::PreviousWord);
                    previousChar = QString(m_textDocument->characterAt(m_tempCursorPosition - 1)).trimmed();
                }
            }
            return true;
        }

        case Qt::Key_E: {
            const QTextBlock lastBlock = m_textDocument->lastBlock();
            const int endPosition = lastBlock.position() + lastBlock.length() - 1;
            const int previousPosition = m_tempCursorPosition;

            if (previousPosition == endPosition) {
                return true;
            }
            moveCursor(QTextCursor::EndOfWord);

            // Prevent it from being stuck at the end of the word
            if (previousPosition == m_tempCursorPosition - 1 || previousPosition == m_tempCursorPosition) {
                moveCursor(QTextCursor::WordRight);

                if (m_tempCursorPosition == endPosition) {
                    return true;
                }
                moveCursor(QTextCursor::EndOfWord);

                const QString previousChar = QString(m_textDocument->characterAt(m_tempCursorPosition - 1)).trimmed();
                if (previousChar.isEmpty()) {
                    // Skip all whiteSpace
                    static const QRegularExpression nonWhiteSpace = QRegularExpression(QStringLiteral("\\S"));
                    const QTextCursor cursor = m_textDocument->find(nonWhiteSpace, m_tempCursorPosition);
                    m_tempCursorPosition = cursor.position();
                    moveCursor(QTextCursor::EndOfWord);
                }
                if (isShift) {
                    QString nextChar = QString(m_textDocument->characterAt(m_tempCursorPosition)).trimmed();
                    while (!nextChar.isEmpty() && !emptyBlock() && m_tempCursorPosition != endPosition) {
                        moveCursor(QTextCursor::EndOfWord);
                        nextChar = QString(m_textDocument->characterAt(m_tempCursorPosition)).trimmed();
                    }
                }
            }
            if (m_tempCursorPosition != endPosition) {
                m_tempCursorPosition--; // The actual last char of the word
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

                    const int futurePosition = lastBlockPosition + positionInBlock;
                    const int lastCharPosition = lastBlockPosition + lastBlockLength;

                    m_tempCursorPosition = futurePosition <= lastCharPosition ? futurePosition : lastCharPosition;
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
                    m_tempCursorPosition = positionInBlock;
                } else {
                    m_tempCursorPosition = 0;
                    moveCursor(QTextCursor::EndOfBlock);
                }
            }
            m_keyBuffer.clear();
            return true;
        }
    }
    return false;
}

bool VimHandler::handleKeyPress(const int key, const int modifiers, const bool visualMove)
{
    if (earlyReturn(key, modifiers, visualMove)) {
        if (visualMove && modifiers == Qt::ShiftModifier) {
            return false;
        }
        return true;
    }

    if (handleMove(key, modifiers)) {
        moveCursorTo(m_tempCursorPosition);
        return true;
    }

    if (m_currentMode == EditorMode::Normal) {
        handleNormalMode(key, modifiers);
        return true;
    }
    return true;
}

QTextCursor VimHandler::getCursor() const
{
    if (!m_textDocument)
        return QTextCursor();

    QTextCursor cursor = QTextCursor(m_textDocument);
    cursor.setPosition(m_tempCursorPosition);
    return cursor;
}

void VimHandler::moveCursor(const QTextCursor::MoveOperation moveOperation)
{
    QTextCursor cursor = getCursor();
    cursor.movePosition(moveOperation);

    m_tempCursorPosition = cursor.position();
}

void VimHandler::moveCursorTo(const int newPosition)
{
    if (m_currentMode == EditorMode::Visual) {
        const Qt::KeyboardModifier modifier = Qt::ShiftModifier;

        // Move the cursor vertically to reduce the amount of KeyEvent sent to the textArea
        // ==============================================================================
        // The cursor is positionned at the m_tempCursorPosition by default
        auto cursor = getCursor();
        cursor.setPosition(cursorPosition());

        int currentBlockPosition = cursor.block().position();

        const int futureBlockPosition = getCursor().block().position();

        const int skipBlockKey = currentBlockPosition <= futureBlockPosition ? Qt::Key_Down : Qt::Key_Up;
        const QTextCursor::MoveOperation skipBlockOpe = currentBlockPosition <= futureBlockPosition ? QTextCursor::Down : QTextCursor::Up;

        while (currentBlockPosition != futureBlockPosition) {
            cursor.movePosition(skipBlockOpe);
            currentBlockPosition = cursor.block().position();
            sendKeyEvent(skipBlockKey, modifier);
        }
        // ==============================================================================

        const int forwardPosition = newPosition - cursor.position();
        const int backwardPosition = cursor.position() - newPosition;

        const int key = 0 <= forwardPosition ? Qt::Key_Right : Qt::Key_Left;
        const int moveNumber = 0 <= forwardPosition ? forwardPosition : backwardPosition;

        // QML TextArea doesn't like backward selection to be done programmaticly
        // This is a far from perfect Workaround
        for (int i = 0; i < moveNumber; i++) {
            sendKeyEvent(key, modifier);
        }
    } else {
        Q_EMIT cursorPositionChanged(m_tempCursorPosition);
    }
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

void VimHandler::setTextArea(QQuickItem *textArea)
{
    if (textArea == m_textArea)
        return;

    m_textArea = textArea;

    if (m_textArea)
        m_textArea->installEventFilter(this);

    Q_EMIT textAreaChanged();
}

QQuickItem *VimHandler::textArea() const
{
    return m_textArea;
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

void VimHandler::setCursorPosition(const int cursorPosition)
{
    if (cursorPosition == m_cursorPosition)
        return;

    m_cursorPosition = cursorPosition;
    m_tempCursorPosition = cursorPosition;
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

void VimHandler::sendKeyEvent(const int key, const Qt::KeyboardModifier modifiers)
{
    QKeyEvent *event = new QKeyEvent(QKeyEvent::KeyPress, key, modifiers);
    QCoreApplication::postEvent(m_textArea, event);
}
