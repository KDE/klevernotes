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

bool VimHandler::handleNormalSwitch(const int key)
{
    switch (key) {
    case Qt::Key_I:
        setMode(EditorMode::Insert);
        return true;
    case Qt::Key_A: {
        setMode(EditorMode::Insert);
        moveCursor(QTextCursor::Right);
        return true;
    }
    case Qt::Key_V:
        setMode(EditorMode::Visual);
        return true;
    }
    return false;
}

bool VimHandler::handleMove(const int key)
{
    const bool isNormal = m_currentMode == EditorMode::Normal;
    const bool isVisual = m_currentMode == EditorMode::Visual;
    if (isNormal || isVisual) {
        QTextCursor cursor = getCursor();
        switch (key) {
        case Qt::Key_H:
            moveCursor(QTextCursor::Left);
            return true;
        case Qt::Key_L:
            moveCursor(QTextCursor::Right);
            return true;
        case Qt::Key_J:
            moveCursor(QTextCursor::Down);
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
        case Qt::Key_W:
            moveCursor(QTextCursor::NextWord);
            return true;
        case Qt::Key_B: {
            moveCursor(QTextCursor::PreviousWord);

            const QTextCursor cursor = getCursor();

            // We went 1 line Up, we want the PreviousWord only if the line is not empty
            if (cursor.atBlockEnd() && !cursor.atBlockStart()) {
                moveCursor(QTextCursor::PreviousWord);
            }

            return true;
        }
        case Qt::Key_E:
            const int previousPosition = cursorPosition();
            moveCursor(QTextCursor::EndOfWord);

            // Prevent it from being stuck at the end of the word
            if (previousPosition == cursorPosition()) {
                moveCursor(QTextCursor::WordRight);
                moveCursor(QTextCursor::EndOfWord);

                QString previousChar = QString(m_textDocument->characterAt(cursorPosition() - 1)).trimmed();
                if (previousChar.isEmpty()) {
                    // Skip all whiteSpace
                    static const QRegularExpression nonWhiteSpace = QRegularExpression(QStringLiteral("\\S"));
                    const QTextCursor cursor = m_textDocument->find(nonWhiteSpace, cursorPosition());
                    setCursorPostion(cursor.position());
                    moveCursor(QTextCursor::EndOfWord);
                }
            }
            return true;
        }
    }
    return false;
}

bool VimHandler::handleKeyPress(const int key, const int modifiers)
{
    Q_UNUSED(modifiers);

    if (!earlyReturn(key)) {
        return false;
    }

    if (handleMove(key)) {
        return true;
    }

    if (m_currentMode == EditorMode::Normal) {
        if (handleNormalSwitch(key)) {
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
