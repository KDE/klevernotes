// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// PARTIALLY BASED ON: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quickcontrols2/texteditor?h=6.2.0

#include "editorHandler.h"
#include "vimHandler.h"
#include <QRegularExpression>
#include <QTextBlock>
#include <qlogging.h>

EditorHandler::EditorHandler(QObject *parent)
    : QObject(parent)
{
    m_vimHandler = new VimHandler(this);
}

// Acces QTextDocument
// ===================

QQuickTextDocument *EditorHandler::document() const
{
    return m_document;
}

void EditorHandler::setDocument(QQuickTextDocument *document)
{
    if (document == m_document)
        return;

    if (m_document)
        m_document->textDocument()->disconnect(this);
    m_document = document;
    if (m_document)
        connect(m_document->textDocument(), &QTextDocument::modificationChanged, this, &EditorHandler::modifiedChanged);

    m_textDocument = m_document->textDocument();
    Q_EMIT documentChanged();
}

QTextCursor EditorHandler::getCursor() const
{
    if (!m_textDocument)
        return QTextCursor();

    QTextCursor cursor = QTextCursor(m_textDocument);
    cursor.setPosition(m_cursorPosition);
    return cursor;
}

void EditorHandler::moveCursorTo(const int position, const bool visual)
{
    Q_EMIT m_vimHandler->getVimMode() == VimHandler::EditorMode::Visual ? moveSelection(position) : cursorPositionChanged(position);
}

int EditorHandler::getLastBlockPosition() const
{
    return m_textDocument->lastBlock().position();
}

int EditorHandler::getLastCharPosition() const
{
    const QTextBlock lastBlock = m_textDocument->lastBlock();
    return lastBlock.position() + lastBlock.length() - 1;
}

int EditorHandler::getCapturePosition(const QRegularExpression &reg, const int from) const
{
    const QString text = m_textDocument->toPlainText();
    const auto capture = reg.match(text, from < 0 ? m_cursorPosition : from);

    return capture.capturedStart();
}

QString EditorHandler::trimmedCharAt(const int position) const
{
    return QString(m_textDocument->characterAt(position)).trimmed();
}

// General Info
// ============

int EditorHandler::cursorPosition() const
{
    return m_cursorPosition;
}

void EditorHandler::setCursorPosition(const int cursorPosition)
{
    if (cursorPosition == m_cursorPosition)
        return;

    m_cursorPosition = cursorPosition;
}

int EditorHandler::selectionStart() const
{
    return m_selectionStart;
}

void EditorHandler::setSelectionStart(const int selectionStart)
{
    if (selectionStart == m_selectionStart)
        return;

    m_selectionStart = selectionStart;
    Q_EMIT selectionStartChanged();
}

int EditorHandler::selectionEnd() const
{
    return m_selectionEnd;
}

void EditorHandler::setSelectionEnd(const int selectionEnd)
{
    if (selectionEnd == m_selectionEnd)
        return;

    m_selectionEnd = selectionEnd;
    Q_EMIT selectionEndChanged();
}

// VimHandler
// ==========

bool EditorHandler::handleKeyPress(const int key, const int modifiers) const
{
    return m_vimHandler->handleKeyPress(key, modifiers);
}
